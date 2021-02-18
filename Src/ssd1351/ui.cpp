#include "DisplayBasic.hpp"
#include "lvgl.h"



void Task_TimeUpdate();
void UILoopTask(void const * argument)
{
    lvgl_port_init();
    AppWindow_Create();
    Pages_Init();
   
    for(;;)
    {
        //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        page.Running();
        //Task_TimeUpdate();
        lv_task_handler(); /* let the GUI do its work */
        
        vTaskDelay(5);
    }
}
void TransLoopTask(void const * argument)
{
    for(;;)
    {
        // RAM_Address();
        // Write_Command(0x5C);
        // OLED_DC_Set();  //DC拉高，发送数据
        // SPI_NSS_LOW(); //片选拉低，选通器件
        // HAL_SPI_Transmit_DMA(&hspi2,(uint8_t*)OLED_GRAM,128*128*2);

        
        // //xTaskNotifyGive(uiloopTaskHandle);

        
        // ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        // SPI_NSS_HIGH(); //片选拉高，关闭器件
        // OLED_DC_Set(); //DC拉高，空闲时为高电平
        vTaskDelay(20);
    }
}

void lvgl_ui_init()
{
    oled_drv_init();
   
    // OLED_GRAM[25][50] = 0x07;
    // OLED_GRAM[25][51] = 0xe0;
     
    xTaskCreate((TaskFunction_t)UILoopTask,
                (const char*)"UILoopTask",
                (uint16_t)512,
                (void*)NULL,
                (UBaseType_t)2,
                (TaskHandle_t*)&uiloopTaskHandle);
    xTaskCreate((TaskFunction_t)TransLoopTask,
                (const char*)"TransloopTask",
                (uint16_t)32,
                (void*)NULL,
                (UBaseType_t)4,
                (TaskHandle_t*)&TransloopTaskHandle);
}

