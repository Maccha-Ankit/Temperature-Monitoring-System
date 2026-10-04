# 🌡️ Industrial IoT Temperature Monitoring System

A lightweight **Industrial IoT temperature monitoring system** built using a **C-based TCP server** and a **HTML/CSS/JavaScript web dashboard**.

The system can obtain temperature data from a **real sensor** when available and automatically switch to **simulated temperature data** when physical sensor hardware is not available.

The temperature data is exposed through an HTTP endpoint and visualized on a live dashboard with statistics, history, sensor status, and a temperature trend graph.

---

## 🚀 Features

* 🌡️ Real-time temperature monitoring
* 🔌 TCP socket-based C backend
* 🌐 HTTP API for temperature data
* 📊 Live temperature graph
* 🟢 Server and sensor status indicators
* 📈 Minimum, maximum, and average temperature
* 📋 Recent temperature reading history
* 🔄 Automatic temperature updates every 2 seconds
* 🔧 Real sensor support
* 🧪 Simulation mode when a real sensor is unavailable
* 💻 Runs locally without requiring cloud infrastructure

---
<img src="images/architecture.png" alt="Industrial IoT System Architecture" width="900">

## 🏗️ System Architecture

```text
              ┌──────────────────────┐
              │    Temperature       │
              │       Sensor         │
              └──────────┬───────────┘
                         │
                 Real Sensor Data
                         │
                         ▼
              ┌──────────────────────┐
              │      C Server        │
              │                      │
              │  TCP Socket Server   │
              │      Port 8080       │
              └──────────┬───────────┘
                         │
                    HTTP GET /data
                         │
                         ▼
              ┌──────────────────────┐
              │      JSON Data       │
              │                      │
              │ Temperature: 31 C    │
              └──────────┬───────────┘
                         │
                         ▼
              ┌──────────────────────┐
              │    Web Dashboard     │
              │                      │
              │ HTML + CSS + JS      │
              │ Chart.js             │
              └──────────────────────┘
```

### Sensor fallback

```text
              Start
                │
                ▼
       Try to open sensor
                │
          ┌─────┴─────┐
          │           │
        Found       Not Found
          │           │
          ▼           ▼
    Read sensor    Simulate
      value        temperature
          │           │
          └─────┬─────┘
                ▼
        Create API response
                │
                ▼
          Send to browser
```

---

## 🛠️ Technology Stack

| Technology      | Purpose                                   |
| --------------- | ----------------------------------------- |
| **C**           | Backend server and sensor handling        |
| **TCP Socket**  | Client-server communication               |
| **HTTP**        | Communication between browser and backend |
| **JSON**        | Temperature data format                   |
| **HTML**        | Dashboard structure                       |
| **CSS**         | Dashboard styling                         |
| **JavaScript**  | API calls and UI updates                  |
| **Chart.js**    | Temperature visualization                 |
| **Linux / WSL** | Development environment                   |

---

## 📁 Project Structure

```text
iot-project/
│
├── frontend/
│   ├── index.html
│   ├── style.css
│   └── script.js
│
├── server/
│
└── server.c
```

---

## ⚙️ How the Backend Works

The backend is implemented in `server.c`.

### 1. Create the TCP socket

