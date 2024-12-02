const ws = new WebSocket("wss://car.mercantec.tech/ws");

ws.onopen = () => {
  console.log("Connected to WebSocket");
  document.getElementById("status").textContent = "Status: Connected";
  document.getElementById("status").style.backgroundColor = "#e8f5e9";
};

ws.onclose = () => {
  console.log("Disconnected from WebSocket");
  document.getElementById("status").textContent = "Status: Disconnected";
  document.getElementById("status").style.backgroundColor = "#ffebee";
};

ws.onerror = (error) => {
  console.error("WebSocket error:", error);
  document.getElementById("status").textContent = "Status: Connection error";
  document.getElementById("status").style.backgroundColor = "#ffebee";
};

ws.onmessage = (event) => {
  console.log("Received from server:", event.data);
  document.getElementById("status").textContent = "Status: " + event.data;
};

function send(message) {
  if (ws.readyState === WebSocket.OPEN) {
    ws.send(message);
  } else {
    console.error("WebSocket is not connected");
    document.getElementById("status").textContent = "Status: Not connected";
  }
}

function forward() {
  send("forward");
}

function backward() {
  send("backward");
}

function stop() {
  send("stop");
}

function left() {
  send("left");
}

function right() {
  send("right");
}
