#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "Praktisi AP";
const char *password = "semangka";

String translateEncryptionType(wifi_auth_mode_t encryptionType);
void scanNetworks();
void connectToNetwork();

void setup() {
    Serial.begin(9600);

    scanNetworks();

    connectToNetwork();

    // Get MAC Address
    uint8_t mac[6];
    WiFi.macAddress(mac);
    ESP_LOGI("SETUP", "MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Get IP Address
    IPAddress ip = WiFi.localIP();
    ESP_LOGI("SETUP", "IP Address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
}

void loop() {}

String translateEncryptionType(wifi_auth_mode_t encryptionType) {
    switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
        return "Open";
    case (WIFI_AUTH_WEP):
        return "WEP";
    case (WIFI_AUTH_WPA_PSK):
        return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
        return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
        return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
        return "WPA2_ENTERPRISE";
    default:
        return "Invalid type of WiFi";
    }
}

void scanNetworks() {
    int numberOfNetworks = WiFi.scanNetworks();

    ESP_LOGI("LOOP", "Number of networks found: %d", numberOfNetworks);

    for (int i = 0; i < numberOfNetworks; i++) {
        ESP_LOGI("LOOP", "Network name: %s", WiFi.SSID(i).c_str());
        ESP_LOGI("LOOP", "Signal strength: %d", WiFi.RSSI(i));
        ESP_LOGI("LOOP", "Encryption type: %s", String(translateEncryptionType(WiFi.encryptionType(i))));
        ESP_LOGI("LOOP", "=======================");
    }
}

void connectToNetwork() {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        ESP_LOGI("WIFI", "Establishing connection to WiFi..");
    }

    ESP_LOGI("WIFI", "Connected to network");
}