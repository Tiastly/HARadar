var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
const maxDataPoints = 3000; // 5 minutes at 10Hz
var msgCounts = [];
var timestampML;
var timestampMA;
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
  console.log("Trying to open a WebSocket connection");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
  resetAll();
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
  // console.log(event.data);
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

  maActivity(velArray)
  processActivity(actArray);
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

function resetAll() {
  stillTime = 0;
  activityQueue = [];
  continuousMovementPoints = 0;
  msgCounts = [];
  timestampML = new Date().getTime();
  timestampMA = new Date().getTime();
}
const TIME_THRESHOLD = 1; // minutes
const TIME_WINDOW_SIZE = Math.floor((TIME_THRESHOLD * 60) / 2.5);

// ========= moving average filter ==========
const threshold = 5;
let stillTime = 0;
function movingAverage(data, windowSize) {
  let result = [];
  for (let i = 0; i <= data.length - windowSize; i++) {
    let sum = 0;
    for (let j = 0; j < windowSize; j++) {
      sum += Math.abs(data[i + j]);
    }
    result.push(sum / windowSize);
  }
  return result;
}
function checkStillness(data, threshold) {
  let avgSpeed =
    data.reduce((acc, val) => acc + Math.abs(val), 0) / data.length;
    console.log("Average Speed: "+avgSpeed);
  return avgSpeed < threshold;
}

function maActivity(velocity) {
  const filteredVelocity = movingAverage(velocity[0], 5);
  const isStill = checkStillness(filteredVelocity, threshold);
  stillTime = isStill ? stillTime + 1 : 0;
  console.log("Moving Average: "+stillTime);
  if (stillTime >= TIME_WINDOW_SIZE) {
    triggerAlertMA();
    stillTime = 0;
  }
}
// ============= ml =================
let activityQueue = [];
let continuousMovementPoints = 0;
const MOVEMENT_THRESHOLD_POINTS = Math.floor(TIME_WINDOW_SIZE * 0.05); // 95% of the time window size
function processActivity(actArray) {
  let activity = actArray[0];
  if (activityQueue.length >= TIME_WINDOW_SIZE) {
    let removedActivity = activityQueue.shift();
    if (removedActivity === 1 && continuousMovementPoints > 0) {
      continuousMovementPoints--;
    }
  }

  activityQueue.push(activity);
  continuousMovementPoints =
    activity === "Sit" ? 0 : continuousMovementPoints + 1;

  console.log("counting Sit: "+ activityQueue.length);
  console.log("continuousMovementPoints: "+continuousMovementPoints);
  if (continuousMovementPoints >= MOVEMENT_THRESHOLD_POINTS) {
    activityQueue = [];
    continuousMovementPoints = 0;
  } else if (
    activityQueue.length === TIME_WINDOW_SIZE &&
    continuousMovementPoints <= MOVEMENT_THRESHOLD_POINTS
  ) {
    triggerAlertML();
    activityQueue = [];
    continuousMovementPoints = 0;
  }
}

function triggerAlertML() {
  alert("Please stand up and move around from ML-Counting!");
  const timestamp = new Date().getTime();
  console.log("MLAltert: "+ getTimestamp(timestamp - timestampML));
  timestampML = timestamp;
}
function triggerAlertMA() {
  alert("Please stand up and move around from Moving Average!");
  const timestamp = new Date().getTime();
  console.log("MAAltert: "+ getTimestamp(timestamp - timestampMA));
  timestampMA = timestamp;
}


function getTimestamp(timeDifference) {
  const hours = Math.floor(timeDifference / 3600000);
  const minutes = Math.floor((timeDifference % 3600000) / 60000);
  const seconds = Math.floor((timeDifference % 60000) / 1000);
  const milliseconds = Math.floor(timeDifference % 1000);

  // Format each component to have leading zeros if necessary
  const formattedHours = ('0' + hours).slice(-2);
  const formattedMinutes = ('0' + minutes).slice(-2);
  const formattedSeconds = ('0' + seconds).slice(-2);
  const formattedMilliseconds = ('00' + milliseconds).slice(-3);

  const formattedTimestamp = `${formattedHours}:${formattedMinutes}:${formattedSeconds}.${formattedMilliseconds}`;

  return formattedTimestamp;
}



