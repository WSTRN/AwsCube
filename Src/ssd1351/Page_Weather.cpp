#include "DisplayBasic.hpp"
#include "CommonMacro.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "gpio.h"
#include "SEGGER_RTT.h"
#include "ButtonEvent.h"
#include <math.h> 
#include "usart.h"
#include "WIFI.h"

static lv_obj_t* appWindow;
static uint8_t ThisPage;
static uint8_t day;
static lv_obj_t* WeatherBG;
static lv_obj_t* WeatherCode;
static lv_obj_t* TempretureHigh;
static lv_obj_t* TempretureLow;
static lv_obj_t* Week;
static lv_obj_t* Humidity;
static lv_obj_t* labelDate;
extern RTC_HandleTypeDef hrtc;
static RTC_TimeTypeDef RTC_Time;
static RTC_DateTypeDef RTC_Date;

LV_IMG_DECLARE(ImgWeatherCode_0)
LV_IMG_DECLARE(ImgWeatherCode_1)
LV_IMG_DECLARE(ImgWeatherCode_4)
LV_IMG_DECLARE(ImgWeatherCode_5)
LV_IMG_DECLARE(ImgWeatherCode_6)
LV_IMG_DECLARE(ImgWeatherCode_9)
LV_IMG_DECLARE(ImgWeatherCode_10)
LV_IMG_DECLARE(ImgWeatherCode_11)
LV_IMG_DECLARE(ImgWeatherCode_12)
LV_IMG_DECLARE(ImgWeatherCode_13)
LV_IMG_DECLARE(ImgWeatherCode_14)
LV_IMG_DECLARE(ImgWeatherCode_15)
LV_IMG_DECLARE(ImgWeatherCode_16)
LV_IMG_DECLARE(ImgWeatherCode_17)
LV_IMG_DECLARE(ImgWeatherCode_19)
LV_IMG_DECLARE(ImgWeatherCode_20)
LV_IMG_DECLARE(ImgWeatherCode_21)
LV_IMG_DECLARE(ImgWeatherCode_22)
LV_IMG_DECLARE(ImgWeatherCode_23)
LV_IMG_DECLARE(ImgWeatherCode_24)
LV_IMG_DECLARE(ImgWeatherCode_25)
LV_IMG_DECLARE(ImgWeatherCode_26)
LV_IMG_DECLARE(ImgWeatherCode_28)
LV_IMG_DECLARE(ImgWeatherCode_30)
LV_IMG_DECLARE(ImgWeatherCode_31)
LV_IMG_DECLARE(ImgWeatherCode_32)
LV_IMG_DECLARE(ImgWeatherCode_34)
LV_IMG_DECLARE(ImgWeatherCode_36)
LV_IMG_DECLARE(ImgWeatherCode_37)
LV_IMG_DECLARE(ImgWeatherCode_99)
const lv_img_dsc_t* WeatherImg[]={  &ImgWeatherCode_0,&ImgWeatherCode_1,&ImgWeatherCode_0,&ImgWeatherCode_1,&ImgWeatherCode_4,
									&ImgWeatherCode_5,&ImgWeatherCode_6,&ImgWeatherCode_5,&ImgWeatherCode_6,&ImgWeatherCode_9,
									&ImgWeatherCode_10,&ImgWeatherCode_11,&ImgWeatherCode_12,&ImgWeatherCode_13,&ImgWeatherCode_14,
									&ImgWeatherCode_15,&ImgWeatherCode_16,&ImgWeatherCode_17,&ImgWeatherCode_17,&ImgWeatherCode_19,
									&ImgWeatherCode_20,&ImgWeatherCode_21,&ImgWeatherCode_22,&ImgWeatherCode_23,&ImgWeatherCode_24,
									//&ImgWeatherCode_25,&ImgWeatherCode_26,&ImgWeatherCode_26,&ImgWeatherCode_28,&ImgWeatherCode_28,
									// &ImgWeatherCode_30,&ImgWeatherCode_31,&ImgWeatherCode_32,&ImgWeatherCode_32,&ImgWeatherCode_34,
									// &ImgWeatherCode_34,&ImgWeatherCode_36,&ImgWeatherCode_37,&ImgWeatherCode_0,&ImgWeatherCode_99
									};

