const ws = new WebSocket("wss://hyperdrivelabs.onrender.com/ws");

ws.onopen = () => {
  console.log("Forbundet til WebSocket");
  document.getElementById("status").textContent = "Status: Forbundet";
  document.getElementById("status").style.backgroundColor = "#e8f5e9";
};

ws.onclose = () => {
  console.log("Afbrudt fra WebSocket");
  document.getElementById("status").textContent = "Status: Afbrudt";
  document.getElementById("status").style.backgroundColor = "#ffebee";
};

ws.onerror = (error) => {
  console.error("WebSocket fejl:", error);
  document.getElementById("status").textContent = "Status: Fejl i forbindelsen";
  document.getElementById("status").style.backgroundColor = "#ffebee";
};

ws.onmessage = (event) => {
  console.log("Modtaget fra server:", event.data);
  document.getElementById("status").textContent = "Status: " + event.data;
};

function forward() {
  if (ws.readyState === WebSocket.OPEN) {
    ws.send("forward");
  } else {
    console.error("WebSocket er ikke forbundet");
    document.getElementById("status").textContent = "Status: Ikke forbundet";
  }
}

function backward() {
  if (ws.readyState === WebSocket.OPEN) {
    ws.send("backward");
  } else {
    console.error("WebSocket er ikke forbundet");
    document.getElementById("status").textContent = "Status: Ikke forbundet";
  }
}

function stop() {
  if (ws.readyState === WebSocket.OPEN) {
    ws.send("stop");
  } else {
    console.error("WebSocket er ikke forbundet");
    document.getElementById("status").textContent = "Status: Ikke forbundet";
  }
}
