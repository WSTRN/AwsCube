#include "WIFI.h"
#include "spi.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "PageManager.h"
#include "usart.h"
#include "semphr.h"
#include "queue.h"


WIFI::WIFI():NetWork()
{
	// tag=0;
	// sum=0;
	
}

void WIFI::ExtPowerEnable(bool En)
{
    if(En)
    	HAL_GPIO_WritePin(ExtPower_GPIO_Port, ExtPower_Pin, GPIO_PIN_RESET);
    else
		HAL_GPIO_WritePin(ExtPower_GPIO_Port, ExtPower_Pin, GPIO_PIN_SET);
}

bool WIFI::CheckConnection(void)
{
	GetProcessRawData(Tag_ConnectionStatus);
	if(Isconnected)
    	return true;
	else
		return false;
}

bool WIFI::RequestData(Tag t)
{
	return GetProcessRawData(t);
}

bool WIFI::GetProcessRawData(Tag t)
{
	uint8_t tag=t;
	UBaseType_t xReturn;
	HAL_UART_Transmit(&huart2,&tag,1,100);
	HAL_UART_Receive_DMA(&huart2,tempdata,10);
	xReturn=xSemaphoreTake(UARTRXcplt,1500);
	if(xReturn!=pdTRUE)
	{
		HAL_UART_AbortReceive_IT(&huart2);
		return false;
	}
	if(tempdata[0]!=0xff)
	{
		return false;
	}
	if(tempdata[1]!=tag)
	{
		return false;
	}
	if(((tempdata[2]+tempdata[3]+tempdata[4]+tempdata[5]+tempdata[6]+tempdata[7])&0xff)!=tempdata[8])
	{
		return false;
	}
	if(tempdata[9]!=0xfe)
	{
		return false;
	}
	if((tempdata[2]==0x55)&&(tempdata[3]==0x66)&&(tempdata[4]==0x5f)&&(tempdata[5]==0x66)&&(tempdata[6]==0x55)&&(tempdata[7]==0x00))
	{
		return false;
	}
	switch(t)
	{
	case Tag_ConnectionStatus:	
		tempdata[4]==0x3f?Isconnected=true:Isconnected=false;
		break;
	case Tag_GetNTP:
	if((tempdata[2]==0x55)&&(tempdata[3]==0x66)&&(tempdata[4]==0x5f)&&(tempdata[5]==0x66)&&(tempdata[6]==0x55)&&(tempdata[7]==0x00))
	{
		return false;
	}
		NTP.sec   = tempdata[7];
		NTP.min   = tempdata[6];
		NTP.hour  = tempdata[5];
		NTP.day   = tempdata[4];
		NTP.month = tempdata[3];
		NTP.year  = tempdata[2];
		break;
	case Tag_GetWeather0:
	if((tempdata[2]==0x55)&&(tempdata[3]==0x66)&&(tempdata[4]==0x5f)&&(tempdata[5]==0x66)&&(tempdata[6]==0x55)&&(tempdata[7]==0x00))
	{
		return false;
	}
		WeatherData[0].code_day   = tempdata[2];
		WeatherData[0].code_night = tempdata[3];
		WeatherData[0].high       = tempdata[4];
		WeatherData[0].low        = tempdata[5];
		WeatherData[0].humidity = tempdata[6];
		WeatherData[0].wind_scale   = tempdata[7];
		break;
	case Tag_GetWeather1:
	if((tempdata[2]==0x55)&&(tempdata[3]==0x66)&&(tempdata[4]==0x5f)&&(tempdata[5]==0x66)&&(tempdata[6]==0x55)&&(tempdata[7]==0x00))
	{
		return false;
	}
		WeatherData[1].code_day   = tempdata[2];
		WeatherData[1].code_night = tempdata[3];
		WeatherData[1].high       = tempdata[4];
		WeatherData[1].low        = tempdata[5];
		WeatherData[1].humidity = tempdata[6];
		WeatherData[1].wind_scale   = tempdata[7];
		break;
	case Tag_GetWeather2:
	if((tempdata[2]==0x55)&&(tempdata[3]==0x66)&&(tempdata[4]==0x5f)&&(tempdata[5]==0x66)&&(tempdata[6]==0x55)&&(tempdata[7]==0x00))
	{
		return false;
	}
		WeatherData[2].code_day   = tempdata[2];
		WeatherData[2].code_night = tempdata[3];
		WeatherData[2].high       = tempdata[4];
		WeatherData[2].low        = tempdata[5];
		WeatherData[2].humidity = tempdata[6];
		WeatherData[2].wind_scale   = tempdata[7];
		break;
	default:
		break;
	}
	return true;
}
extern"C" void UART2_Receive_CB(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	uint32_t ulReturn;
	ulReturn = taskENTER_CRITICAL_FROM_ISR();

	xSemaphoreGiveFromISR(UARTRXcplt,&pxHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	
	taskEXIT_CRITICAL_FROM_ISR(ulReturn);
}
