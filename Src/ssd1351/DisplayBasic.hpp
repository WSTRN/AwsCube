#pragma once

#include "lvgl.h"
#include "spi.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "PageManager.h"

typedef enum
{
    /*保留*/
    PAGE_NONE,
    /*用户页面*/
    PAGE_MainClock,
    //PAGE_MainMenu,
    //PAGE_TimeCfg,
    //PAGE_Backlight,
    //PAGE_StopWatch,
    PAGE_Weather,
    //PAGE_About,
    //PAGE_Game,
    /*保留*/
    PAGE_MAX
} Page_Type;

//static uint8_t OLED_GRAM[128][256];
static TaskHandle_t uiloopTaskHandle = NULL;

extern PageManager page;

void lvgl_UI_Init();
void lvgl_port_init();
/*AppWindow*/
void AppWindow_Create();
void Pages_Init();
lv_obj_t * AppWindow_GetCont(uint8_t pageID);
lv_coord_t AppWindow_GetHeight();
lv_coord_t AppWindow_GetWidth();
#define APP_WIN_HEIGHT AppWindow_GetHeight()
#define APP_WIN_WIDTH  AppWindow_GetWidth()



extern PageManager page;
// void PageDelay(uint32_t ms);
#define PageWaitUntil(condition)\
while(!(condition)){\
    lv_task_handler();\
}



#define LV_ANIM_TIME_DEFAULT 200
#define LV_SYMBOL_DEGREE_SIGN   "\xC2\xB0"


bool lv_obj_del_safe(lv_obj_t** obj);
void lv_label_set_text_add(lv_obj_t * label, const char * text);
lv_coord_t lv_obj_get_x_center(lv_obj_t * obj);
lv_coord_t lv_obj_get_y_center(lv_obj_t * obj);
void lv_obj_set_color(lv_obj_t * obj, lv_color_t color);
void lv_table_set_align(lv_obj_t * table, lv_label_align_t align);
lv_obj_t * lv_win_get_label(lv_obj_t * win);
void lv_obj_add_anim(
    lv_obj_t * obj, lv_anim_t * a,
    lv_anim_exec_xcb_t exec_cb, 
    int32_t start, int32_t end,
    uint16_t time = LV_ANIM_TIME_DEFAULT,
    lv_anim_ready_cb_t ready_cb = NULL,
    lv_anim_path_cb_t path_cb = lv_anim_path_ease_out
);
#define LV_OBJ_ADD_ANIM(obj,attr,target,time)\
do{\
    static lv_anim_t a;\
    lv_obj_add_anim(\
        (obj), &a,\
        (lv_anim_exec_xcb_t)lv_obj_set_##attr,\
        lv_obj_get_##attr(obj),\
        (target),\
        (time)\
    );\
}while(0)



#define OLED_RES_GPIO_PORT      (GPIOB)				//RES#引脚
#define OLED_RES_GPIO_PINS      (RES_Pin)

#define OLED_DC_GPIO_PORT       (GPIOB)				//D/C#引脚
#define OLED_DC_GPIO_PINS       (DC_Pin)

#define OLED_CS_GPIO_PORT      	(GPIOA)				//CS#引脚
#define OLED_CS_GPIO_PINS      	(CS_Pin)

#define OLED_RES_Set()          HAL_GPIO_WritePin(OLED_RES_GPIO_PORT, OLED_RES_GPIO_PINS, GPIO_PIN_SET)
#define OLED_RES_Clr()          HAL_GPIO_WritePin(OLED_RES_GPIO_PORT, OLED_RES_GPIO_PINS, GPIO_PIN_RESET)

#define OLED_DC_Set()           HAL_GPIO_WritePin(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PINS, GPIO_PIN_SET)
#define OLED_DC_Clr()           HAL_GPIO_WritePin(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PINS, GPIO_PIN_RESET)

#define	SPI_NSS_HIGH()					HAL_GPIO_WritePin(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PINS, GPIO_PIN_SET)
#define	SPI_NSS_LOW()						HAL_GPIO_WritePin(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PINS, GPIO_PIN_RESET)

