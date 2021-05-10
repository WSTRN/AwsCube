#include "DisplayBasic.hpp"
#include "CommonMacro.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "gpio.h"
#include "SEGGER_RTT.h"
#include "ButtonEvent.h"
#include <math.h>

/*RTC时间*/
RTC_TimeTypeDef RTC_Time;
RTC_TimeTypeDef RTC_TimeLast;
static RTC_DateTypeDef RTC_Date;
extern RTC_HandleTypeDef hrtc;
/*此页面窗口*/
static lv_obj_t * appWindow;
static uint8_t ThisPage;
/*背景图片*/
static lv_obj_t * MainClockBG;

/*气压计信息*/
//static lv_obj_t * labelBMP;

/*电池信息*/
//static lv_obj_t * labelBatt;

/*时间信息*/
static lv_obj_t * labelDate;

/*时间容器*/
static lv_obj_t * contTime;

/*秒指示灯LED*/
static lv_obj_t * ledSec[2];

/*时间标签1、2，交替使用实现上下滑动效果*/
static lv_obj_t * labelTime_Grp[4];
static lv_obj_t * labelTime_Grp2[4];

/*运动图标*/
//static lv_obj_t * imgRun;

/*计步次数标签*/
//static lv_obj_t * labelStepCnt;

/*时间更新任务句柄*/
static lv_task_t * taskTimeUpdate;

/*状态栏更新任务句柄*/
static lv_task_t * taskTopBarUpdate;
/**
  * @brief  创建背景图
  * @param  无
  * @retval 无
  */
static void ImgBg_Create()
{
    LV_IMG_DECLARE(ImgMainClockBG);
    MainClockBG = lv_img_create(appWindow, NULL);
    lv_img_set_src(MainClockBG, &ImgMainClockBG);
    lv_obj_set_pos(MainClockBG,0,0);
    //lv_obj_align(MainClockBG, NULL, LV_ALIGN_CENTER, 0, 0);
}

// /**
//   * @brief  状态栏更新
//   * @param  task:任务句柄
//   * @retval 无
//   */
// static void Task_TopBarUpdate(lv_task_t * task)
// {
//     /*气压计状态更新*/
//     BMP_Update();
//     lv_label_set_text_fmt(labelBMP, "% 2dC"LV_SYMBOL_DEGREE_SIGN" %dm", (int)BMP180.temperature, (int)BMP180.altitude);
    
//     /*读取电池电压*/
//     float battVoltage = (float)analogRead_DMA(BAT_DET_Pin) / 4095.0f * 3.3f * 2;
    
//     /*是否充电*/
//     bool Is_BattCharging = !digitalRead(BAT_CHG_Pin);

//     /*电池图标组*/
//     const char * battSymbol[] =
//     {
//         LV_SYMBOL_BATTERY_EMPTY,
//         LV_SYMBOL_BATTERY_1,
//         LV_SYMBOL_BATTERY_2,
//         LV_SYMBOL_BATTERY_3,
//         LV_SYMBOL_BATTERY_FULL
//     };
    
//     /*电压映射到图标索引*/
//     int symIndex = fmap(
//         battVoltage, 
//         2.8f, 4.2f, 
//         0, __Sizeof(battSymbol)
//     );
//     __LimitValue(symIndex, 0, __Sizeof(battSymbol) - 1);
    
//     if(Is_BattCharging)
//     {
//         /*充电动画效果*/
//         static uint8_t usage = 0;
//         usage++;
//         usage %= (symIndex + 1);
//         symIndex = usage;
//     }

//     lv_label_set_text_fmt(labelBatt, "#FFFFFF %s#", battSymbol[symIndex]);
// }

