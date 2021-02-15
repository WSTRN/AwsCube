#include "DisplayBasic.hpp"
#include "lvgl.h"

static lv_disp_buf_t disp_buf;
static lv_color_t buf1[LV_HOR_RES_MAX * 10];
static lv_color_t buf2[LV_HOR_RES_MAX * 10];
void flush_callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

void lvgl_port_init()
{
    lv_init();
    lv_disp_buf_init(&disp_buf, buf1, buf2, LV_HOR_RES_MAX * 10);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 128;
    disp_drv.ver_res = 128;
    disp_drv.flush_cb = flush_callback;
    disp_drv.buffer = &disp_buf;
    lv_disp_t * disp=NULL;
    disp=lv_disp_drv_register(&disp_drv);
    if(disp==NULL)
        while(1);
    //lv_theme_set_current(lv_theme_night_init(200, NULL));

}

void flush_callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint16_t(* ram)[128]=(uint16_t(*)[128])OLED_GRAM;
    uint16_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            ram[y][x]=color_p->full;//put_px(x, y, *color_p)
            color_p++;
        }
    }
    lv_disp_flush_ready(disp_drv);
}

