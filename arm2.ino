#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Replace with your Wi-Fi credentials
const char* ssid = "HUAWEI nova 5T";
const char* password = "12345678";

WebServer server(80);

Servo servo1;
Servo servo2;

int angle1 = 90;
int angle2 = 90;

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 Dual Servo Control</title>
      <style>
        body { font-family: Arial; text-align: center; padding-top: 40px; }
        input[type=range] { width: 80%; margin: 20px 0; }
      </style>
    </head>
    <body>
      <h2>ESP32 Dual Servo Control</h2>
      <p>Servo 1 Angle: <span id="angle1Val">90</span>°</p>
      <input type="range" min="0" max="180" value="90" id="angle1Slider" oninput="updateServo(1, this.value)">

      <p>Servo 2 Angle: <span id="angle2Val">90</span>°</p>
      <input type="range" min="0" max="180" value="90" id="angle2Slider" oninput="updateServo(2, this.value)">

      <script>
        function updateServo(num, val) {
          if (num === 1) document.getElementById('angle1Val').innerText = val;
          if (num === 2) document.getElementById('angle2Val').innerText = val;
          fetch(`/setAngle?servo=${num}&value=${val}`);
        }
      </script>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleSetAngle() {
  if (server.hasArg("servo") && server.hasArg("value")) {
    int servoNum = server.arg("servo").toInt();
    int value = constrain(server.arg("value").toInt(), 0, 180);

    if (servoNum == 1) {
      angle1 = value;
      servo1.write(angle1);
      Serial.println("Servo 1 angle: " + String(angle1));
    } else if (servoNum == 2) {
      angle2 = value;
      servo2.write(angle2);
      Serial.println("Servo 2 angle: " + String(angle2));
    }

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);

  // Attach servos to appropriate GPIOs
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo1.attach(13, 500, 2400);  // Servo 1 on GPIO 13
  servo2.attach(14, 500, 2400);  // Servo 2 on GPIO 14

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  // Configure web server
  server.on("/", handleRoot);
  server.on("/setAngle", handleSetAngle);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
