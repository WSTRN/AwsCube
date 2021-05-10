#include "WIFI.h"
#include "spi.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "PageManager.h"
#include "semphr.h"
#include "queue.h"
#include "SEGGER_RTT.h"
#include "stm32f4xx_hal_rtc.h"

TaskHandle_t WIFITaskHandle = NULL;
SemaphoreHandle_t UARTRXcplt = NULL;

class WIFI esp8266;

extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef RTC_TimeNow;
RTC_DateTypeDef RTC_DateNow;
TimeType ntp;


void WIFITask(void const * argument)
{
    esp8266.ExtPowerEnable(1);
    vTaskDelay(5000);
    while(!esp8266.CheckConnection())
    {
        vTaskDelay(1000);
    }
    SEGGER_RTT_printf(0,"connected\r\n");
    esp8266.GetData(WIFI::Tag_GetNTP,NULL);

    esp8266.getNTP(&ntp);
    RTC_TimeNow.Hours = ntp.hour;
    RTC_TimeNow.Minutes = ntp.min;
    RTC_TimeNow.Seconds = ntp.sec;
    RTC_TimeNow.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeNow.StoreOperation = RTC_STOREOPERATION_RESET;
  
    RTC_DateNow.Month = ntp.month;
    RTC_DateNow.Date = ntp.day;
    RTC_DateNow.Year = ntp.year;
    if(!HAL_RTC_SetTime(&hrtc,&RTC_TimeNow,RTC_FORMAT_BCD))
    {
        SEGGER_RTT_printf(0,"time success\r\n");
    }
    if(!HAL_RTC_SetDate(&hrtc,&RTC_DateNow,RTC_FORMAT_BCD))
    {
        SEGGER_RTT_printf(0,"date success\r\n");
    }
    for(;;)
    {
        vTaskDelay(1);
    }
}

void WIFI_Init(void)
{
    UARTRXcplt=xSemaphoreCreateBinary();
    xTaskCreate((TaskFunction_t)WIFITask,
                (const char*)"WIFITask",
                (uint16_t)256,
                (void*)NULL,
                (UBaseType_t)3,
                (TaskHandle_t*)&WIFITaskHandle);
}