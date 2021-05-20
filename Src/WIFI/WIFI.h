#pragma once

#include <stdbool.h>
#include "spi.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "NetWork.h"


extern TaskHandle_t WIFITaskHandle;
extern SemaphoreHandle_t UARTRXcplt;
extern QueueHandle_t WIFI_Queue;
extern class WIFI esp8266;

void WIFI_Init(void);


class WIFI:public NetWork
{
public:
    WIFI();
    
    enum Tag
    {
        Tag_ConnectionStatus,
        Tag_GetNTP,
        Tag_GetWeather0,
        Tag_GetWeather1,
        Tag_GetWeather2,
        Tag_PowerOn=0xfd,
        Tag_PowerOff=0xfe,
        Tag_NONE=0xff,
    };

    bool Isconnected;

    void ExtPowerEnable(bool En);
    bool CheckConnection(void);
    bool RequestData(Tag t);

private:

    uint8_t tempdata[12];
    // uint8_t sum;
    // uint8_t tag;

    bool GetProcessRawData(Tag t);
};