#include "OLED_GFX.h"
#include "font.h"
#include "stdlib.h"
#include "arm_math.h"
#define swap(a, b) { uint16_t t = a; a = b; b = t; }
#define ROUND_TO_UINT16(x)   ((uint16_t)(x)+0.5)>(x)? ((uint16_t)(x)):((uint16_t)(x)+1)

uint8_t Row,Column;
FONT_SIZE Font_Size;
extern uint8_t OLED_GRAM[128][256];
extern uint8_t OLED_LAY1[128][256];
extern uint8_t OLED_LAY2[128][256];

#ifdef __cplusplus
extern "C"  {
#endif
  
OLED_GFX::OLED_GFX(void)  {
  Row = 0;
  Column = 0;
  Font_Size = FONT_5X8;
}
  
 
void OLED_GFX::Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  // Update in subclasses if desired!
  if(x0 == x1)  {
    if(y0 > y1) swap(y0, y1);
    Draw_FastVLine(x0, y0, y1 - y0 );
  } 
  else if(y0 == y1) {
    if(x0 > x1)
      swap(x0, x1);
    Draw_FastHLine(x0, y0, x1 - x0 );
  }
  else
    Write_Line(x0, y0, x1, y1);
}
  
void OLED_GFX::Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color) {
  // Update in subclasses if desired!
  if(x0 == x1)  {
    if(y0 > y1) swap(y0, y1);
    Draw_FastVLine(x0, y0, y1 - y0 ,color);
  } 
  else if(y0 == y1) {
    if(x0 > x1)
      swap(x0, x1);
    Draw_FastHLine(x0, y0, x1 - x0 ,color);
  }
  else
    Write_Line(x0, y0, x1, y1,color);
}
  