void anim_weather_ready_cb(lv_anim_t* a)
{
  lv_coord_t now = lv_obj_get_y(WeatherCode);
  lv_anim_set_values(a,now,now+(now>40-6-3?-6:6));
  lv_anim_set_delay(a,0);//clean a->act_time
  lv_anim_start(a);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup(int arg)
{
    /*将此页面移到前台*/
    day=0;
    lv_obj_set_pos(appWindow,0,arg>0?128:-128);
    lv_obj_move_foreground(appWindow);
    //background
    LV_IMG_DECLARE(ImgWeatherBG);
    WeatherBG = lv_img_create(appWindow, NULL);
    lv_img_set_src(WeatherBG, &ImgWeatherBG);
    lv_obj_align(WeatherBG, appWindow, LV_ALIGN_CENTER, 0, -4);

    //weathercode
    WeatherCode = lv_img_create(appWindow, NULL);
    lv_obj_align(WeatherCode, WeatherBG, LV_ALIGN_CENTER, -42, 44-6-64);
    lv_img_set_src(WeatherCode, WeatherImg[esp8266.WeatherData[day].code_day]);

	//weathercode animation
    lv_anim_t anim_weathercode;
    lv_anim_init(&anim_weathercode);
    lv_anim_path_t path_weathercode;
    lv_anim_path_init(&path_weathercode);
    lv_anim_set_ready_cb(&anim_weathercode, anim_weather_ready_cb);
    lv_anim_path_set_cb(&path_weathercode, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&anim_weathercode,(lv_anim_exec_xcb_t) lv_obj_set_y);
    lv_anim_set_var(&anim_weathercode,WeatherCode);
    lv_anim_set_time(&anim_weathercode,900);
    lv_anim_set_values(&anim_weathercode,lv_obj_get_y(WeatherCode),lv_obj_get_y(WeatherCode)+6);
    lv_anim_set_path(&anim_weathercode,&path_weathercode);
    lv_anim_start(&anim_weathercode);


    //weatherdata
    LV_FONT_DECLARE(lv_font_montserrat_36);
    TempretureHigh = lv_label_create(appWindow, NULL);
    static lv_style_t Tempreture_style;
    lv_style_set_text_font(&Tempreture_style,LV_STATE_DEFAULT,&lv_font_montserrat_36);
    lv_obj_add_style(TempretureHigh,LV_LABEL_PART_MAIN,&Tempreture_style);
    lv_label_set_text_fmt(TempretureHigh,"%d",esp8266.WeatherData[day].high);
    TempretureLow = lv_label_create(appWindow, TempretureHigh);
    lv_label_set_text_fmt(TempretureLow,"%d",esp8266.WeatherData[day].low);
    lv_obj_align(TempretureHigh, WeatherBG, LV_ALIGN_CENTER, 34, -22);
    lv_obj_align(TempretureLow, WeatherBG, LV_ALIGN_CENTER, 34, 10);
    lv_obj_set_auto_realign(TempretureHigh, true); 
    lv_obj_set_auto_realign(TempretureLow, true); 

    LV_FONT_DECLARE(Morganite_36);
    Week = lv_label_create(appWindow, NULL);
    static lv_style_t WeekHumidity_style;
    lv_style_set_text_font(&WeekHumidity_style,LV_STATE_DEFAULT,&Morganite_36);
    lv_obj_add_style(Week,LV_LABEL_PART_MAIN,&WeekHumidity_style);
    //RTC
    HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &RTC_Time, RTC_FORMAT_BIN);
    const char* week_str[7] = { "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    int8_t index = RTC_Date.WeekDay - 1;
    __LimitValue(index, 0, 6);

    lv_label_set_text_fmt(Week,"%s",week_str[index]);
    Humidity = lv_label_create(appWindow, Week);
    lv_label_set_text_fmt(Humidity,"%d",esp8266.WeatherData[day].humidity);

    lv_obj_align(Week, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 14, 0);
    lv_obj_align(Humidity, NULL, LV_ALIGN_IN_BOTTOM_MID, 26, 0);
    lv_obj_set_auto_realign(Week, true); 
    lv_obj_set_auto_realign(Humidity, true); 

    LV_FONT_DECLARE(lv_font_montserrat_14);
    labelDate = lv_label_create(appWindow, NULL);
    
    static lv_style_t labelDate_style;
    // bmp_style = *lv_label_get_style(labelDate, LV_LABEL_STYLE_MAIN);
    // bmp_style.text.font = &Morganite_36;
    // bmp_style.text.color = LV_COLOR_WHITE;
    //lv_label_set_style(labelDate, LV_LABEL_STYLE_MAIN, &bmp_style);
    //lv_style_set_text_color(&bmp_style,LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&labelDate_style,LV_STATE_DEFAULT,&lv_font_montserrat_14);
    lv_obj_add_style(labelDate,LV_LABEL_PART_MAIN,&labelDate_style);
    lv_label_set_recolor(labelDate, true);
    //lv_label_set_text(labelDate, "01#FF0000 /#01 MON");
    
    lv_label_set_text_fmt(labelDate, "%02d#0000FF /#%02d %s %02d#0000FF :#%02d", RTC_Date.Month, RTC_Date.Date, week_str[index],RTC_Time.Hours,RTC_Time.Minutes);
    lv_obj_align(labelDate, NULL, LV_ALIGN_IN_TOP_MID, 0, 2);
    lv_obj_set_auto_realign(labelDate, true); 

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
void Weather_exit_cb(lv_anim_t* a)
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
  lv_anim_set_ready_cb(&anim_exit, Weather_exit_cb);
  lv_anim_set_exec_cb(&anim_exit,(lv_anim_exec_xcb_t) lv_obj_set_y);
  lv_anim_set_var(&anim_exit,appWindow);
  lv_anim_set_time(&anim_exit,300);
  lv_anim_set_values(&anim_exit,lv_obj_get_y(appWindow),lv_obj_get_y(appWindow)-y_offset);
  lv_anim_set_path(&anim_exit,&path);

  lv_anim_start(&anim_exit);
   
}


static void Loop(int arg)
{
	HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN);
  HAL_RTC_GetTime(&hrtc, &RTC_Time, RTC_FORMAT_BIN);
  int8_t index = RTC_Date.WeekDay-1;
    __LimitValue(index, 0, 6);
	lv_label_set_text_fmt(labelDate, "%02d#0080FF /#%02d %s %02d#0080FF :#%02d", RTC_Date.Month, RTC_Date.Date, week_str[index],RTC_Time.Hours,RTC_Time.Minutes);
	lv_img_set_src(WeatherCode, WeatherImg[esp8266.WeatherData[day].code_day]);
	lv_label_set_text_fmt(TempretureHigh,"%d",esp8266.WeatherData[day].high);
	lv_label_set_text_fmt(TempretureLow,"%d",esp8266.WeatherData[day].low);
	lv_label_set_text_fmt(Humidity,"%d",esp8266.WeatherData[day].humidity);
	index=(index+day)%7;
	lv_label_set_text_fmt(Week,"%s",week_str[index]);
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
			day++;
      if(day==3)
        day=0;
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
void PageRegister_Weather(uint8_t pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    ThisPage = pageID;
    /*注册至页面调度器*/
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
    
}