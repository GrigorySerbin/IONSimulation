#include "informationmodule.h"

InformationModule::InformationModule()
{
    //Настройки по умолчанию
    number = 0;
    month = 0;
    year = 0;
    voltage = 0.0;
    temperature = 0;
}

void InformationModule::set_number(short num)
{
    number = num;
}

void InformationModule::set_month(short m)
{
    month = m;
}

void InformationModule::set_year(short y)
{
    year = y;
}

void InformationModule::set_voltage(float volt)
{
    voltage = volt;
}

void InformationModule::set_temperature(short temp)
{
    temperature = temp;
}

short InformationModule::get_number()
{
    return number;
}

short InformationModule::get_month()
{
    return month;
}

short InformationModule::get_year()
{
    return year;
}

float InformationModule::get_voltage()
{
    return voltage;
}

short InformationModule::get_temperature()
{
    return temperature;
}