void  OLED_GFX::Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t with, uint16_t color)
{ 

   
  int32_t   dx;						// 直线x轴差值变量
  int32_t   dy;          			// 直线y轴差值变量
  int16_t    dx_sym;					// x轴增长方向，为-1时减值方向，为1时增值方向
  int16_t    dy_sym;					// y轴增长方向，为-1时减值方向，为1时增值方向
  int32_t   dx_x2;					// dx*2值变量，用于加快运算速度
  int32_t   dy_x2;					// dy*2值变量，用于加快运算速度
  int32_t   di;						// 决策变量
  
  int32_t   wx, wy;					// 线宽变量
  int32_t   draw_a, draw_b;

 
  
  if(with==0) return;
  if(with>50) with = 50;
  
  dx = x1-x0;						// 求取两点之间的差值
  dy = y1-y0;
  
  wx = with/2;
  wy = with-wx-1;
  
  //判断增长方向，或是否为水平线、垂直线、点 
  
  if(dx>0)							// 判断x轴方向
  {  dx_sym = 1;					// dx>0，设置dx_sym=1
  }
  else
  {  if(dx<0)
    {  dx_sym = -1;				// dx<0，设置dx_sym=-1
    }
    else
    {  // dx==0，画垂直线，或一点 
        wx = x0-wx;
        if(wx<0) wx = 0;
        wy = x0+wy;
        
        while(1)
        {  x0 = wx;
          
          if(y0 > y1)
            swap(y0, y1);
            Draw_FastVLine(x0, y0, y1 - y0, color);
          if(wx>=wy) break;
          wx++;
        }
        
        return;
    }
  }
  
  if(dy>0)							// 判断y轴方向
  {  dy_sym = 1;					// dy>0，设置dy_sym=1
  }
  else
  {  if(dy<0)
    {  dy_sym = -1;				// dy<0，设置dy_sym=-1
    }
    else
    {  // dy==0，画水平线，或一点 
        wx = y0-wx;
        if(wx<0) wx = 0;
        wy = y0+wy;
        
        while(1)
        {  y0 = wx;
          if(x0 > x1)
            swap(x0, x1);
            Draw_FastHLine(x0, y0, x1 - x0 ,color);
          if(wx>=wy) break;
          wx++;
        }
        return;
    }
  }
  
  // 将dx、dy取绝对值 
  dx = dx_sym * dx;
  dy = dy_sym * dy;

  // 计算2倍的dx及dy值 
  dx_x2 = dx*2;
  dy_x2 = dy*2;
  
  // 使用Bresenham法进行画直线 
  if(dx>=dy)						// 对于dx>=dy，则使用x轴为基准
  {  di = dy_x2 - dx;
    while(x0!=x1)
    {  // x轴向增长，则宽度在y方向，即画垂直线 
        draw_a = y0-wx;
        if(draw_a<0) draw_a = 0;
        draw_b = y0+wy;
      if(draw_a > draw_b)
        swap(draw_a, draw_b);
        Draw_FastVLine(x0, draw_a, draw_b - draw_a, color);
        
        x0 += dx_sym;				
        if(di<0)
        {  di += dy_x2;			// 计算出下一步的决策值
        }
        else
        {  di += dy_x2 - dx_x2;
          y0 += dy_sym;
        }
    }
    draw_a = y0-wx;
    if(draw_a<0) draw_a = 0;
    draw_b = y0+wy;
      if(draw_a > draw_b)
        swap(draw_a, draw_b);
    Draw_FastVLine(x0, draw_a, draw_b - draw_a, color);
  }
  else								// 对于dx<dy，则使用y轴为基准
  {  di = dx_x2 - dy;
    while(y0!=y1)
    {  // y轴向增长，则宽度在x方向，即画水平线 
        draw_a = x0-wx;
        if(draw_a<0) draw_a = 0;
        draw_b = x0+wy;
      if(draw_a > draw_b)
        swap(draw_a, draw_b);
        Draw_FastHLine(draw_a, y0, draw_b - draw_a, color);
        
        y0 += dy_sym;
        if(di<0)
        {  di += dx_x2;
        }
        else
        {  di += dx_x2 - dy_x2;
          x0 += dx_sym;
        }
    }
    draw_a = x0-wx;
    if(draw_a<0) draw_a = 0;
    draw_b = x0+wy;
      if(draw_a > draw_b)
        swap(draw_a, draw_b);
    Draw_FastHLine(draw_a, y0, draw_b - draw_a, color);
  } 

/*
int dx, dy, x, y, e;
		(x1 > x2) ? (dx = x1 - x2) : (dx = x2 - x1);
		(y1 > y2) ? (dy = y1 - y2) : (dy = y2 - y1);
		if (((dx > dy) && (x1 > x2)) || ((dx <= dy) && (y1 > y2)))
		{
			x = x2; y = y2;
			x2 = x1; y2 = y1;
			x1 = x; y1 = y;
		}
		x = x1; y = y1;
		if (dx > dy)
		{
			e = dy - dx / 2;
			for (; x1 <= x2; ++x1, e += dy)
			{
				Draw_Pixel(x1, y1, color);
				if (e > 0) { e -= dx; (y > y2) ? --y1 : ++y1; }
			}
		}
		else
		{
			e = dx - dy / 2;
			for (; y1 <= y2; ++y1, e += dx)
			{
				Draw_Pixel(x1, y1, color);
				if (e > 0) { e -= dy; (x > x2) ? --x1 : ++x1; }
			}
		}
*/

}

void OLED_GFX::Draw_LineA(float x0, float y0, float agl, uint8_t lenth ,uint8_t with ,uint16_t color)
{
  float vs,vc;
  arm_sin_cos_f32(agl,&vs,&vc);
  for(;lenth>0;lenth--)
  {
    Fill_Circle(ROUND_TO_UINT16(-vs*lenth+x0),ROUND_TO_UINT16(vc*lenth+y0),with/2,color);
  }
  //Fill_Circle(x0,y0,with/2,color);
  //Draw_Line(ROUND_TO_UINT16(-vs*0.2f+x0),ROUND_TO_UINT16(vc*0.2f+y0),ROUND_TO_UINT16(-vs*lenth+x0),ROUND_TO_UINT16(vc*lenth+y0),with,color);
}

