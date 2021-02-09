#include "OLED_Driver.h"
#include "main.h"
#include "gpio.h"
#include "stdlib.h"
#include "spi.h"
#include "font.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "stdlib.h"

uint8_t color_byte[2],color_Damp_byte[2];
uint8_t OLED_GRAM[128][256];
uint8_t OLED_LAY1[128][256];
uint8_t OLED_LAY2[128][256];



#ifdef __cplusplus
extern "C"  {
#endif

OLED_Driver::OLED_Driver(void) {
}
  
uint16_t OLED_Driver::ConvertColor(uint8_t red,uint8_t green,uint8_t blue)
{
	return red<<11|green<<6|blue;
}


uint16_t OLED_Driver::RandomColor(void){
	
	uint8_t red,green,blue;
	red = rand();
	__ASM("NOP");
	green = rand();
	__ASM("NOP");
	blue = rand();
	return (red<<11|green<<5|blue);
}

void OLED_Driver::Set_DampColor(uint16_t color)  {
  
  color_Damp_byte[0] = (uint8_t)(color >> 8);
  color_Damp_byte[1] = (uint8_t)(color & 0x00ff);
}

void OLED_Driver::Write_Command(uint8_t cmd)  {
  OLED_DC_Clr();  //DC拉低，发送命令
  SPI_NSS_LOW(); //片选拉低，选通器件
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 100);
//SPI_RW_Byte(Cmd); //发送数据
  SPI_NSS_HIGH(); //片选拉高，关闭器件
  OLED_DC_Set(); //DC拉高，空闲时为高电平
}


void OLED_Driver::Write_Data(uint8_t dat) {
 
	OLED_DC_Set();  //DC拉高，发送数据
  SPI_NSS_LOW(); //片选拉低，选通器件
  HAL_SPI_Transmit(&hspi2, &dat, 1, 100); //发送数据
  SPI_NSS_HIGH(); //片选拉高，关闭器件
  OLED_DC_Set(); //DC拉高，空闲时为高电平
  
}


void OLED_Driver::Write_Data(uint8_t* dat_p, uint16_t length) {
	int i;
  for(i=0;i<length;i++)
		Write_Data(dat_p[i]);
}


void OLED_Driver::RAM_Address(void)  {
  
  Write_Command(0x15);
  Write_Data(0x00);
  Write_Data(0x7f);

  Write_Command(0x75);
  Write_Data(0x00);
  Write_Data(0x7f);
}


void OLED_Driver::Refrash_Screen(void)  {
  /*
  //Set_Column_Address(0x00,0x7F);  //设置列地址0~127
  Write_Command(0x15);  // 设置列地址
  Write_Data(0x00);  // 列起始地址
  Write_Data(0x7F);  // 列结束地址
  //Set_Row_Address(0x00,0x7F);   //设置行地址0~127
  Write_Command(0x75);  // 设置行地址
  Write_Data(0x00);  // 行起始地址
  Write_Data(0x7F);  // 行结束地址
  //Set_Write_RAM();  //使能MCU写RAM
  Write_Command(0x5C);

  OLED_DC_Set();  //DC拉高，发送数据
  SPI_NSS_LOW(); //片选拉低，选通器件
  HAL_SPI_Transmit_DMA(&hspi3,(uint8_t*)OLED_GRAM,128*128);   //发送显存数组
  
*/
  RAM_Address();
  Write_Command(0x5C);OLED_DC_Set();  //DC拉高，发送数据
  SPI_NSS_LOW(); //片选拉低，选通器件
  HAL_SPI_Transmit_IT(&hspi2,(uint8_t*)OLED_GRAM,128*128*2);
  while(!txcplt)
    osDelay(1);
  txcplt=0;
  //SPI_NSS_HIGH(); //片选拉高，关闭器件
  //OLED_DC_Set(); //DC拉高，空闲时为高电平
  

  /*
  for(i=0;i<128;i++)  {
    for(j=0;j<128;j++)  {
      Write_Data(OLED_GRAM[j*2][i]);//RAM data clear
      Write_Data(OLED_GRAM[j*2+1][i]);//RAM data clear
    }
  }
*/
}
  


