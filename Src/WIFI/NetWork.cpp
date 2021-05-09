#include "NetWork.h"


NetWork::NetWork()
{
    WeatherUDTime={0xff,0xff,0xff,0xff,0xff,0xff};
}
void NetWork::setNTPData(TimeType *ttemp)
{
    NTP=*ttemp;
}
void NetWork::setWeatherData(uint8_t indx, WeatherDataType *wtemp, TimeType *ttemp)
{
    WeatherData[indx]=*wtemp;
    WeatherUDTime=*ttemp;
}
bool NetWork::getWeather(uint8_t indx, WeatherDataType *wp)
{
    if(WeatherUDTime.sec!=0xff)
    {
        *wp=WeatherData[indx];
        return true;
    }
    else 
        return false;
}
bool NetWork::getNTP(TimeType *tp)
{
    if(NTP.sec!=0xff)
    {
        *tp=NTP;
        return true;
    }
    else
        return false;
}