void OLED_GFX::Write_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1)  {
    
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    
  if (steep)  {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1)  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if(y0 < y1) {
    ystep = 1;
  } 
  else  {
    ystep = -1;
  }

  for(; x0<=x1; x0++) {
    if(steep)
      Draw_Pixel(y0, x0);
    else
      Draw_Pixel(x0, y0);
    err -= dy;
      
    if(err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void OLED_GFX::Write_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color)  {
    
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    
  if (steep)  {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1)  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if(y0 < y1) {
    ystep = 1;
  } 
  else  {
    ystep = -1;
  }

  for(; x0<=x1; x0++) {
    if(steep)
      Draw_Pixel(y0, x0,color);
    else
      Draw_Pixel(x0, y0,color);
    err -= dy;
      
    if(err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

  // Draw a rectangle
void OLED_GFX::Draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h) {
    
  Draw_FastHLine(x, y, w);
  Draw_FastHLine(x, y+h, w);
  Draw_FastVLine(x, y, h);
  Draw_FastVLine(x+w, y, h);
}
  // Draw a rectangle
void OLED_GFX::Draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color) {
    
  Draw_FastHLine(x, y, w, color);
  Draw_FastHLine(x, y+h, w, color);
  Draw_FastVLine(x, y, h, color);
  Draw_FastVLine(x+w, y, h, color);
}

/**************************************************************************/
/*!
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void OLED_GFX::Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

	uint16_t i,j;
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
  return;

  // Y bounds check
  if (y+h > SSD1351_HEIGHT) {
    h = SSD1351_HEIGHT - y;
  }

  // X bounds check
  if (x+w > SSD1351_WIDTH)  {
    w = SSD1351_WIDTH - x;
  }

	for(i=x;i<=w+x;i++)
		for(j=y;j<=h+y;j++)
			Draw_Pixel(i,j);
}

/**************************************************************************/
/*!
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void OLED_GFX::Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t color) {

	uint16_t i,j;
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
  return;

  // Y bounds check
  if (y+h > SSD1351_HEIGHT) {
    h = SSD1351_HEIGHT - y ;
  }

  // X bounds check
  if (x+w > SSD1351_WIDTH)  {
    w = SSD1351_WIDTH - x ;
  }

	for(i=x;i<=w+x;i++)
		for(j=y;j<=h+y;j++)
			Draw_Pixel(i,j, color);
}
void OLED_GFX::Fill_Circle(int16_t x0, int16_t y0, int16_t r) {
  
  Draw_FastVLine(x0, y0-r, 2*r);
  FillCircle_Helper(x0, y0, r, 3, 0);
}
void OLED_GFX::Fill_Circle(int16_t x0, int16_t y0, int16_t r,uint16_t color) {
  
  Draw_FastVLine(x0, y0-r, 2*r, color);
  FillCircle_Helper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void OLED_GFX::FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner, int16_t delta)  {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while(x<y)  {
    if(f >= 0)  {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if(corner & 0x1)  {
      Draw_FastVLine(x0+x, y0-y, 2*y+delta);
      Draw_FastVLine(x0+y, y0-x, 2*x+delta);
    }
    if(corner & 0x2)  {
      Draw_FastVLine(x0-x, y0-y, 2*y+delta);
      Draw_FastVLine(x0-y, y0-x, 2*x+delta);
    }
  }
}

// Used to do circles and roundrects
void OLED_GFX::FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner, int16_t delta,uint16_t color)  {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while(x<y)  {
    if(f >= 0)  {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if(corner & 0x1)  {
      Draw_FastVLine(x0+x, y0-y, 2*y+delta, color);
      Draw_FastVLine(x0+y, y0-x, 2*x+delta, color);
    }
    if(corner & 0x2)  {
      Draw_FastVLine(x0-x, y0-y, 2*y+delta, color);
      Draw_FastVLine(x0-y, y0-x, 2*x+delta, color);
    }
  }
}

// Draw a circle outline
void OLED_GFX::Draw_Circle(int16_t x0, int16_t y0, int16_t r) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  Draw_Pixel(x0  , y0+r);
  Draw_Pixel(x0  , y0-r);
  Draw_Pixel(x0+r, y0  );
  Draw_Pixel(x0-r, y0  );

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    Draw_Pixel(x0 + x, y0 + y);
    Draw_Pixel(x0 - x, y0 + y);
    Draw_Pixel(x0 + x, y0 - y);
    Draw_Pixel(x0 - x, y0 - y);
    Draw_Pixel(x0 + y, y0 + x);
    Draw_Pixel(x0 - y, y0 + x);
    Draw_Pixel(x0 + y, y0 - x);
    Draw_Pixel(x0 - y, y0 - x);
    }
}
// Draw a circle outline
void OLED_GFX::Draw_Circle(int16_t x0, int16_t y0, int16_t r,uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  Draw_Pixel(x0  , y0+r, color);
  Draw_Pixel(x0  , y0-r, color);
  Draw_Pixel(x0+r, y0  , color);
  Draw_Pixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    Draw_Pixel(x0 + x, y0 + y, color);
    Draw_Pixel(x0 - x, y0 + y, color);
    Draw_Pixel(x0 + x, y0 - y, color);
    Draw_Pixel(x0 - x, y0 - y, color);
    Draw_Pixel(x0 + y, y0 + x, color);
    Draw_Pixel(x0 - y, y0 + x, color);
    Draw_Pixel(x0 + y, y0 - x, color);
    Draw_Pixel(x0 - y, y0 - x, color);
    }
}

void OLED_GFX::Draw_Circle(int16_t x0, int16_t y0, int16_t r,uint8_t Type,uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
	switch(Type)
	{
		case 1:
				Draw_Pixel(x0  , y0+r, color);
				Draw_Pixel(x0+r, y0  , color);break;//右下
		case 2:
				Draw_Pixel(x0  , y0+r, color);
				Draw_Pixel(x0-r, y0  , color);break;//左下
		case 3:
				Draw_Pixel(x0  , y0-r, color);
				Draw_Pixel(x0+r, y0  , color);break;//右上
		case 4:
				Draw_Pixel(x0-r, y0  , color);
				Draw_Pixel(x0  , y0-r, color);break;//左上
	}

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

	switch(Type)
	{
		case 1:
    Draw_Pixel(x0 + x, y0 + y, color);
    Draw_Pixel(x0 + y, y0 + x, color);break;
		case 2:
    Draw_Pixel(x0 - x, y0 + y, color);
    Draw_Pixel(x0 - y, y0 + x, color);break;
		case 3:
    Draw_Pixel(x0 + x, y0 - y, color);
    Draw_Pixel(x0 + y, y0 - x, color);break;
		case 4:
    Draw_Pixel(x0 - y, y0 - x, color);
    Draw_Pixel(x0 - x, y0 - y, color);break;
	}
		
		
		
    }
}

// Draw a rounded rectangle
void OLED_GFX::Draw_RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r) {
    // smarter version

    Draw_FastHLine(x+r  , y    , w-2*r); // Top
    Draw_FastHLine(x+r  , y+h-1, w-2*r); // Bottom
    Draw_FastVLine(x    , y+r  , h-2*r); // Left
    Draw_FastVLine(x+w-1, y+r  , h-2*r); // Right
    // draw four corners
    DrawCircle_Helper(x+r    , y+r    , r, 1);
    DrawCircle_Helper(x+w-r-1, y+r    , r, 2);
    DrawCircle_Helper(x+w-r-1, y+h-r-1, r, 4);
    DrawCircle_Helper(x+r    , y+h-r-1, r, 8);
}
// Draw a rounded rectangle
void OLED_GFX::Draw_RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,uint16_t color) {
    // smarter version

    Draw_FastHLine(x+r  , y    , w-2*r, color); // Top
    Draw_FastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    Draw_FastVLine(x    , y+r  , h-2*r, color); // Left
    Draw_FastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    DrawCircle_Helper(x+r    , y+r    , r, 1, color);
    DrawCircle_Helper(x+w-r-1, y+r    , r, 2, color);
    DrawCircle_Helper(x+w-r-1, y+h-r-1, r, 4, color);
    DrawCircle_Helper(x+r    , y+h-r-1, r, 8, color);
}

void OLED_GFX::DrawCircle_Helper( int16_t x0, int16_t y0, int16_t r, uint8_t corner) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4) {
      Draw_Pixel(x0 + x, y0 + y);
      Draw_Pixel(x0 + y, y0 + x);
    }
    if (corner & 0x2) {
      Draw_Pixel(x0 + x, y0 - y);
      Draw_Pixel(x0 + y, y0 - x);
    }
    if (corner & 0x8) {
      Draw_Pixel(x0 - y, y0 + x);
      Draw_Pixel(x0 - x, y0 + y);
    }
    if (corner & 0x1) {
      Draw_Pixel(x0 - y, y0 - x);
      Draw_Pixel(x0 - x, y0 - y);
    }
  }
}


void OLED_GFX::DrawCircle_Helper( int16_t x0, int16_t y0, int16_t r, uint8_t corner,uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4) {
      Draw_Pixel(x0 + x, y0 + y, color);
      Draw_Pixel(x0 + y, y0 + x, color);
    }
    if (corner & 0x2) {
      Draw_Pixel(x0 + x, y0 - y, color);
      Draw_Pixel(x0 + y, y0 - x, color);
    }
    if (corner & 0x8) {
      Draw_Pixel(x0 - y, y0 + x, color);
      Draw_Pixel(x0 - x, y0 + y, color);
    }
    if (corner & 0x1) {
      Draw_Pixel(x0 - y, y0 - x, color);
      Draw_Pixel(x0 - x, y0 - y, color);
    }
  }
}

// Draw a triangle
void OLED_GFX::Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)  {
  Draw_Line(x0, y0, x1, y1);
  Draw_Line(x1, y1, x2, y2);
  Draw_Line(x2, y2, x0, y0);
}
// Draw a triangle
void OLED_GFX::Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,uint16_t color)  {
  Draw_Line(x0, y0, x1, y1, color);
  Draw_Line(x1, y1, x2, y2, color);
  Draw_Line(x2, y2, x0, y0, color);
}

void OLED_GFX::DrawFont16x8(const char* str, int16_t row, int16_t column, uint16_t color )
{
  if( row > 128 -16 )
			return;
		
		uint8_t str_ind = 0;
		while( column < 128 )
		{
			uint8_t s = str[str_ind];
			if( s>=29 && s<=126 )				
				s -= 29;
			else
				return;
			
			for( uint8_t i = 0; i < 16; ++i )
			{
				//读取对应字体
				uint8_t f = Font_16x8[s][i];
				//uint16_t p = (row+i)*128 + column;
				for( uint8_t k = 0; k < 8; ++k )
				{
					if( ((f>>k) & 1)&&(k+column>=0)&&(k+column<=127) )
          {
            
            OLED_GRAM[row+i][2*(k+column)]=(uint8_t)(color >> 8);
            OLED_GRAM[row+i][2*(k+column)+1]=(uint8_t)(color & 0x00ff);
          }
            
						//((uint16_t*)vram)[p+k] = (color<<8)|(color>>8);
				}
			}
			++str_ind;
			column += 8;
		}
}
void OLED_GFX::DrawFont24x12(const char* str, int16_t row, int16_t column, uint16_t color )
{
  if( row > 128 - 24 )
    return;
  
  uint8_t str_ind = 0;
  while( column < 128  )
  {
    uint8_t s = str[str_ind];
    if( s>=32 && s<=126 )				
      s -= 32;
    else
      return;
    
    for( uint8_t i = 0; i < 24; ++i )
    {
      //读取对应字体
      uint16_t f = ((uint16_t*)Font_24x12[s])[i];
      //uint16_t p = (row+i)*128 + column;
      for( uint8_t k = 0; k < 12; ++k )
      {
        if(( (f>>k) & 1)&&(k+column>=0)&&(k+column<=127) )
        {
          OLED_GRAM[row+i][2*(k+column)]=(uint8_t)(color >> 8);
          OLED_GRAM[row+i][2*(k+column)+1]=(uint8_t)(color & 0x00ff);
        }
          //((uint16_t*)vram)[p+k] = (color<<8)|(color>>8);
      }
    }
    ++str_ind;
    column += 12;
  }

}

void OLED_GFX::DrawFont32x16(const char* str, int16_t row, int16_t column, uint16_t color )
{
  if( row > 128 - 32 )
    return;
  
  uint8_t str_ind = 0;
  while( column < 128  )
  {
    uint8_t s = str[str_ind];
    if( s>=32 && s<=126 )				
      s -= 32;
    else
      return;
    
    for( uint8_t i = 0; i < 32; ++i )
    {
      //读取对应字体
      uint16_t f = ((uint16_t*)Font_32x16[s])[i];
      //uint16_t p = (row+i)*128 + column;
      for( uint8_t k = 0; k < 16; ++k )
      {
        if( ((f>>k) & 1)&&(k+column>=0)&&(k+column<=127) )
        {
          
          OLED_GRAM[row+i][2*(k+column)]=(uint8_t)(color >> 8);
          OLED_GRAM[row+i][2*(k+column)+1]=(uint8_t)(color & 0x00ff);
            
        }
          //((uint16_t*)vram)[p+k] = (color<<8)|(color>>8);
      }
    }
    ++str_ind;
    column += 16;
  }
}

void OLED_GFX::Draw_bitmap(const BITMAP_INFO *pBitmap, int x, int y)
{
  int xsize = pBitmap->width;
  int ysize = pBitmap->height;
  const unsigned short* pData = (const unsigned short*)pBitmap->pixel_color_array;
  for (int y_ = y; y_ < y + ysize; y_++)
  {
    for (int x_ = x; x_ < x + xsize; x_++)
    {
      unsigned int rgb = *pData++;
      Draw_Pixel(y_, x_, rgb);
    }
  }
}

#ifdef __cplusplus
}
#endif

