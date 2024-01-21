var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
// Init web socket when the page loads
window.addEventListener("load", onload);

function onload(event) {
  initWebSocket();
}

function getReadings() {
  websocket.send("getReadings");
}

function initWebSocket() {
  console.log("Trying to open a WebSocket connection…");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

// When websocket is established, call the getReadings() function
function onOpen(event) {
  console.log("Connection opened");
  getReadings();
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
  console.log(event.data);
  var obj = JSON.parse(event.data);
  var keys = Object.keys(obj);

  // for (var i = 0; i < keys.length; i++) {
  //   var key = keys[i];
  // }

  document.getElementById("act").innerHTML = obj["act"];
  plotChart(obj["vel"]);
  plotCoordinates(obj["col_x"], obj["col_y"]);
}

function plotChart(velocityData) {
  var ctx = document.getElementById("velocityChart").getContext("2d");

  var chart = Chart.getChart("velocityChart");
  if (!chart) {
    chart = new Chart(ctx, {
      type: "line",
      data: {
        labels: [],
        datasets: [
          {
            label: "Velocity",
            data: [],
            borderColor: "rgb(75, 192, 192)",
            borderWidth: 2,
            fill: false,
          },
        ],
      },
      options: {
        scales: {
          y: {
            beginAtZero: true,
          },
        },
      },
    });
  }
  // Update chart data with new data
  let labelValue = chart.data.labels.length + 1;
  velocityData.forEach(function (value, index) {
    chart.data.labels.push(index + labelValue);
    chart.data.datasets[0].data.push(value);
  });

  // Update the chart to reflect the new data
  chart.update();

  return chart;
}
function plotCoordinates(xData, yData) {
  var ctx = document.getElementById("coordinatesChart").getContext("2d");
  var chart = Chart.getChart("coordinatesChart");
  var coordinatesData = [];
  for (var i = 0; i < xData.length; i++) {
    var point = {
      x: xData[i],
      y: yData[i],
    };
    coordinatesData.push(point);
  }
  if (!chart) {
    chart = new Chart(ctx, {
      type: "scatter",
      data: {
        datasets: [
          {
            label: "Coordinates",
            data: coordinatesData,
            borderColor: "rgb(75, 192, 192)",
            borderWidth: 2,
            fill: false,
          },
        ],
      },
      options: {
        scales: {
          y: {
            beginAtZero: true,
          },
        },
      },
    });
  } else {
    // 添加新的点到数据集中
    for (var i = 0; i < coordinatesData.length; i++) {
      chart.data.datasets[0].data.push(coordinatesData[i]);
    }
    chart.update();
  }

  return chart;
}

