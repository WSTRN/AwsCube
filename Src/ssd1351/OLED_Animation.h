#ifndef __OLED_ANIMATION_H
#define __OLED_ANIMATION_H

#include "stm32f4xx_hal.h"
#include "OLED_GFX.h"
#include "stdlib.h"
//#include "sys.h"

#define MINDMAX 20

typedef enum
{
  OLED_OK       = 0x00U,
  OLED_ERROR    = 0x01U,
  OLED_BUSY     = 0x02U,
  lxOLED_IDLE     = 0x03U,
  bxOLED_IDLE     = 0x04U,
  lyOLED_IDLE     = 0x05U,
  byOLED_IDLE     = 0x06U,
} STATUS;

typedef struct
{
  STATUS sta;
	float dirx;//运动的思维点位横坐标运动量
	float diry;//运动的思维点位纵坐标运动量
	//float acc;//
	//uint16_t color;//运动的思维点位颜色
}OLED_STATUS;

extern OLED_GFX oled;
#ifdef __cplusplus
extern "C" {
#endif

  class OLED_Animation {
    
    public: 
			OLED_Animation(void);
			void Motion_MindInit(void);
			void Motion_Mind(void);
    private:  
      OLED_STATUS OLED_MovMind(uint8_t Index);

  };





#ifdef __cplusplus
}
#endif
  
#endif
