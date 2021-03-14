#include "DisplayBasic.hpp"
#include "lvgl.h"
#include "SEGGER_RTT.h"



void UILoopTask(void const * argument)
{
    lvgl_port_init();
    AppWindow_Create();
    Pages_Init();
    TickType_t xLastWakeTime; 
    for(;;)
    {
        page.Running();
        lv_task_handler(); /* let the GUI do its work */
        vTaskDelayUntil(&xLastWakeTime,20);
    }
}


void lvgl_UI_Init()
{
    oled_drv_init();
    xTaskCreate((TaskFunction_t)UILoopTask,
                (const char*)"UILoopTask",
                (uint16_t)512,
                (void*)NULL,
                (UBaseType_t)2,
                (TaskHandle_t*)&uiloopTaskHandle);
    

}

