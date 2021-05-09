#pragma once

#include <stdbool.h>
#include "spi.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "NetWork.h"

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
        Tag_NONE=0xff,
    };

    bool Isconnected;

    void ExtPowerEnable(bool En);
    bool CheckConnection(void);
    bool GetData(Tag t,uint8_t *dp);

private:

    uint8_t tempdata[10];
    // uint8_t sum;
    // uint8_t tag;

    volatile bool GetProcessRawData(Tag t);
};