void OLED_Driver::Clear_Screen()  {
  
  int i,j;
  
  for(i=0;i<128;i++)  {
    for(j=0;j<128;j++)  {
      OLED_GRAM[i][2*j] = 0;
      OLED_GRAM[1][2*j+1] = 0;
    }
  }
  
}
  
void OLED_Driver::Set_Coordinate(uint16_t x, uint16_t y)  {

  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) 
    return;
  //Set x and y coordinate
  Write_Command(SSD1351_CMD_SETCOLUMN);
  Write_Data(x);
  Write_Data(SSD1351_WIDTH-1);
  Write_Command(SSD1351_CMD_SETROW);
  Write_Data(y);
  Write_Data(SSD1351_HEIGHT-1);
  Write_Command(SSD1351_CMD_WRITERAM);
}
  
void OLED_Driver::Set_Address(uint8_t column, uint8_t row)  {
  
  Write_Command(SSD1351_CMD_SETCOLUMN);  
  Write_Data(column);	//X start 
  Write_Data(column);	//X end 
  Write_Command(SSD1351_CMD_SETROW); 
  Write_Data(row);	//Y start 
  Write_Data(row+7);	//Y end 
  Write_Command(SSD1351_CMD_WRITERAM); 
}
  
  
void  OLED_Driver::Invert(bool v) {
  
  if (v)
    Write_Command(SSD1351_CMD_INVERTDISPLAY);
  else
    Write_Command(SSD1351_CMD_NORMALDISPLAY);
}

void OLED_Driver::Draw_Pixel(int16_t x, int16_t y)
{
  // Bounds check.
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
  if ((x < 0) || (y < 0)) return;

	OLED_GRAM[x][2*y] = color_byte[0];
	OLED_GRAM[x][2*y+1] = color_byte[1];
  
}

void OLED_Driver::Draw_Pixel(int16_t x, int16_t y,uint16_t color)
{
  // Bounds check.
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
  if ((x < 0) || (y < 0)) return;

	OLED_GRAM[x][2*y] = (uint8_t)(color >> 8);
	OLED_GRAM[x][2*y+1] = (uint8_t)(color & 0x00ff);
  
}


  
void OLED_Driver::Device_Init(void) {

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
	Refrash_Screen();
  Write_Command(0xaf);	 //display on
}

  
// Draw a horizontal line ignoring any screen rotation.
void OLED_Driver::Draw_FastHLine(int16_t x, int16_t y, int16_t length) {
  // Bounds check
		int16_t x0=x;
    do
    {
        Draw_Pixel(x, y);   // 逐点显示，描出垂直线
        x++;
    }
    while(x0+length>=x);
}
// Draw a horizontal line ignoring any screen rotation.
void OLED_Driver::Draw_FastHLine(int16_t x, int16_t y, int16_t length,uint16_t color) {
  // Bounds check
		int16_t x0=x;
    do
    {
        Draw_Pixel(x, y,color);   // 逐点显示，描出垂直线
        x++;
    }
    while(x0+length>=x);
}
  
  // Draw a vertical line ignoring any screen rotation.
void OLED_Driver::Draw_FastVLine(int16_t x, int16_t y, int16_t length)  {
  // Bounds check
		int16_t y0=y;
    do
    {
        Draw_Pixel(x, y);   // 逐点显示，描出垂直线
        y++;
    }
    while(y0+length>=y);
}
  // Draw a vertical line ignoring any screen rotation.
void OLED_Driver::Draw_FastVLine(int16_t x, int16_t y, int16_t length,uint16_t color)  {
  // Bounds check
		int16_t y0=y;
    do
    {
        Draw_Pixel(x, y,color);   // 逐点显示，描出垂直线
        y++;
    }
    while(y0+length>=y);
}
  

#ifdef __cplusplus
}
#endif


