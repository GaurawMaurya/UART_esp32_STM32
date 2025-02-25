#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

static const char *RX_TASK_TAG = "UART_RX";                     //unique identifier for LOG
static const char *UART_TASK_TAG = "UART2";
static const int RX_BUF_SIZE = 1024;                            //Buffer size for Receiving data

#define TXD_PIN (GPIO_NUM_4)                                    //Macros defined for GPIO transmitting UART pin
#define RXD_PIN (GPIO_NUM_5)                                    //Macros defined for GPIO Receiving UART pin

void init(void)  
{
    ESP_LOGI(UART_TASK_TAG, "Configuring UART");
    
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_LOGI(UART_TASK_TAG, "UART parameters configured");

    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_LOGI(UART_TASK_TAG, "UART pins configured");

    QueueHandle_t uart_queue;
    esp_err_t err = uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, RX_BUF_SIZE * 2, 20, &uart_queue, 0);
    if (err != ESP_OK) {
        ESP_LOGE(UART_TASK_TAG, "UART driver install failed: %s", esp_err_to_name(err));
        return;
    }

    uart_flush(UART_NUM_1);                                                                     // Clear UART buffer
    ESP_LOGI(UART_TASK_TAG, "UART driver installed");
}

void uart_send_data(const char *LOGIName, const char *data){
  int len = uart_write_bytes(UART_NUM_1, data, 17); 
 if(len >= 0){
    ESP_LOGI(LOGIName, "Data sent: %s\n",data);
 }
}

static void uart_task(void *arg){
    uint8_t RxData[RX_BUF_SIZE];
    
    while (1)
    {
        static const char *TX_TASK_TAG = "UART_TX";
        int len = uart_read_bytes(UART_NUM_1, RxData, RX_BUF_SIZE - 1, 100/portTICK_PERIOD_MS);

        if(len > 0){
            RxData[len] = '\0';
            ESP_LOGI(RX_TASK_TAG, "Data Received: %s\n", RxData);

            vTaskDelay(pdMS_TO_TICKS(200));                //wait for 200 milli seconds
            //if data is received then send data
            if(strcmp((char *)RxData, "Hello from stm32") == 0)
                uart_send_data(TX_TASK_TAG, "Hello from ESP32");
        }else{

            //ESP_LOGI(RX_TASK_TAG, "Data not recevide");
        }

        vTaskDelay(pdMS_TO_TICKS(200));                //wait for 200 milli seconds
    }
    //free(RxData);
}

void app_main(void)
{
    ESP_LOGI(UART_TASK_TAG, "Starting UART");
    init();
    //ESP_ERROR_CHECK(esp_heap_caps_check_integrity_all(true));  // Heap check

    //ESP_LOGI(UART_TASK_TAG, "Started UART");
    xTaskCreate(uart_task, "uart_task", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
}
