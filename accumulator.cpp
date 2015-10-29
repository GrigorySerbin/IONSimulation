#include "accumulator.h"

Accumulator::Accumulator()
{
    is_on = true;
    rand_volt = true;
    normal_volt = true;
    rand_temp = true;
    normal_temp = true;
    ballance = true;

    voltage = 0.0;
    temperature = 0;
    number = 0;
    year = 0;
    quarter = 0;
}

bool Accumulator::turned_on()
{
    return is_on;
}

bool Accumulator::get_rand_volt()
{
    return rand_volt;
}

bool Accumulator::get_normal_volt()
{
    return normal_volt;
}

bool Accumulator::get_rand_temp()
{
    return rand_temp;
}

bool Accumulator::get_normal_temp()
{
    return normal_temp;
}

bool Accumulator::get_ballance()
{
    return ballance;
}

void Accumulator::set_on(bool state)
{
    is_on = state;
}

void Accumulator::set_rand_volt(bool state)
{
    rand_volt = state;
}

void Accumulator::set_normal_volt(bool state)
{
    normal_volt = state;
}

void Accumulator::set_volt(float volt)
{
    voltage = volt;
}

void Accumulator::set_rand_temp(bool state)
{
    rand_temp = state;
}

void Accumulator::set_normal_temp(bool state)
{
    normal_temp = state;
}

void Accumulator::set_temp(short temp)
{
    temperature = temp;
}

void Accumulator::set_number(short num)
{
    number  = num;
}

void Accumulator::set_year(short y)
{
    year = y;
}

void Accumulator::set_quarter(short q)
{
    quarter = q;
}

void Accumulator::set_ballance(bool state)
{
    ballance = state;
}

float Accumulator::get_volt()
{
    if(rand_volt)  return voltage = rand() % 5;
    return voltage;
}

short Accumulator::get_temp()
{
    if(rand_temp)  return temperature = rand() % 150;
    return temperature;
}

short Accumulator::get_number()
{
    return number;
}

short Accumulator::get_year()
{
    return year;
}

short Accumulator::get_quarter()
{
    return quarter;
}



