#include "DisplayBasic.hpp"



typedef struct{
    lv_obj_t * cont;
    lv_style_t style;
}AppWindow_TypeDef;



AppWindow_TypeDef appWindow_Grp[PAGE_MAX];

lv_obj_t * AppWindow_GetCont(uint8_t pageID)
{
    return (pageID < PAGE_MAX) ? appWindow_Grp[pageID].cont : NULL;
}

lv_coord_t AppWindow_GetHeight()
{
    return (lv_obj_get_height(lv_scr_act()));// - BarStatus_GetHeight() - BarNavigation_GetHeight());
}

lv_coord_t AppWindow_GetWidth()
{
    return (lv_obj_get_width(lv_scr_act()));
}

void AppWindow_Create()
{
    for(int i = 0; i < PAGE_MAX; i++)
    {
        lv_style_init(&appWindow_Grp[i].style);
        lv_style_set_bg_color(&appWindow_Grp[i].style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_style_set_border_color(&appWindow_Grp[i].style, LV_STATE_DEFAULT, LV_COLOR_BLACK);

        appWindow_Grp[i].cont = lv_cont_create(lv_scr_act(), NULL);
        //lv_obj_set_auto_realign(appWindow_Grp[i].cont, true);                    /*Auto realign when the size changes*/
        //lv_obj_align_origo(appWindow_Grp[i].cont, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
        //lv_cont_set_fit(appWindow_Grp[i].cont, LV_FIT_TIGHT);
        //lv_cont_set_layout(appWindow_Grp[i].cont, LV_LAYOUT_COLUMN_MID);
        lv_obj_set_size(appWindow_Grp[i].cont, AppWindow_GetWidth(), AppWindow_GetHeight());
        
        lv_obj_add_style(appWindow_Grp[i].cont,LV_OBJ_PART_MAIN,&appWindow_Grp[i].style); 
        
        //lv_obj_align(appWindow_Grp[i].cont, NULL, LV_ALIGN_CENTER, 0, 0);
        //lv_cont_set_fit(appWindow_Grp[i].cont, LV_FIT_NONE);
        
        //appWindow_Grp[i].style = lv_style_plain;
        //appWindow_Grp[i].style.body.main_color = LV_COLOR_BLACK;
        // appWindow_Grp[i].style.body.grad_color = LV_COLOR_BLACK;
        // lv_cont_set_style(appWindow_Grp[i].cont, LV_CONT_STYLE_MAIN, &appWindow_Grp[i].style);
    }
    
}
