#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

static const char *RX_TASK_TAG = "UART_RX";                    //unique identifier for LOG
static const int RX_BUF_SIZE = 1024;                //Buffer size for Receiving data

#define TXD_PIN (GPIO_NUM_4)                        //Macros defined for GPIO transmitting UART pin
#define RXD_PIN (GPIO_NUM_5)                        //Macros defined for GPIO Receiving UART pin

void init(void)  
{
    //intialization of - UART pins for trnasmitting and receiving data
    //set uart different properties before configuration 
    uart_config_t uart_config = {
        .baud_rate = 115200,                        //set baudrate 
        .data_bits = UART_DATA_8_BITS,              //data stream length will be 8 bits
        .parity = UART_PARITY_DISABLE,              // disable parity bit
        .stop_bits = UART_STOP_BITS_1,              //set 1 bit for stop condition
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE       //No flow control
    };
    
    //configure uart parameters 
    uart_param_config(UART_NUM_1, &uart_config);

    //set uart pins
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //install UART driver
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}

/*
int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

static void tx_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) {
        sendData(TX_TASK_TAG, "Hello world");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    free(data);
}
*/

void uart_send_data(const char *LOGIName,const char *data){
  uart_write_bytes(UART_NUM_1, data, strlen(data)); 
  ESP_LOGI(LOGIName, "Data sent: %s",data);
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
        }
         
        uart_send_data(TX_TASK_TAG, "Hello from ESP32\n");

        vTaskDelay(pdMS_TO_TICKS(2000));                //wait for 2 seconds
    }
    
}

void app_main(void)
{
    init();
    //xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(uart_task, "uart_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
}
