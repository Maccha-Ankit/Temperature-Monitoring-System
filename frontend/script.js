const currentTemp = document.getElementById("current-temp");
const minTemp = document.getElementById("min-temp");
const maxTemp = document.getElementById("max-temp");
const avgTemp = document.getElementById("avg-temp");

const serverStatus = document.getElementById("server-status");
const sensorStatus = document.getElementById("sensor-status");
const dataSource = document.getElementById("data-source");
const lastUpdated = document.getElementById("last-updated");

const readingTable = document.getElementById("reading-table");

let readings = [];

const ctx = document
    .getElementById("temperatureChart")
    .getContext("2d");

const chart = new Chart(ctx, {
    type: "line",

    data: {
        labels: [],

        datasets: [
            {
                label: "Temperature °C",

                data: [],

                borderWidth: 2,

                borderColor: "#38bdf8",

                backgroundColor: "rgba(56, 189, 248, 0.08)",

                fill: true,

                tension: 0.35,

                pointRadius: 3
            }
        ]
    },

    options: {
        responsive: true,

        maintainAspectRatio: false,

        animation: {
            duration: 400
        },

        plugins: {
            legend: {
                labels: {
                    color: "#94a3b8"
                }
            }
        },

        scales: {
            x: {
                ticks: {
                    color: "#64748b"
                },

                grid: {
                    color: "#1e293b"
                }
            },

            y: {
                ticks: {
                    color: "#64748b"
                },

                grid: {
                    color: "#1e293b"
                }
            }
        }
    }
});


async function getTemperature() {

    try {

        const response = await fetch("http://localhost:8080/data");

        if (!response.ok) {
            throw new Error("Server error");
        }

        const data = await response.json();

        const match = data.reading.match(/-?\d+(\.\d+)?/);

        if (!match) {
            throw new Error("Invalid temperature");
        }

        const temperature = parseFloat(match[0]);

        updateDashboard(temperature);

        setServerStatus(true);

    } catch (error) {

        console.error(error);

        setServerStatus(false);
    }
}


function updateDashboard(temperature) {

    const time = new Date().toLocaleTimeString();

    readings.push({
        temperature: temperature,
        time: time
    });

    if (readings.length > 30) {
        readings.shift();
    }

    updateStatistics();

    updateChart(time, temperature);

    updateHistory();

    currentTemp.textContent = `${temperature} °C`;

    lastUpdated.textContent = time;

    sensorStatus.textContent = "ACTIVE";

    sensorStatus.style.color = "#34d399";

    dataSource.textContent = "Real Sensor / Simulation";
}


function updateStatistics() {

    if (readings.length === 0) {
        return;
    }

    const values = readings.map(
        reading => reading.temperature
    );

    const minimum = Math.min(...values);

    const maximum = Math.max(...values);

    const total = values.reduce(
        (sum, value) => sum + value,
        0
    );

    const average = total / values.length;

    minTemp.textContent = `${minimum} °C`;

    maxTemp.textContent = `${maximum} °C`;

    avgTemp.textContent = `${average.toFixed(1)} °C`;
}


function updateChart(time, temperature) {

    chart.data.labels.push(time);

    chart.data.datasets[0].data.push(temperature);

    if (chart.data.labels.length > 15) {

        chart.data.labels.shift();

        chart.data.datasets[0].data.shift();
    }

    chart.update();
}


function updateHistory() {

    readingTable.innerHTML = "";

    const recentReadings = [...readings]
        .reverse()
        .slice(0, 10);

    recentReadings.forEach(reading => {

        const row = document.createElement("tr");

        row.innerHTML = `
            <td>${reading.time}</td>
            <td class="temperature-value">
                ${reading.temperature} °C
            </td>
            <td class="reading-status">
                Normal
            </td>
        `;

        readingTable.appendChild(row);
    });
}


function setServerStatus(online) {

    if (online) {

        serverStatus.textContent = "ONLINE";

        serverStatus.style.color = "#34d399";

    } else {

        serverStatus.textContent = "OFFLINE";

        serverStatus.style.color = "#f87171";

        sensorStatus.textContent = "UNAVAILABLE";

        sensorStatus.style.color = "#f87171";

        dataSource.textContent = "No connection";
    }
}


getTemperature();

setInterval(getTemperature, 2000);