// /**
//   * @brief  创建状态栏
//   * @param  无
//   * @retval 无
//   */
// static void LabelTopBar_Create()
// {
//     LV_FONT_DECLARE(HandGotn_14);
//     labelBMP = lv_label_create(appWindow, NULL);
//     static lv_style_t bmp_style;
//     bmp_style = *lv_label_get_style(labelBMP, LV_LABEL_STYLE_MAIN);
//     bmp_style.text.font = &HandGotn_14;
//     bmp_style.text.color = LV_COLOR_WHITE;
//     lv_label_set_style(labelBMP, LV_LABEL_STYLE_MAIN, &bmp_style);
//     lv_label_set_text(labelBMP, "-- C -- kPa");
//     lv_obj_align(labelBMP, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
//     lv_obj_set_auto_realign(labelBMP, true);
    
//     labelBatt = lv_label_create(appWindow, NULL);
//     lv_label_set_recolor(labelBatt, true);
//     lv_label_set_text(labelBatt, "#FFFFFF "LV_SYMBOL_BATTERY_EMPTY"#");
//     lv_obj_align(labelBatt, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
//     lv_obj_set_auto_realign(labelBatt, true);
    
//     taskTopBarUpdate = lv_task_create(Task_TopBarUpdate, 2 * 1000, LV_TASK_PRIO_LOW, NULL);
//     Task_TopBarUpdate(taskTopBarUpdate);
// }