```c
server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

This creates an IPv4 TCP socket for communication with clients.

### 2. Bind the server

The server is configured to use:

```text
Port: 8080
```

The socket is then bound to the port using `bind()`.

### 3. Listen for clients

```c
listen(server_fd, 10);
```

The server starts listening for incoming client connections.

### 4. Accept a client

```c
client_fd = accept(...);
```

When the browser connects, the server accepts the connection.

### 5. Process the request

The server reads the HTTP request and checks whether the client requested:

```text
GET /data
```

### 6. Get temperature

The function:

```c
get_temperature()
```

first tries to read the real sensor.

If the sensor is unavailable, the server generates simulated temperature values.

### 7. Return JSON

The server sends a response similar to:

```json
{
    "reading": "Temperature: 31 C"
}
```

---

## 🌡️ Real Sensor and Simulation Mode

The backend attempts to read the sensor from:

```text
/tmp/real_temperature_sensor
```

If the file is available and contains a valid reading, that value is returned.

If the sensor is unavailable, the server uses a simulated temperature value.

The simulated value moves between approximately:

```text
25°C → 40°C
```

This allows the complete project to be demonstrated without physical IoT hardware.

---

## 🌐 API

### Get Temperature

**Endpoint**

```text
GET /data
```

**URL**

```text
http://localhost:8080/data
```

### Example response

```json
{
    "reading": "Temperature: 31 C"
}
```

The JavaScript frontend extracts the numerical temperature from this response.

---

## 💻 Frontend

The frontend is built using:

* HTML
* CSS
* JavaScript
* Chart.js

The JavaScript periodically requests new temperature data:

```javascript
setInterval(getTemperature, 2000);
```

This means the dashboard requests a new reading approximately every **2 seconds**.

---

## 📊 Dashboard

The dashboard displays:

### Current Temperature

Shows the latest temperature received from the backend.

### Minimum Temperature

Calculates the lowest recorded temperature during the current session.

### Maximum Temperature

Calculates the highest recorded temperature during the current session.

### Average Temperature

Calculates the average of the collected readings.

### Temperature Graph

The temperature trend is displayed using a line graph.

### Reading History

Recent temperature readings are displayed in a table.

### Server Status

The dashboard indicates whether the backend is:

```text
ONLINE
```

or

```text
OFFLINE
```

### Sensor Status

The interface also indicates whether the sensor is currently available.

---

## ▶️ Running the Project

### 1. Open the project

```bash
cd ~/iot-project
```

Check the files:

```bash
ls
```

You should have:

```text
frontend
server
server.c
```

---

### 2. Compile the C server

```bash
gcc server.c -o server
```

---

### 3. Start the server

```bash
./server
```

You should see:

```text
Industrial IoT Server running at http://localhost:8080
```

---

### 4. Test the API

Open another terminal and run:

```bash
curl http://localhost:8080/data
```

Example:

```text
{"reading":"Temperature: 31 C"}
```

---

### 5. Open the frontend

Open:

```text
frontend/index.html
```

in your browser.

The dashboard will start requesting temperature data from:

```text
http://localhost:8080/data
```

---

## 🔄 Complete Data Flow

```text
User opens dashboard
        │
        ▼
JavaScript starts
        │
        ▼
fetch("/data")
        │
        ▼
C TCP Server
        │
        ▼
handle_client()
        │
        ▼
get_temperature()
        │
        ├───────────────┐
        ▼               ▼
 Real Sensor       Simulation
        │               │
        └───────┬───────┘
                ▼
        Temperature value
                │
                ▼
          JSON response
                │
                ▼
        JavaScript receives
                │
        ┌───────┼────────┐
        ▼       ▼        ▼
     Current  Chart   History
     Temp     Update   Table
        │       │        │
        └───────┼────────┘
                ▼
          Dashboard
```

---

## 🧪 Testing Without a Physical Sensor

The project can be tested without IoT hardware because the backend contains a simulation fallback.

This makes it possible to develop and demonstrate:

* TCP communication
* HTTP requests
* JSON responses
* frontend integration
* live charts
* temperature statistics

without connecting an actual temperature sensor.

---

## 🔮 Future Improvements

The current project is a prototype that can be extended into a larger IoT monitoring system.

### Hardware Integration

Connect an actual temperature sensor to the Linux system and expose its readings to the backend.

### Multiple Sensors

Support multiple temperature sensors and identify each sensor separately.

### Database

Store historical temperature readings in a database for long-term analysis.

### Alerts

Add threshold-based alerts:

```text
Temperature > 40°C
        ↓
     WARNING
```

### MQTT

MQTT can be added for IoT-oriented messaging between sensors and backend services.

### Cloud Integration

The collected data could be sent to cloud platforms for remote monitoring and analytics.

### Security

A production deployment should add:

* HTTPS
* Authentication
* Input validation
* Access control

### Scalability

The current server handles clients sequentially. Multithreading, multiprocessing, or asynchronous networking could be introduced to support more concurrent clients.

---

## 🎯 Project Outcome

This project demonstrates a complete **sensor-to-dashboard Industrial IoT pipeline**:

```text
Sensor
  ↓
C Backend
  ↓
TCP Socket
  ↓
HTTP
  ↓
JSON
  ↓
JavaScript
  ↓
Dashboard
  ↓
Temperature Visualization
```

It combines **low-level C networking**, **sensor data handling**, **HTTP communication**, and **web-based visualization** into one end-to-end IoT monitoring system.

---

## 👨‍💻 Project

**Industrial IoT Temperature Monitoring System**

Built with:

```text
C • TCP Sockets • HTTP • JSON • HTML • CSS • JavaScript • Chart.js
```

---
