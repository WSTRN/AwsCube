#pragma once 


#include <stdbool.h>
#include "stm32f4xx_hal.h"


struct TimeType
{
    //DCB encode
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
};
struct WeatherDataType
{
    uint8_t code_day;
    uint8_t code_night;
    uint8_t high;
    uint8_t low;
    uint8_t rainfall;
    uint8_t wind_scale;
    uint8_t humidity;
};
class NetWork
{
public:
    NetWork();
    void setNTPData(TimeType *ttemp);
    void setWeatherData(uint8_t indx,WeatherDataType *wtemp,TimeType *ttemp);

    uint8_t getWeatherCode(uint8_t indx, bool DorN);
    uint8_t getWeatherTemp(uint8_t indx, bool HorL);
    uint8_t getWeatherHumi(uint8_t indx);
    
    bool getWeather(uint8_t indx, WeatherDataType *wp);
    bool getNTP(TimeType *tp);
    bool getWeatherUDTime(TimeType *tp);


    TimeType NTP;
    WeatherDataType WeatherData[3];
    TimeType WeatherUDTime;
};