void Label_Slide_Change(uint8_t nowval,uint8_t lastval,int label_index)
{
  if(nowval != lastval)
  {
    static lv_anim_t anim_now;
    lv_anim_init(&anim_now);
    static lv_anim_t anim_next;
    lv_anim_init(&anim_next);

    lv_anim_path_t path;
    lv_anim_path_init(&path);

    lv_obj_t * next_label;
    lv_obj_t * now_label;
    /*判断两个标签的相对位置，确定谁是下一个标签*/
    if(lv_obj_get_y(labelTime_Grp2[label_index]) > lv_obj_get_y(labelTime_Grp[label_index]))
    {
        now_label = labelTime_Grp2[label_index];
        next_label = labelTime_Grp[label_index];
    }
    else
    {
        now_label = labelTime_Grp[label_index];
        next_label = labelTime_Grp2[label_index];
    }
    
    lv_label_set_text_fmt(now_label, "%d", lastval);
    lv_label_set_text_fmt(next_label, "%d", nowval);
    
    //lv_obj_align(next_label, now_label, LV_ALIGN_OUT_TOP_MID, 0, -10);
    /*计算需要的Y偏移量*/
    lv_coord_t y_offset = abs(lv_obj_get_y(now_label) - lv_obj_get_y(next_label));
    /*滑动动画*/
    lv_obj_align(next_label, now_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_anim_path_set_cb(&path, lv_anim_path_bounce);

    lv_anim_set_exec_cb(&anim_now,(lv_anim_exec_xcb_t) lv_obj_set_y);
    lv_anim_set_var(&anim_now,now_label);
    lv_anim_set_time(&anim_now,300);
    lv_anim_set_values(&anim_now,lv_obj_get_y(now_label),lv_obj_get_y(now_label)-y_offset);
    lv_anim_set_path(&anim_now,&path);

    lv_anim_set_exec_cb(&anim_next,(lv_anim_exec_xcb_t) lv_obj_set_y);
    lv_anim_set_var(&anim_next,next_label);
    lv_anim_set_time(&anim_next,300);
    lv_anim_set_values(&anim_next,lv_obj_get_y(next_label),lv_obj_get_y(next_label)-y_offset);
    lv_anim_set_path(&anim_next,&path);

    lv_anim_start(&anim_now);
    lv_anim_start(&anim_next);
  }
}


/**
  * @brief  时间标签更新
  * @param  无
  * @retval 无
  */
static void LabelTimeGrp_Update()
{
    /*获取RTC时间*/
    // RTC_GetTime(RTC_Format_BIN, &RTC_Time);
//    RTC_Time.RTC_Hours = (millis() / (3600 * 1000)) % 100;
//    RTC_Time.RTC_Minutes = (millis() / (60 * 1000)) % 60;
//    RTC_Time.RTC_Seconds = (millis() / 1000) % 60;
    HAL_RTC_GetTime(&hrtc, &RTC_Time, RTC_FORMAT_BIN);
    //SEGGER_RTT_printf(0,"%d \r\n",RTC_Time.Seconds);


   /*秒-个位*/
  //  LABEL_TIME_CHECK_DEF(RTC_Time.Seconds % 10,RTC_TimeLast.Seconds % 10, 3);
  //  /*秒-十位*/
  //  LABEL_TIME_CHECK_DEF(RTC_Time.Seconds / 10,RTC_TimeLast.Seconds / 10, 2);
    // lv_label_set_text_fmt(labelTime_Grp[3], "%1d", RTC_Time.Seconds % 10);
    // lv_label_set_text_fmt(labelTime_Grp[2], "%1d", RTC_Time.Seconds / 10);
    Label_Slide_Change(RTC_Time.Seconds%10,RTC_TimeLast.Seconds%10,3);
    Label_Slide_Change(RTC_Time.Seconds/10,RTC_TimeLast.Seconds/10,2);
    if(RTC_Time.Seconds%10!=RTC_TimeLast.Seconds%10)
    {
        /*翻转LED状态*/
      lv_led_toggle(ledSec[0]);
      lv_led_toggle(ledSec[1]);
    }
    // /*分-个位*/
    // LABEL_TIME_CHECK_DEF(RTC_Time.RTC_Minutes % 10,RTC_TimeLast.RTC_Minutes % 10, 3);
    // /*分-十位*/
    // LABEL_TIME_CHECK_DEF(RTC_Time.RTC_Minutes / 10,RTC_TimeLast.RTC_Minutes / 10, 2);
    
    // /*时-个位*/
    // LABEL_TIME_CHECK_DEF(RTC_Time.Hours % 10,RTC_TimeLast.Hours % 10, 1);
    // /*时-十位*/
    // LABEL_TIME_CHECK_DEF(RTC_Time.Hours / 10,RTC_TimeLast.Hours / 10, 0);
    // lv_label_set_text_fmt(labelTime_Grp[1], "%d", RTC_Time.Minutes % 10);
    // lv_label_set_text_fmt(labelTime_Grp[0], "%d", RTC_Time.Minutes / 10);
    
    Label_Slide_Change(RTC_Time.Minutes%10,RTC_TimeLast.Minutes%10,1);
    Label_Slide_Change(RTC_Time.Minutes/10,RTC_TimeLast.Minutes/10,0);
    RTC_TimeLast = RTC_Time;
}

/**
  * @brief  时间更新任务
  * @param  task:任务句柄
  * @retval 无
  */
void TimeUpdate(int arg)//(lv_task_t * task)
{
    /*时间标签状态更新*/
    LabelTimeGrp_Update();
    
    /*日期*/
    HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN);
    const char* week_str[7] = { "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    int8_t index = RTC_Date.WeekDay - 1;
    __LimitValue(index, 0, 6);
    lv_label_set_text_fmt(labelDate, "%02d#0000FF /#%02d %s", RTC_Date.Month, RTC_Date.Date, week_str[index]);
}

/**
  * @brief  创建日期标签
  * @param  无
  * @retval 无
  */
static void LabelDate_Create()
{
    LV_FONT_DECLARE(Morganite_36);
    labelDate = lv_label_create(MainClockBG, NULL);
    
    static lv_style_t labelDate_style;
    // bmp_style = *lv_label_get_style(labelDate, LV_LABEL_STYLE_MAIN);
    // bmp_style.text.font = &Morganite_36;
    // bmp_style.text.color = LV_COLOR_WHITE;
    //lv_label_set_style(labelDate, LV_LABEL_STYLE_MAIN, &bmp_style);
    //lv_style_set_text_color(&bmp_style,LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&labelDate_style,LV_STATE_DEFAULT,&Morganite_36);
    lv_obj_add_style(labelDate,LV_LABEL_PART_MAIN,&labelDate_style);
    lv_label_set_recolor(labelDate, true);
    //lv_label_set_text(labelDate, "01#FF0000 /#01 MON");
    HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN);
    const char* week_str[7] = { "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    int8_t index = RTC_Date.WeekDay - 1;
    __LimitValue(index, 0, 6);
    lv_label_set_text_fmt(labelDate, "%02d#0000FF /#%02d %s", RTC_Date.Month, RTC_Date.Date, week_str[index]);
    lv_obj_align(labelDate, NULL, LV_ALIGN_IN_TOP_MID, 0, 8);
    //lv_obj_set_pos(labelDate,0,0);
    lv_obj_set_auto_realign(labelDate, true);

    
}
// /**
//   * @brief  创建时间标签
//   * @param  无
//   * @retval 无
//   */
static void LabelTime_Create()
{
    LabelDate_Create();
    
    // /*contTime*/
    contTime = lv_cont_create(MainClockBG, NULL);
    // //lv_cont_set_style(contTime, LV_CONT_STYLE_MAIN, &lv_style_transp);
    static lv_style_t time_cont_style;
    //lv_style_set_bg_color(&time_cont_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&time_cont_style, LV_STATE_DEFAULT,LV_OPA_TRANSP);
    //lv_style_set_border_color(&time_cont_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_border_opa(&time_cont_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_add_style(contTime,LV_CONT_PART_MAIN,&time_cont_style);
    lv_obj_set_size(contTime, 128, 80);
    lv_obj_align(contTime, NULL, LV_ALIGN_CENTER, 0, 12);
    
    // /*ledSec*/
    static lv_style_t led_style;
    // // led_style = lv_style_pretty_color;
    // // led_style.body.main_color = LV_COLOR_RED;
    // // led_style.body.grad_color = LV_COLOR_RED;
    lv_style_init(&led_style);
    lv_style_set_bg_color(&led_style,LV_STATE_DEFAULT,LV_COLOR_RED);
    lv_style_set_border_color(&led_style,LV_STATE_DEFAULT,LV_COLOR_RED);
    
    for(int i = 0; i < __Sizeof(ledSec); i++)
    {
        lv_obj_t * led = lv_led_create(contTime, NULL);

        lv_obj_add_style(led,LV_LED_PART_MAIN,&led_style);
        //lv_led_set_style(led, LV_LED_STYLE_MAIN, &led_style);
        lv_obj_set_size(led, 4, 6);
        lv_led_set_bright(led,190);
        //lv_obj_set_pos(led,60,46+(i == 0 ? -10 : 10));
        lv_obj_align(led, NULL, LV_ALIGN_CENTER, 0, i == 0 ? -10 : 10);
        lv_led_on(led);
        ledSec[i] = led;
    }
    
    // /*labelTime*/
    LV_FONT_DECLARE(Morganite_100);
    static lv_style_t time_style;
    // // time_style = lv_style_plain;
    // // time_style.text.font = &Morganite_100;
    // // time_style.text.color = LV_COLOR_WHITE;
    lv_style_set_text_color(&time_style,LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&time_style,LV_STATE_DEFAULT,&Morganite_100);
    const lv_coord_t x_mod[4] = {-42, -16, 16, 42};
    for(int i = 0; i < __Sizeof(labelTime_Grp); i++)
    {
        lv_obj_t * label = lv_label_create(contTime, NULL);
        lv_obj_add_style(label, LV_LABEL_PART_MAIN, &time_style);
        lv_label_set_text(label, "0");
        //lv_obj_set_pos(label,50+x_mod[i],6);
        lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, x_mod[i], 6);
        labelTime_Grp[i] = label;
    }
    
    for(int i = 0; i < __Sizeof(labelTime_Grp2); i++)
    {
        lv_obj_t * label = lv_label_create(contTime, NULL);
        lv_obj_add_style(label, LV_LABEL_PART_MAIN, &time_style);
        lv_label_set_text(label, "0");
        lv_obj_align(label, labelTime_Grp[i], LV_ALIGN_OUT_TOP_MID, 0, -10);
        labelTime_Grp2[i] = label;
    }
    HAL_RTC_GetTime(&hrtc, &RTC_Time, RTC_FORMAT_BIN);
    lv_label_set_text_fmt(labelTime_Grp[0], "%d", RTC_Time.Minutes/10);
    lv_label_set_text_fmt(labelTime_Grp[1], "%d", RTC_Time.Minutes%10);
    lv_label_set_text_fmt(labelTime_Grp[2], "%d", RTC_Time.Seconds/10);
    lv_label_set_text_fmt(labelTime_Grp[3], "%d", RTC_Time.Seconds%10);
    RTC_TimeLast = RTC_Time;
    /*时间清零*/
    //memset(&RTC_TimeLast, 0, sizeof(RTC_TimeLast));
    
    /*注册时间标签更新任务，执行周期500ms*/
    //taskTimeUpdate = lv_task_create(Task_TimeUpdate, 500, LV_TASK_PRIO_MID, NULL);
    //Task_TimeUpdate(taskTimeUpdate);
}

