#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Create an instance of the web server
WebServer server(80);

// Define the GPIO pins
const int ledPin = 2;
const int buttonPin = 4;

void setup() {
  // Initialize the serial port
  Serial.begin(115200);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print the ESP32 IP address
  Serial.println(WiFi.localIP());

  // Initialize the GPIO pins
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Serve the HTML page to clients connecting to the server
  server.on("/", [](){
    server.sendHeader("Content-Encoding", "gzip");
    server.send_P(200, "text/html", index_html, index_html_gz_len);
  });

  // Handle the button click event
  server.on("/button", [](){
    digitalWrite(ledPin, !digitalRead(ledPin));
    server.send(200, "text/plain", "Button clicked");
  });

  // Start the server
  server.begin();
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}

// Define the HTML page as a compressed string
static const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Web Server</title>
    <style>
      body {
        font-family: Arial, Helvetica, sans-serif;
      }
      button {
        background-color: #4CAF50;
        border: none;
        color: white;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        cursor: pointer;
      }
      button:active {
        background-color: #3e8e41;
      }
    </style>
  </head>
  <body>
    <h1>ESP32 Web Server</h1>
    <p>LED status: <span id="ledStatus">OFF</span></p>
    <button id="button" onclick="toggleLED()">Toggle LED</button>
    <script>
      // Define the function to toggle the LED
      function toggleLED() {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/button");
        xhr.onreadystatechange = function() {
          if (xhr.readyState === 4 && xhr.status === 200) {
            var ledStatus = document.getElementById("ledStatus");
            if (ledStatus.innerHTML === "OFF") {
              ledStatus.innerHTML = "ON";
            } else {
              ledStatus.innerHTML = "OFF";
            }
          }
        }
        xhr.send();
      }
    </script>
  </body>
</html>
)=====";
