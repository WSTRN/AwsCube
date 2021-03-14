#include "DisplayBasic.hpp"
#include "CommonMacro.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "SEGGER_RTT.h"
#include <math.h> 

static lv_obj_t * appWindow;
static lv_obj_t * WeatherBG;

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);

    LV_IMG_DECLARE(ImgWatchX);
    WeatherBG = lv_img_create(appWindow, NULL);
    lv_img_set_src(WeatherBG, &ImgWatchX);
    lv_obj_set_pos(WeatherBG,0,0);
    
    // LabelTopBar_Create();
    //LabelTime_Create();
    // LabelStep_Create();

    //Power_SetAutoLowPowerEnable(true);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
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
  * @brief  页面事件
  * @param  btn:发出事件的按键
  * @param  event:事件编号
  * @retval 无
  */
static void Event(void* btn, int event)
{
    /*当有按键点击或长按时*/
    // if(event == ButtonEvent::EVENT_ButtonClick || event == ButtonEvent::EVENT_ButtonLongPressed)
    // {
    //     /*进入主菜单*/
    //     page.PagePush(PAGE_MainClock);
    // }
}
/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Weather(uint8_t pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
 
    /*注册至页面调度器*/
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
    
}