// /**
//   * @brief  创建计步标签
//   * @param  无
//   * @retval 无
//   */
// static void LabelStep_Create()
// {
//     LV_IMG_DECLARE(ImgRun);
//     imgRun = lv_img_create(appWindow, NULL);
//     lv_img_set_src(imgRun, &ImgRun);
//     lv_obj_align(imgRun, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -20);
    
//     LV_FONT_DECLARE(HandGotn_26);
    
//     labelStepCnt = lv_label_create(appWindow, NULL);
    
//     static lv_style_t step_style;
//     step_style = *lv_label_get_style(labelStepCnt, LV_LABEL_STYLE_MAIN);
//     step_style.text.font = &HandGotn_26;
//     step_style.text.color = LV_COLOR_WHITE;
//     lv_label_set_style(labelStepCnt, LV_LABEL_STYLE_MAIN, &step_style);
    
//     lv_label_set_recolor(labelStepCnt, true);
//     /*懒得做计步，先随便写个值*/
//     lv_label_set_text(labelStepCnt, "#FF0000 /# 1255");
//     lv_obj_align(labelStepCnt, imgRun, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
//     lv_obj_set_auto_realign(labelStepCnt, true);
// }

// /**
//   * @brief  页面初始化事件
//   * @param  无
//   * @retval 无
//   */
static void Setup(int arg)
{
    /*将此页面移到前台*/
    lv_obj_set_pos(appWindow,0,arg>0?128:-128);
    lv_obj_move_foreground(appWindow);
    ImgBg_Create();
    // LabelTopBar_Create();
    LabelTime_Create();
    // LabelStep_Create();


    //HAL_GPIO_WritePin(ExtPower_GPIO_Port, ExtPower_Pin, GPIO_PIN_SET);
    //Power_SetAutoLowPowerEnable(true);

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
void MainClock_exit_cb(lv_anim_t* a)
{
  
    /*删除此页面上的子控件*/
    lv_obj_clean(appWindow);
    memset((void*)&RTC_Time,0,sizeof(RTC_Time));
    memset((void*)&RTC_TimeLast,0,sizeof(RTC_TimeLast));
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
    lv_anim_set_ready_cb(&anim_exit, MainClock_exit_cb);
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
        /*进入主菜单*/
        page.PagePush(PAGE_Weather);
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
void PageRegister_MainClock(uint8_t pageID)
{
    /*获取分配给此页面的窗口*/
    appWindow = AppWindow_GetCont(pageID);
    ThisPage = pageID;
    /*注册至页面调度器*/
    page.PageRegister(pageID, Setup, TimeUpdate, Exit, Event);
    
}
