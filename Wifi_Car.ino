#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi Access Point
const char* ssid = "RobotCar";
const char* password = "robot123";

// L298N motor driver pins
const int ENA = 25;
const int IN1 = 26;
const int IN2 = 27;

const int ENB = 13;
const int IN3 = 12;
const int IN4 = 14;

// Web server
WebServer server(80);

// Stop all motors
void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  server.send(200, "text/plain", "STOPPED");
}

// Move forward
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  server.send(200, "text/plain", "FORWARD");
}

// Move backward
void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  server.send(200, "text/plain", "BACKWARD");
}

// Turn left
void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  server.send(200, "text/plain", "LEFT");
}

// Turn right
void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  server.send(200, "text/plain", "RIGHT");
}

// Control webpage
void handleRoot() {
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RobotCar Controller</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background: #151515;
      color: white;
      margin: 0;
      padding: 20px;
    }

    h1 { color: #00e5ff; }

    .controls {
      display: grid;
      grid-template-columns: 100px 100px 100px;
      gap: 12px;
      justify-content: center;
      margin-top: 30px;
    }

    button {
      height: 75px;
      font-size: 18px;
      font-weight: bold;
      border: none;
      border-radius: 15px;
      background: #00bcd4;
      color: black;
      touch-action: manipulation;
    }

    button:active { background: #ffffff; }

    .stop {
      background: #ff3b30;
      color: white;
    }

    .empty { visibility: hidden; }
  </style>
</head>

<body>
  <h1>🤖 RobotCar</h1>
  <p>ESP32 Wi-Fi Controller</p>

  <div class="controls">
    <button class="empty"> </button>
    <button onclick="move('forward')">⬆️<br>FORWARD</button>
    <button class="empty"> </button>

    <button onclick="move('left')">⬅️<br>LEFT</button>
    <button class="stop" onclick="move('stop')">STOP</button>
    <button onclick="move('right')">➡️<br>RIGHT</button>

    <button class="empty"> </button>
    <button onclick="move('backward')">⬇️<br>BACKWARD</button>
    <button class="empty"> </button>
  </div>

  <p id="status">Ready to drive!</p>

  <script>
    function move(direction) {
      document.getElementById("status").innerText =
        "Command: " + direction.toUpperCase();

      fetch("/" + direction)
        .catch(() => {
          document.getElementById("status").innerText =
            "Connection error!";
        });
    }
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Keep both motor channels enabled.
  // ENA and ENB wires must connect to GPIO 25 and GPIO 13.
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // Start with motors stopped
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Start Wi-Fi access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("Robot car Wi-Fi is ready!");
  Serial.print("Connect to Wi-Fi: ");
  Serial.println(ssid);
  Serial.print("Open this address: ");
  Serial.println(WiFi.softAPIP());

  // Webpage and movement routes
  server.on("/", handleRoot);
  server.on("/forward", forward);
  server.on("/backward", backward);
  server.on("/left", left);
  server.on("/right", right);
  server.on("/stop", stopCar);

  server.begin();

  Serial.println("Control webpage started!");
}

void loop() {
  server.handleClient();
}