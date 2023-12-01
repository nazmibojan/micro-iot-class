#include <Arduino.h>
#include <WiFi.h>

#define ALL_EVENT_BITS 0xFF
#define WIFI_CONNECTION_BIT (0x01UL)
#define QUEUE_SIZE 15

const char *ssid = "CONNEXT-AXIATA";
const char *pass = "4xiatadigitallabs18";
int lastRequest = 0;

TaskHandle_t uartTaskHandle;
TaskHandle_t ledTaskHandle;
EventGroupHandle_t sampleGroup;
QueueHandle_t messageQueue;

void setupWifi();
void uartTask(void *parameter);
void ledTask(void *parameter);

void setup() {
  // put your setup code here, to run once:
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  vTaskDelay(1000);

  // Create event group
  sampleGroup = xEventGroupCreate();
  configASSERT(sampleGroup);

  // Create message queue
  messageQueue = xQueueCreate(QUEUE_SIZE, sizeof(char));
  if (messageQueue == NULL) {
    ESP_LOGI("SETUP", "Error creating the queue");
  }

  // Create RTOS task
  xTaskCreatePinnedToCore(uartTask, "UART Task", 2048, NULL, 1, &uartTaskHandle,
                          0);
  xTaskCreatePinnedToCore(ledTask, "LED Task", 2048, NULL, 2, &ledTaskHandle,
                          0);

  setupWifi();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - lastRequest > 10000) {
    if (WiFi.status() != WL_CONNECTED) {
      setupWifi();
    } else {
      ESP_LOGI("WIFI", "WiFi is already connected...");
    }

    lastRequest = millis();
  }
}

void uartTask(void *parameter) {
  EventBits_t clientBits;

  for (;;) {
    clientBits = xEventGroupWaitBits(sampleGroup, ALL_EVENT_BITS, pdTRUE,
                                     pdFALSE, 1000 / portTICK_PERIOD_MS);
    clientBits &= WIFI_CONNECTION_BIT;

    if (clientBits != 0) {
      char deviceIP[QUEUE_SIZE] = {0};
      // Read message queue
      for (int readIndex = 0; readIndex < QUEUE_SIZE; readIndex++) {
        xQueueReceive(messageQueue, deviceIP, portMAX_DELAY);
      }

      ESP_LOGI("WIFI", "WiFi connected");
    }
  }
}

void ledTask(void *parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(BUILTIN_LED, HIGH);
      vTaskDelay(1000);
      digitalWrite(BUILTIN_LED, LOW);
      vTaskDelay(1000);
    } else {
      digitalWrite(BUILTIN_LED, LOW);
    }
  }
}

void setupWifi() {
  vTaskDelay(10);
  // We start by connecting to a WiFi network
  ESP_LOGI("WIFI", "Connecting to %s", ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500);
    ESP_LOGI("WIFI", ".");
  }

  String Esp32Ip = String(WiFi.localIP());

  for (int insertIndex = 0; insertIndex < QUEUE_SIZE; insertIndex++) {
    xQueueSend(messageQueue, &Esp32Ip.c_str()[insertIndex], portMAX_DELAY);
  }
  xEventGroupSetBits(sampleGroup, WIFI_CONNECTION_BIT);
}