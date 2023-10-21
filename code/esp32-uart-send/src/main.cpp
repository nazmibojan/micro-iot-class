#include <Arduino.h>

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    delay(1000);
}

void loop() {
    // put your main code here, to run repeatedly:
    // Serial.println("Hello World!");
    ESP_LOGI("INFO", "Info Microcontroller");
    delay(3000);
    ESP_LOGE("ERROR", "Error Microcontroller");
    delay(3000);
    ESP_LOGV("VERBOSE", "Verbose Microcontroller");
    delay(3000);
    ESP_LOGW("WARNING", "WARNING Microcontroller");
    delay(3000);
    ESP_LOGD("DEBUG", "DEBUG Microcontroller");
    delay(3000);
}

// #include "driver/uart.h"

// void setup() {
//     uart_config_t uart_config =
//         {
//             .baud_rate = 9600,
//             .data_bits = UART_DATA_8_BITS,
//             .parity = UART_PARITY_DISABLE,
//             .stop_bits = UART_STOP_BITS_1,
//             .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

//     uart_param_config(UART_NUM_0, &uart_config);
//     uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
// }

// void loop() {
//     const char* data = "Hello, World!\r\n";
//     uart_write_bytes(UART_NUM_0, data, strlen(data));
//     delay(3000);
// }