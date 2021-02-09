#ifndef __OLED_GFX_H
#define __OLED_GFX_H
//#define __cplusplus
#include "stm32f4xx_hal.h"

#include "OLED_Driver.h"

typedef enum  {
  FONT_5X8 = 0,
  FONT_8X16
}FONT_SIZE;

#ifdef __cplusplus
extern "C"  {
#endif
  
  
typedef struct struct_bitmap_info
{
	unsigned short width;
	unsigned short height;
	unsigned short color_bits;//support 16 bits only
	const unsigned short* pixel_color_array;
} BITMAP_INFO;

class OLED_GFX : public virtual OLED_Driver {
  
  public:
      
    OLED_GFX(void);
      
    void Set_FontSize(FONT_SIZE size);
    void Set_TextCoordinate(uint8_t x, uint8_t y);
    
    void Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
	
		void Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t with, uint16_t color);
    void Draw_LineA(float x0, float y0, float agl, uint8_t lenth ,uint8_t with,uint16_t color);
    void Draw_LineS(int16_t *points, uint8_t no);
    void Draw_LineS(int16_t *points, uint8_t no,uint16_t color);
    
    
    void Draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h);
    void Draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color);
    void Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t color);
    
    void Fill_Circle(int16_t x0, int16_t y0, int16_t r);
    void Fill_Circle(int16_t x0, int16_t y0, int16_t r,uint16_t color);
    void Draw_Circle(int16_t x0, int16_t y0, int16_t r);
    void Draw_Circle(int16_t x0, int16_t y0, int16_t r,uint16_t color);
		void Draw_Circle(int16_t x0, int16_t y0, int16_t r,uint8_t Type,uint16_t color);

    void Draw_RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
    void Draw_RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,uint16_t color);
    void Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,uint16_t color);
    
    void print_String(uint8_t x, uint8_t y, const uint8_t *text, FONT_SIZE size);
    void Display_String_5x8(uint8_t x, uint8_t y, const uint8_t *text);
    void Display_String_8x16(uint8_t x, uint8_t y, const uint8_t *text);
    void DrawFont16x8( const char* str, int16_t row, int16_t column, uint16_t color );
    void DrawFont24x12( const char* str, int16_t row, int16_t column, uint16_t color );
    void DrawFont32x16( const char* str, int16_t row, int16_t column, uint16_t color );
    void Draw_bitmap( const BITMAP_INFO *pBitmap, int x, int y);

  private:
    
    void Write_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void Write_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
    
    void FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta);
    void FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta,uint16_t color);
    void DrawCircle_Helper( int16_t x0, int16_t y0, int16_t r, uint8_t corner);
    void DrawCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner,uint16_t color);

};


#ifdef __cplusplus
}
#endif

#endif
