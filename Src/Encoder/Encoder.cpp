#include "Encoder.h"
#include "DisplayBasic.hpp"

class ButtonEvent EncoderBtn;
TaskHandle_t EncoderTaskHandle = NULL;

/**
  * @brief  按键事件回调处理
  * @param  btn:按键对象地址
  * @param  event:事件类型
  * @retval 无
  */
static void Button_EventHandler(ButtonEvent* btn, int event)
{
    /*自动关机时间更新*/
    //Power_HandleTimeUpdate();
    
    /*传递至页面事件*/
    page.PageEventTransmit(btn, event);
    SEGGER_RTT_printf(0,"%d\r\n",event);
}
/**
  * @brief  Encoder Event Handler
  * @param  btn:按键对象地址
  * @param  event:事件类型
  * @retval 无
  */
static void Encoder_EventHandler(int16_t Cnt)
{
    /*自动关机时间更新*/
    //Power_HandleTimeUpdate();
    
    /*传递至页面事件*/
    page.PageEventTransmit(NULL, Cnt);
    SEGGER_RTT_printf(0,"%d\r\n",Cnt);
}
/**
  * @brief  按键监控更新
  * @param  无
  * @retval 无
  */
void EncoderBtn_Update()
{
    static uint8_t loop=0;
    EncoderBtn.EventMonitor((HAL_GPIO_ReadPin(EncoderKey_GPIO_Port,EncoderKey_Pin)==GPIO_PIN_RESET)?(uint8_t)1:(uint8_t)0);
    if(loop>=9)
    {
        loop=0;
        int16_t cnt=(int16_t)__HAL_TIM_GET_COUNTER(&htim3);__HAL_TIM_SET_COUNTER(&htim3,0);
        if(cnt!=0)
        {
            Encoder_EventHandler(cnt);
        }
    }
    else
        loop++;
}

void EncoderTask(void const * argument)
{
    for(;;)
    {
        EncoderBtn_Update();
        vTaskDelay(10);
    }
}

void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
    EncoderBtn.EventAttach(Button_EventHandler);
    xTaskCreate((TaskFunction_t)EncoderTask,
                (const char*)"EncoderTask",
                (uint16_t)128,
                (void*)NULL,
                (UBaseType_t)3,
                (TaskHandle_t*)&EncoderTaskHandle);
}