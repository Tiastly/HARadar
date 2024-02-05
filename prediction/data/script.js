var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
const maxDataPoints = 3000; //5min
var msgCounts = [];
const borderColorArray = [
  "rgba(135, 206, 235, 1)",
  "rgba(255, 192, 203, 1)",
  "rgba(255, 255, 0, 1)",
];

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

function onOpen(event) {
  console.log("Connection opened");
  getReadings();
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
  console.log(event.data);
  const objs = JSON.parse(event.data);

  let actArray = [];
  let velArray = [];
  let colXArray = [];
  let colYArray = [];

  objs.forEach((obj) => {
    const { act, vel, col_x: colX, col_y: colY } = obj;
    actArray.push(act);
    velArray.push(vel);
    colXArray.push(colX);
    colYArray.push(colY);
  });

  plotAct(actArray);
  plotChart(velArray);
  plotCoordinates(colXArray, colYArray);
  msgCounts = plotActCounter(actArray, msgCounts);
}

function plotAct(actDataArray) {
  var tbody = document.getElementById("act");
  tbody.innerHTML = "";

  var fragment = document.createDocumentFragment();

  var headerRow = document.createElement("tr");
  var headerContent = actDataArray
    .map((_, index) => `<td>Target ${index + 1}</td>`)
    .join("");
  headerRow.innerHTML = headerContent;
  fragment.appendChild(headerRow);

  var dataRow = document.createElement("tr");
  var dataContent = actDataArray.map((act) => `<td>${act}</td>`).join("");
  dataRow.innerHTML = dataContent;
  fragment.appendChild(dataRow);

  tbody.appendChild(fragment);
}

function plotChart(velocityDataArray) {
  const ctx = document.getElementById("velocityChart").getContext("2d");
  let chart = Chart.getChart("velocityChart");

  if (!chart) {
    chart = new Chart(ctx, {
      type: "line",
      data: { labels: [], datasets: [] },
      options: {
        scales: { y: { beginAtZero: true } },
      },
    });
  }
  const maxLength = Math.max(...velocityDataArray.map((data) => data.length));
  const newLabels = Array.from(
    { length: maxLength },
    (_, i) => i + chart.data.labels.length + 1
  );
  chart.data.labels.push(...newLabels);

  velocityDataArray.forEach((velocityData, index) => {
    const borderColor = borderColorArray[index] || "rgba(0, 0, 0, 1)";
    if (!chart.data.datasets[index]) {
      chart.data.datasets.push({
        label: `Target ${index + 1}`,
        data: new Array(chart.data.labels.length - velocityData.length).fill(
          null
        ),
        borderColor,
        borderWidth: 2,
      });
    }
    chart.data.datasets[index].data.push(...velocityData);
  });

  chart.update();
  if (chart.data.labels.length >= maxDataPoints) {
    chart.data.labels = [];
    chart.data.datasets = [];
  }
  return chart;
}
function plotCoordinates(colXArray, colYArray) {
  let chart = Chart.getChart("coordinatesChart");
  const ctx = document.getElementById("coordinatesChart").getContext("2d");

  if (!chart) {
    chart = new Chart(ctx, {
      type: "scatter",
      data: { datasets: [] },
      options: {
        scales: {
          y: { beginAtZero: true },
        },
      },
    });
  }

  colXArray.forEach((colX, index) => {
    const dataPoints = colX.map((value, i) => ({
      x: value,
      y: colYArray[index][i],
    }));
    let dataset = chart.data.datasets[index];
    if (!dataset) {
      dataset = {
        label: `Target ${index + 1}`,
        data: dataPoints,
        borderColor: borderColorArray[index],
        showLine: false,
      };
      chart.data.datasets.push(dataset);
    } else {
      dataset.data.push(...dataPoints);
    }
  });

  chart.update();
  if (chart.data.datasets[0].data.length >= maxDataPoints) {
    chart.data.datasets = [];
  }
  return chart;
}
function plotActCounter(actDataArray, msgCounts) {
  for (const act of actDataArray) {
    const idx = actDataArray.indexOf(act);
    if (!msgCounts[idx]) {
      msgCounts[idx] = {};
    }
    msgCounts[idx][act] = (msgCounts[idx][act] || 0) + 1;
  }
  console.log(msgCounts);
  updateTable(msgCounts);
  return msgCounts;
}
function updateTable(msgCounts) {
  var tbody = document.getElementById("actMsg");
  var allKeys = [];

  msgCounts.forEach(function (target) {
    for (var key in target) {
      if (!allKeys.includes(key)) {
        allKeys.push(key);
      }
    }
  });
  tbody.innerHTML = "";
  var trHeader = document.createElement("tr");
  msgCounts.forEach(function (_, index) {
    var title = document.createElement("td");
    title.textContent = "Target" + (index + 1);
    title.setAttribute("colspan", 2);
    trHeader.appendChild(title);
  });
  tbody.appendChild(trHeader);
  allKeys.forEach(function (key) {
    var row = document.createElement("tr");
    msgCounts.forEach(function (target) {
      var keyCell = document.createElement("td");
      keyCell.textContent = key;
      var valueCell = document.createElement("td");
      valueCell.textContent = target[key] || "";
      row.appendChild(keyCell);
      row.appendChild(valueCell);
    });
    tbody.appendChild(row);
  });
}
