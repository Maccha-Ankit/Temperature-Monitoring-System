#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define SENSOR_PATH "/tmp/real_temperature_sensor"

void get_temperature(char *dest, size_t size)
{
    int sensor_fd = open(SENSOR_PATH, O_RDONLY);

    if (sensor_fd != -1)
    {
        char sensor_data[32];

        int bytes = read(sensor_fd, sensor_data, sizeof(sensor_data) - 1);
        close(sensor_fd);

        if (bytes > 0)
        {
            sensor_data[bytes] = '\0';
            sensor_data[strcspn(sensor_data, "\r\n")] = '\0';

            snprintf(dest, size, "Temperature: %s C", sensor_data);
            return;
        }
    }

    static int temperature = 30;
    static int direction = 1;

    temperature += direction;

    if (temperature >= 40)
        direction = -1;

    if (temperature <= 25)
        direction = 1;

    snprintf(dest, size, "Temperature: %d C", temperature);
}

void send_file(int client_fd, const char *filename, const char *content_type)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        const char *response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "File not found";

        write(client_fd, response, strlen(response));
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *content = malloc(file_size + 1);

    if (content == NULL)
    {
        fclose(file);
        return;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';

    fclose(file);

    char header[256];

    int header_len = snprintf(
        header,
        sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        content_type,
        file_size
    );

    write(client_fd, header, header_len);
    write(client_fd, content, file_size);

    free(content);
}

void handle_client(int client_fd)
{
    char buffer[1024];

    int bytes_read = read(
        client_fd,
        buffer,
        sizeof(buffer) - 1
    );

    if (bytes_read <= 0)
        return;

    buffer[bytes_read] = '\0';

    if (strncmp(buffer, "GET /data", 9) == 0)
    {
        char temp_str[64];

        get_temperature(
            temp_str,
            sizeof(temp_str)
        );

        char json[256];

        int json_len = snprintf(
            json,
            sizeof(json),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"reading\":\"%s\"}",
            temp_str
        );

        write(client_fd, json, json_len);
    }

    else if (strncmp(buffer, "GET /style.css", 14) == 0)
    {
        send_file(
            client_fd,
            "frontend/style.css",
            "text/css"
        );
    }

    else if (strncmp(buffer, "GET /script.js", 14) == 0)
    {
        send_file(
            client_fd,
            "frontend/script.js",
            "application/javascript"
        );
    }

    else if (strncmp(buffer, "GET /", 5) == 0)
    {
        send_file(
            client_fd,
            "frontend/index.html",
            "text/html"
        );
    }

    else
    {
        const char *response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "404 Not Found";

        write(
            client_fd,
            response,
            strlen(response)
        );
    }
}

int main()
{
    int server_fd;
    int client_fd;

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    int opt = 1;

    setsockopt(
        server_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    memset(
        &address,
        0,
        sizeof(address)
    );

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(
        server_fd,
        (struct sockaddr *)&address,
        sizeof(address)
    ) < 0)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf(
        "Industrial IoT Server running at "
        "http://localhost:%d\n",
        PORT
    );

    while (1)
    {
        client_fd = accept(
            server_fd,
            (struct sockaddr *)&address,
            (socklen_t *)&addrlen
        );

        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        handle_client(client_fd);

        close(client_fd);
    }

    close(server_fd);

    return 0;
}
