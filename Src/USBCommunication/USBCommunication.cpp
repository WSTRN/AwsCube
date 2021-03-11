#include "USBCommunication.h"



void usb_communication_init()
{
    xTaskCreate((TaskFunction_t)USBTask,
                (const char*)"USBTask",
                (uint16_t)256,
                (void*)NULL,
                (UBaseType_t)3,
                (TaskHandle_t*)&USBTaskHandle);
}


void USBTask(void const * argument)
{
    vTaskDelay(1);
}