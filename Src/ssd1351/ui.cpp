#include "DisplayBasic.hpp"
#include "lvgl.h"




void UILoopTask(void const * argument)
{
    lvgl_port_init();
    
    
    // lv_obj_t * label;
    // lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    // //lv_obj_set_event_cb(btn1, NULL);
    // lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);
    // label = lv_label_create(btn1, NULL);
    // lv_label_set_text(label, "Button");
    AppWindow_Create();
    Pages_Init();
   
    for(;;)
    {
        //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        page.Running();
        lv_task_handler(); /* let the GUI do its work */
        
        vTaskDelay(1);
    }
}
void TransLoopTask(void const * argument)
{
    for(;;)
    {
        RAM_Address();
        Write_Command(0x5C);
        OLED_DC_Set();  //DC拉高，发送数据
        SPI_NSS_LOW(); //片选拉低，选通器件
        HAL_SPI_Transmit_DMA(&hspi2,(uint8_t*)OLED_GRAM,128*128*2);

        
        //xTaskNotifyGive(uiloopTaskHandle);

        
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        SPI_NSS_HIGH(); //片选拉高，关闭器件
        OLED_DC_Set(); //DC拉高，空闲时为高电平
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

void flush_callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint16_t(* ram)[128]=(uint16_t(*)[128])OLED_GRAM;
    uint16_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            ram[y][x]=color_p->full;//put_px(x, y, *color_p)
            color_p++;
        }
    }
    lv_disp_flush_ready(disp_drv);
}
extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance==SPI2)
    {
        BaseType_t xHighTaskWoken=pdFALSE;
        vTaskNotifyGiveFromISR(TransloopTaskHandle,&xHighTaskWoken);
        portYIELD_FROM_ISR(xHighTaskWoken);
    }
} 