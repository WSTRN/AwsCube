#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H

#include "stm32f4xx_hal.h"
#include "arm_math.h"


#define OCX 64
#define OCY 64
//#define PI 3.14159f

#define SSD1351_WIDTH   128
#define SSD1351_HEIGHT  128

#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C
#define SSD1351_CMD_READRAM         0x5D
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET 	0xA2
#define SSD1351_CMD_DISPLAYALLOFF 	0xA4
#define SSD1351_CMD_DISPLAYALLON  	0xA5
#define SSD1351_CMD_NORMALDISPLAY 	0xA6
#define SSD1351_CMD_INVERTDISPLAY 	0xA7
#define SSD1351_CMD_FUNCTIONSELECT 	0xAB
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_DISPLAYON     	0xAF
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_SETGPIO 		    0xB5
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_SETGRAY         0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL 	0xBB
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F


#define swap(a, b) { uint16_t t = a; a = b; b = t; }
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

#define OLED_CS PAout(6)
#define OLED_RST PAout(4)
#define OLED_A0 PAout(5)
#define OLED_WR PBout(0)
#define OLED_RD PBout(1)  

#define DATAOUT(x) GPIOC->ODR = x; //数据输出
#define DATAIN     GPIOC->IDR;   //数据输入

#define OLED_CMD  0		    //写命令
#define OLED_DATA 1				//写数据

#define		Black			0x0000		//黑色（不亮）
#define		Navy			0x000F		//深蓝色
#define		Dgreen		0x03E0		//深绿色
#define		Dcyan			0x03EF		//深青色
#define		Maroon		0x7800		//深红色
#define		Purple		0x780F		//紫色
#define		Olive			0x7BE0		//橄榄绿
#define		Lgray			0xC618		//灰白色
#define		Dgray			0x7BEF		//深灰色
#define		Blue			0x001F		//蓝色
#define		Green			0x07E0		//绿色
#define		Cyan			0x07FF		//青色
#define		Red				0xF800		//红色
#define		Magenta		0xF81F		//品红
#define		Yellow		0xFFE0		//黄色
#define		White			0xFFFF		//白色


#define GL_RGB(r, g, b) (uint32_t)((0xFF << 24) | (((unsigned int)(r)) << 16) | (((unsigned int)(g)) << 8) | ((unsigned int)(b)))
#define GL_RGB_32_to_16(rgb) (uint16_t)(((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))

#ifdef __cplusplus
extern "C" {
#endif

  class OLED_Driver {
    
    public: 
      uint8_t txcplt;
      OLED_Driver(void);
		
			void MOLED_Fill(uint16_t data);
		
			uint16_t ConvertColor(uint8_t red,uint8_t green,uint8_t blue);
      void Device_Init(void);
      void Clear_Screen(void);
      void Refrash_Screen(void);
      void Fill_Color(uint16_t color);
      void Fill_Color(void);
      void Set_Coordinate(uint16_t x, uint16_t y);
			uint16_t RandomColor(void);

      void Write_text(uint8_t data1);
      void Set_Address(uint8_t column, uint8_t row);
      
      void Set_Color(uint16_t color);
      void Set_FillColor(uint16_t color);
    
      void Invert(bool v);
      void Draw_Pixel(int16_t x, int16_t y);
      void Draw_Pixel(int16_t x, int16_t y,uint16_t color);
      //void Draw_Pixel(int16_t x, int16_t y,uint16_t color);
    
      void Write_Data(uint8_t dat);
      void Write_Data(uint8_t* dat_p, uint16_t length);
    
      void Draw_FastHLine(int16_t x, int16_t y, int16_t length);
      void Draw_FastHLine(int16_t x, int16_t y, int16_t length,uint16_t color);
			
      void Draw_FastVLine(int16_t x, int16_t y, int16_t length);
      void Draw_FastVLine(int16_t x, int16_t y, int16_t length,uint16_t color);
      void Write_Command(uint8_t data1);
			
			
			void Set_DampColor(uint16_t color);
			void Set_Wheel(uint16_t WheelPos);
			void Set_Wheelf(uint16_t WheelPos);
			uint16_t WheelP(uint16_t WheelPos);
    private:  
      uint8_t Float2U8(float Input);
      void RAM_Address(void);
			void Calc_Color(void);
  };



#ifdef __cplusplus
}
#endif
  
#endif