static void Write_Command(uint8_t cmd)  {
  OLED_DC_Clr();  //DC拉低，发送命令
  SPI_NSS_LOW(); //片选拉低，选通器件
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 100);
//SPI_RW_Byte(Cmd); //发送数据
  SPI_NSS_HIGH(); //片选拉高，关闭器件
  OLED_DC_Set(); //DC拉高，空闲时为高电平
}
static void Write_Data(uint8_t dat) {
 
	OLED_DC_Set();  //DC拉高，发送数据
  SPI_NSS_LOW(); //片选拉低，选通器件
  HAL_SPI_Transmit(&hspi2, &dat, 1, 100); //发送数据
  SPI_NSS_HIGH(); //片选拉高，关闭器件
  OLED_DC_Set(); //DC拉高，空闲时为高电平
  
}
static void RAM_Address(uint8_t start_column,uint8_t end_column,uint8_t start_row,uint8_t end_row)  
{
  static uint8_t cmd;
  cmd=0x75;
  OLED_DC_Clr();  //DC拉低，发送命令
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 10);
  OLED_DC_Set();  //DC拉高，发送数据
  HAL_SPI_Transmit(&hspi2, &start_column, 1, 10);
  HAL_SPI_Transmit(&hspi2, &end_column, 1, 10);
  cmd=0x15;
  OLED_DC_Clr();  //DC拉低，发送命令
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 10);
  OLED_DC_Set();  //DC拉高，发送数据
  HAL_SPI_Transmit(&hspi2, &start_row, 1, 10);
  HAL_SPI_Transmit(&hspi2, &end_row, 1, 10);

}
static void DMA_Send_Pixel(uint16_t size,lv_color_t * color_p)
{
  static uint8_t cmd;
  cmd=0x5c;
  OLED_DC_Clr();  //DC拉低，发送命令
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 10);
  OLED_DC_Set();  //DC拉高，发送数据
  HAL_SPI_Transmit_DMA(&hspi2,(uint8_t*)color_p,size);
}

static void oled_drv_init()
{
  SPI_NSS_LOW();
	OLED_DC_Clr();

  OLED_RES_Clr();
  vTaskDelay(40);
	OLED_RES_Set();
  vTaskDelay(20);
    
  Write_Command(0xfd);	// command lock
  Write_Data(0x12);
  Write_Command(0xfd);	// command lock
  Write_Data(0xB1);

  Write_Command(0xae);	// display off
  Write_Command(0xa4); 	// Normal Display mode

  Write_Command(0x15);	//set column address
  Write_Data(0x00);     //column address start 00
  Write_Data(0x7f);     //column address end 95
  Write_Command(0x75);	//set row address
  Write_Data(0x00);     //row address start 00
  Write_Data(0x7f);     //row address end 63	

  Write_Command(0xB3);
  Write_Data(0xF1);

  Write_Command(0xCA);	
  Write_Data(0x7F);

  Write_Command(0xa0);  //set re-map & data format
  Write_Data(0x76);     //Horizontal address increment

  Write_Command(0xa1);  //set display start line
  Write_Data(0x00);     //start 00 line

  Write_Command(0xa2);  //set display offset
  Write_Data(0x00);

  Write_Command(0xAB);	
  Write_Command(0x01);	

  Write_Command(0xB4);	
  Write_Data(0xA0);	  
  Write_Data(0xB5);  
  Write_Data(0x55);    

  Write_Command(0xC1);	
  Write_Data(0xC8);	
  Write_Data(0x80);
  Write_Data(0xC0);

  Write_Command(0xC7);	
  Write_Data(0x0F);

  Write_Command(0xB1);	
  Write_Data(0x32);

  Write_Command(0xB2);	
  Write_Data(0xA4);
  Write_Data(0x00);
  Write_Data(0x00);

  Write_Command(0xBB);	
  Write_Data(0x17);

  Write_Command(0xB6);
  Write_Data(0x01);

  Write_Command(0xBE);
  Write_Data(0x05);

  Write_Command(0xA6);

  Write_Command(0xaf);	 //display on
  
}


