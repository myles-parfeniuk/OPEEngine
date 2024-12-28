#pragma once
// std lib includes
#include <cstdio>
#include <cstring>
#include <stdio.h>
// esp-idf includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
// OPEEngine includes
#include "OPEEDevice.h"

class SerialService
{
    public:
        SerialService(Device& d);

    private:
        Device& d;

        TaskHandle_t uart_task_hdl;
        QueueHandle_t queue_uart;

        static void uart_task_trampoline(void* arg);
        void uart_task();

        uint8_t tx_buffer[256];

        static const constexpr uint16_t UART_TASK_PRIO = 3;
        static const constexpr char* TAG = "SerialService";
};