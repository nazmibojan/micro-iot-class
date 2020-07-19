#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

#define DHT_PIN 23
#define DHT_TYPE DHT11

float temperature = 0, humidity = 0;
String tempString, humidString;
long lastPublish = 0, lastConnected = 0;
const char *ssid = "ipop";
const char *password = "iyanfopi";
String header;
String ledState = "off";

DHT dht(DHT_PIN, DHT_TYPE);
WiFiServer server(80);

void updateDhtData();
void connectToNetwork();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  dht.begin();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  // Start server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    while(client.available()) {
    String request = client.readStringUntil('\n');
    header += request;
    Serial.print(request);
    Serial.println(request.length());

    // lastConnected = millis();
    // String currentLine = "";

    // while (client.connected() && millis() - lastConnected <= 2000) {
    //   // Read client data
    //   if (client.available()) {
    //     char c = client.read();
    //     Serial.print(c);
    //     header += c;

    //     if (c == '\n') {
    //       if (currentLine.length() == 0) {
      if (request.length() - 1 == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("LED on");
              ledState = "on";
              digitalWrite(BUILTIN_LED, HIGH);
            } else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("LED off");
              ledState = "off";
              digitalWrite(BUILTIN_LED, LOW);
            }
            header = "";

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta http-equiv=\"refresh\" content=\"30\", "
                           "name=\"viewport\" "
                           "content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>body { text-align: center; font-family: "
                           "\"Arial\", Arial;}");
            client.println(
                "table { border-collapse: collapse; width:40%; "
                "margin-left:auto; margin-right:auto; border-spacing: "
                "2px; background-color: white; border: 4px solid green; }");
            client.println("th { padding: 20px; background-color: #008000; "
                           "color: white; }");
            client.println("tr { border: 5px solid green; padding: 2px; }");
            client.println("tr:hover { background-color:yellow; }");
            client.println("td { border:4px; padding: 12px; }");
            client.println(
                ".sensor { color:red; font-weight: bold; padding: 1px;}");
            client.println(
                ".button { background-color: #4CAF50; border: none; color: "
                "white; padding: 16px 40px; }</style>");
            client.println(
                "</head><body><h1>ESP32 Web Server Reading sensor values</h1>");
            client.println("<h2>DHT11</h2>");
            client.println(
                "<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>");
            client.println(
                "<tr><td>Temp. Celsius</td><td><span class=\"sensor\">");
            client.println(temperature);
            client.println(" *C</span></td></tr>");
            client.println(
                "<tr><td>Temp. Fahrenheit</td><td><span class=\"sensor\">");
            client.println(1.8 * temperature + 32);
            client.println(" *F</span></td></tr>");
            client.println("<tr><td>Humidity</td><td><span class=\"sensor\">");
            client.println(humidity);
            client.println(" %</span></td></tr></table>");
            if (ledState == "on") {
              client.println("<p><a href=\"/led/off\"><button "
                             "class=\"button\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/led/on\"><button "
                             "class=\"button\">ON</button></a></p>");
            }
            client.println("</body></html>");
            client.println();
            client.stop();
    //       } else {
    //         currentLine = "";
    //       }
    //     } else if (c != '\r') {
    //       currentLine += c;
    //     }
      }
    }
  }

  if (millis() - lastPublish > 5000) {
    updateDhtData();
    lastPublish = millis();
  }
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");
}

void updateDhtData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  tempString = String(temperature);
  humidString = String(humidity);

  Serial.println("Temperature: " + tempString + " C & Humidity: " +
                 humidString + " %");
}