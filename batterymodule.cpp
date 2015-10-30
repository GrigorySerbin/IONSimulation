#include "batterymodule.h"

BatteryModule::BatteryModule()
{
    //Настройка значений по умолчанию
    is_on = true;
    number = 0;
    year = 0;
    month = 0;
}

bool BatteryModule::turned_on()
{
    return is_on;
}

void BatteryModule::set_on(bool status)
{
    is_on = status;
}

void BatteryModule::set_number(short num)
{
    number = num;
}

void BatteryModule::set_year(short y)
{
    year = y;
}

void BatteryModule::set_month(short m)
{
    month = m;
}

short BatteryModule::get_number()
{
    return number;
}

short BatteryModule::get_year()
{
    return year;
}

short BatteryModule::get_month()
{
    return month;
}

