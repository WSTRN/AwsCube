#pragma once


#include "spi.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

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


static uint8_t OLED_GRAM[128][256];

void uitask_init();

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
static void RAM_Address(void)  {
  
  Write_Command(0x15);
  Write_Data(0x00);
  Write_Data(0x7f);

  Write_Command(0x75);
  Write_Data(0x00);
  Write_Data(0x7f);
}
static void Clear_Screen()  {
  
  int i,j;
  
  for(i=0;i<128;i++)  {
    for(j=0;j<128;j++)  {
      OLED_GRAM[i][2*j] = 0x00;
      OLED_GRAM[1][2*j+1] = 0x00;
    }
  }
  
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
  Write_Data(0x74);     //Horizontal address increment

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

  Clear_Screen();
  //Refrash_Screen();
  Write_Command(0xaf);	 //display on
  
}


