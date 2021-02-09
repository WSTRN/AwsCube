#include "OLED_Animation.h"
#include "main.h"
#include "gpio.h"
//#include "sys.h"
#include "math.h"
#include "arm_math.h"

#ifdef __cplusplus
extern "C"  {
#endif
	
OLED_Animation::OLED_Animation(void) {
}

typedef struct
{
	float x;//运动的思维点位横坐标
	float y;//运动的思维点位纵坐标
	float dirx;//运动的思维点位横坐标运动量
	float diry;//运动的思维点位纵坐标运动量
	float r;//
	uint16_t color;//运动的思维点位颜色
}MTMOVMIND;

MTMOVMIND mtmovmind[MINDMAX];
  

OLED_STATUS OLED_Animation::OLED_MovMind(uint8_t Index)
{
	OLED_STATUS oledsta;
	if(mtmovmind[Index].x<=3)
	{	
		mtmovmind[Index].x = 4;
		oledsta.dirx=mtmovmind[Index].dirx;
		oledsta.diry=mtmovmind[Index].diry;
		oledsta.sta=lxOLED_IDLE;
		return oledsta;
	}
	else if(mtmovmind[Index].x>=SSD1351_WIDTH-2)
	{
		mtmovmind[Index].x = SSD1351_WIDTH-3;
		oledsta.dirx=mtmovmind[Index].dirx;
		oledsta.diry=mtmovmind[Index].diry;
		oledsta.sta=bxOLED_IDLE;
		return oledsta;
	}
	else if(mtmovmind[Index].y<=3)
	{
		mtmovmind[Index].y = 4;
		oledsta.dirx=mtmovmind[Index].dirx;
		oledsta.diry=mtmovmind[Index].diry;
		oledsta.sta=lyOLED_IDLE;
		return oledsta;
	}
	else if(mtmovmind[Index].y>=SSD1351_HEIGHT-2)
	{
		mtmovmind[Index].y = SSD1351_HEIGHT-3;
		oledsta.dirx=mtmovmind[Index].dirx;
		oledsta.diry=mtmovmind[Index].diry;
		oledsta.sta=byOLED_IDLE;
		return oledsta;
	}
	else
	{
		mtmovmind[Index].x += mtmovmind[Index].dirx;
		mtmovmind[Index].y += mtmovmind[Index].diry;
		//oledsta.dirx=mtmovmind[Index].dirx;
		//oledsta.diry=mtmovmind[Index].diry;
		oledsta.sta=OLED_BUSY;
  		return oledsta;
	}
}

uint16_t GetMindDistanceSquare(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{
	return ((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1));
}

void OLED_Animation::Motion_MindInit(void)
{
	int i;
	for(i=0;i<MINDMAX;i++)
	{
		mtmovmind[i].color = oled.RandomColor();
		__ASM("NOP");
		mtmovmind[i].x = rand()%(SSD1351_WIDTH-4)+2;
		__ASM("NOP");
		mtmovmind[i].y = rand()%(SSD1351_HEIGHT-4)+2;
		
		mtmovmind[i].dirx = (rand()%20-10)*0.2f;
		mtmovmind[i].diry = (rand()%20-10)*0.2f;
		if(mtmovmind[i].dirx<0.2f&&mtmovmind[i].dirx>-0.2f)
			mtmovmind[i].dirx = 0.2;
		if(mtmovmind[i].diry<0.2f&&mtmovmind[i].diry>-0.2f)
			mtmovmind[i].diry = 0.2;
	}
}


void OLED_Animation::Motion_Mind(void)
{
	int i,j;
	for(i=0;i<MINDMAX;i++)
	{
		OLED_STATUS sta=OLED_MovMind(i);
		if((sta.sta == lxOLED_IDLE)||(sta.sta ==bxOLED_IDLE)||(sta.sta ==lyOLED_IDLE)||(sta.sta ==byOLED_IDLE))
		{
			//float mult=pow(2.0f,(rand()%3)-1.0f);
			switch(sta.sta)
			{
				
				case lxOLED_IDLE:
					mtmovmind[i].dirx = -sta.dirx;
					mtmovmind[i].diry = sta.diry;
					break;
				case bxOLED_IDLE:
					mtmovmind[i].dirx = -sta.dirx;
					mtmovmind[i].diry = sta.diry;
					break;
				case lyOLED_IDLE:
					mtmovmind[i].dirx = sta.dirx;
					mtmovmind[i].diry = -sta.diry;
					break;
				case byOLED_IDLE:
					mtmovmind[i].dirx = sta.dirx;
					mtmovmind[i].diry = -sta.diry;
					break;

			}
			mtmovmind[i].color = oled.RandomColor();
			//mtmovmind[i].dirx = (rand()%30-15)*0.1f;
			//mtmovmind[i].diry = (rand()%30-15)*0.1f;
			//if(mtmovmind[i].dirx<0.2f&&mtmovmind[i].dirx>-0.2f)
			//	mtmovmind[i].dirx = 0.5;
			//if(mtmovmind[i].diry<0.2f&&mtmovmind[i].diry>-0.2f)
			//	mtmovmind[i].diry = 0.5;
		}
	}
	
	for(i=0;i<MINDMAX;i++)
	{
		for(j=i+1;j<MINDMAX;j++)
		{
			if((mtmovmind[i].x-mtmovmind[j].x)*(mtmovmind[i].x-mtmovmind[j].x)+(mtmovmind[i].y-mtmovmind[j].y)*(mtmovmind[i].y-mtmovmind[j].y)<900)
			{
				oled.Draw_Line(mtmovmind[j].x,mtmovmind[j].y,mtmovmind[i].x,mtmovmind[i].y,Dgray);
			}
			/*
			if((mtmovmind[i].x-mtmovmind[j].x)*(mtmovmind[i].x-mtmovmind[j].x)+(mtmovmind[i].y-mtmovmind[j].y)*(mtmovmind[i].y-mtmovmind[j].y)<16)
			{
				float tx=mtmovmind[i].dirx;
				float ty=mtmovmind[i].diry;
				mtmovmind[i].dirx=mtmovmind[j].dirx;
				mtmovmind[i].diry=mtmovmind[j].diry;
				mtmovmind[i].x+=4*mtmovmind[i].dirx;
				mtmovmind[i].y+=4*mtmovmind[i].diry;
				mtmovmind[j].diry=tx;
				mtmovmind[j].diry=ty;
				mtmovmind[j].x+=4*mtmovmind[j].dirx;
				mtmovmind[j].y+=4*mtmovmind[j].diry;
			}
			*/
		}
	}
	
	for(i=0;i<MINDMAX;i++)
		oled.Fill_Circle(mtmovmind[i].x,mtmovmind[i].y,2,mtmovmind[i].color);
}

#ifdef __cplusplus
}
#endif


