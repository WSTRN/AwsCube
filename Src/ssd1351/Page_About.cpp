#include "DisplayBasic.hpp"
#include "CommonMacro.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "gpio.h"
#include "SEGGER_RTT.h"
#include "ButtonEvent.h"
#include <math.h> 
#include "usart.h"

static lv_obj_t * appWindow;
static uint8_t ThisPage;
static lv_obj_t * Logo_1;
static lv_obj_t * Logo_2;
static lv_obj_t * espFlash;
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup(int arg)
{
    /*将此页面移到前台*/
    lv_obj_set_pos(appWindow,0,arg>0?128:-128);
    lv_obj_move_foreground(appWindow);

    LV_IMG_DECLARE(ImgAwsCube);
    Logo_2 = lv_img_create(appWindow, NULL);
    lv_img_set_src(Logo_2, &ImgAwsCube);
    lv_obj_align(Logo_2, appWindow, LV_ALIGN_CENTER, 0, -26);

    LV_IMG_DECLARE(ImgPoweredBy);
    Logo_1 = lv_img_create(appWindow, NULL);
    lv_img_set_src(Logo_1, &ImgPoweredBy);
    lv_obj_align(Logo_1, appWindow, LV_ALIGN_CENTER, 0, 30);

	// /*espFlashStatus*/
    static lv_style_t led_style;
    lv_style_init(&led_style);
    lv_style_set_bg_color(&led_style,LV_STATE_DEFAULT,LV_COLOR_RED);
    lv_style_set_border_color(&led_style,LV_STATE_DEFAULT,LV_COLOR_RED);
    espFlash = lv_led_create(appWindow, NULL);
  	lv_obj_add_style(espFlash,LV_LED_PART_MAIN,&led_style);
    lv_obj_set_size(espFlash, 6, 6);
    lv_led_set_bright(espFlash,190);
  	lv_obj_align(espFlash, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    //Power_SetAutoLowPowerEnable(true);
    HAL_GPIO_WritePin(ExtPower_GPIO_Port, ExtPower_Pin, GPIO_PIN_RESET);

    //Animation
    static lv_anim_t anim_setup;
    lv_anim_init(&anim_setup);
    lv_anim_path_t path;
    lv_anim_path_init(&path);
    /*计算需要的Y偏移量*/
    lv_coord_t y_offset = arg>0?128:-128;
    /*滑动动画*/
    lv_anim_path_set_cb(&path, lv_anim_path_ease_out);
    //lv_anim_set_ready_cb(&anim_setup, MainClock_exit_cb);
    lv_anim_set_exec_cb(&anim_setup,(lv_anim_exec_xcb_t) lv_obj_set_y);
    lv_anim_set_var(&anim_setup,appWindow);
    lv_anim_set_time(&anim_setup,300);
    lv_anim_set_values(&anim_setup,lv_obj_get_y(appWindow),lv_obj_get_y(appWindow)-y_offset);
    lv_anim_set_path(&anim_setup,&path);
    lv_anim_start(&anim_setup);

  
}
void About_exit_cb(lv_anim_t* a)
{
   /*关任务*/
    //lv_task_del(taskTimeUpdate);
    //lv_task_del(taskTopBarUpdate);
    
    /*删除此页面上的子控件*/
    lv_obj_clean(appWindow);
    
    /*禁用自动关机*/
    //Power_SetAutoLowPowerEnable(false);
}
/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit(int arg)
{

  static lv_anim_t anim_exit;
  lv_anim_init(&anim_exit);
  lv_anim_path_t path;
  lv_anim_path_init(&path);
  /*计算需要的Y偏移量*/
  lv_coord_t y_offset = arg>0?128:-128;
  /*滑动动画*/

  lv_anim_path_set_cb(&path, lv_anim_path_ease_out);
  lv_anim_set_ready_cb(&anim_exit, About_exit_cb);
  lv_anim_set_exec_cb(&anim_exit,(lv_anim_exec_xcb_t) lv_obj_set_y);
  lv_anim_set_var(&anim_exit,appWindow);
  lv_anim_set_time(&anim_exit,300);
  lv_anim_set_values(&anim_exit,lv_obj_get_y(appWindow),lv_obj_get_y(appWindow)-y_offset);
  lv_anim_set_path(&anim_exit,&path);

  lv_anim_start(&anim_exit);
   
}

/**
  * @brief  页面事件
  * @param  btn:发出事件的按键
  * @param  event:事件编号
  * @retval 无
  */
static void Event(void* btn, int event)
{
    /*当有按键点击或长按时*/
    if((event == ButtonEvent::EVENT_ButtonClick || event == ButtonEvent::EVENT_ButtonLongPressed)&&(btn!=NULL))
    {
        static uint8_t t=0;
        if(t)
		{
			HAL_UART_MspDeInit(&huart2);
			lv_led_on(espFlash);
			t=0;
		}	
        else
		{
			HAL_UART_MspInit(&huart2);
			lv_led_off(espFlash);
			t=1;
		}
			
    }
    if((event>3)&&(btn==NULL))
        page.PageChangeTo(ThisPage<=PAGE_NONE+1?ThisPage:ThisPage-1);
    else if((event<-3)&&(btn==NULL))
        page.PageChangeTo(ThisPage>=PAGE_MAX-1?ThisPage:ThisPage+1);
}
/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_About(uint8_t pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    ThisPage = pageID;
    /*注册至页面调度器*/
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
    
}