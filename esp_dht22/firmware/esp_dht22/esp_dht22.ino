#define DHT_PIN                14
#define DHT_TYPE               DHT22

#include <dht.h>   // installed via DHTSTable library
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

dht sensor;
ESP8266WebServer server(80);

// wifi_credentials.h should define WIFI_SSID and WIFI_PASSWORD
#include "wifi_credentials.h"

bool WiFiConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFI connection, trying to connect");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      yield();
      Serial.print(".");
      if (count++ >= 60) {
        Serial.println("\nunable to connect after 30 seconds. retrying");
        return false;
      }
    }
    Serial.print("\n");
    Serial.print("WiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
  }
  return true;
}

void handleRequest() {
  String payload("DHT22 ");
  switch (sensor.read22(DHT_PIN)) {
    case DHTLIB_OK:
      payload.concat("temperature: ");
      payload.concat(static_cast<int>(32.0 + 9.0 / 5.0 * sensor.temperature));
      payload.concat("F humidity ");
      payload.concat(sensor.humidity);
      payload.concat("%");
    break;
    case DHTLIB_ERROR_CHECKSUM:
      payload.concat("checksum mis-match");
      break;

    case DHTLIB_ERROR_TIMEOUT:
      payload.concat("timeout");
      break;

    default:
      payload.concat("unknown error");
      break;
  }
  Serial.println(payload);

  String response("<html><body>");
  response.concat(payload);
  response.concat("</body></html>");
  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(115200);
  Serial.println("READY");
  server.on("/", handleRequest);
  server.begin();
}

void loop() {
  while (true) {
    if (WiFi.status() != WL_CONNECTED) {
      WiFiConnect();
    }
    yield();
    server.handleClient();
  }
}
