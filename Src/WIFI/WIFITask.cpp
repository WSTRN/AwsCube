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
QueueHandle_t WIFI_Queue = NULL;

class WIFI esp8266;

extern RTC_HandleTypeDef hrtc;


void WIFITask(void const * argument)
{
    // esp8266.ExtPowerEnable(1);
    // vTaskDelay(5000);
    // while(!esp8266.CheckConnection())
    // {
    //     vTaskDelay(1000);
    // }
    // SEGGER_RTT_printf(0,"connected\r\n");
    // esp8266.GetData(WIFI::Tag_GetNTP,NULL);

    
    uint8_t WifiMsg=0xff;
    for(;;)
    {
        xQueueReceive(WIFI_Queue,&WifiMsg,portMAX_DELAY);
        SEGGER_RTT_printf(0,"recvive %d\r\n",WifiMsg);
        if(WifiMsg==WIFI::Tag_PowerOn)
        {
            esp8266.ExtPowerEnable(1);
            vTaskDelay(5000);
        }
        else if(WifiMsg==WIFI::Tag_PowerOff)
        {
            esp8266.ExtPowerEnable(0);
        }
        else if(WifiMsg==WIFI::Tag_NONE)
        {
            //asm("NOP");
        }
        else if(WifiMsg==WIFI::Tag_ConnectionStatus)
        {
            while(!esp8266.CheckConnection())
            {
                vTaskDelay(1000);
            }
        }
        else if(WifiMsg==WIFI::Tag_GetNTP)
        {
            while(!esp8266.RequestData((WIFI::Tag)WifiMsg));
            RTC_TimeTypeDef RTC_TimeNow;
            RTC_DateTypeDef RTC_DateNow;
            TimeType ntp;
            esp8266.getNTP(&ntp);
            RTC_TimeNow.Hours = ntp.hour;
            RTC_TimeNow.Minutes = ntp.min;
            RTC_TimeNow.Seconds = ntp.sec;
            RTC_TimeNow.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
            RTC_TimeNow.StoreOperation = RTC_STOREOPERATION_RESET;
        
            RTC_DateNow.Month = ntp.month;
            RTC_DateNow.Date = ntp.day;
            RTC_DateNow.Year = ntp.year;
            RTC_DateNow.WeekDay=((5-40+RTC_Bcd2ToByte(ntp.year)+(RTC_Bcd2ToByte(ntp.year)/4)+((13*(RTC_Bcd2ToByte(ntp.month)+1))/5)+RTC_Bcd2ToByte(ntp.day)-1)%7);
            if(RTC_DateNow.WeekDay==0) RTC_DateNow.WeekDay=7;
            HAL_RTC_SetTime(&hrtc,&RTC_TimeNow,RTC_FORMAT_BCD);
            HAL_RTC_SetDate(&hrtc,&RTC_DateNow,RTC_FORMAT_BCD);
        }
        else
        {
            esp8266.RequestData((WIFI::Tag)WifiMsg);
        }
    }
}

void WIFI_Init(void)
{
    UARTRXcplt=xSemaphoreCreateBinary();
    WIFI_Queue=xQueueCreate(8,1);
    xTaskCreate((TaskFunction_t)WIFITask,
                (const char*)"WIFITask",
                (uint16_t)256,
                (void*)NULL,
                (UBaseType_t)3,
                (TaskHandle_t*)&WIFITaskHandle);
}