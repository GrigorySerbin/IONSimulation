#ifndef BATTERYMODULE_H
#define BATTERYMODULE_H

#include "accumulator.h"


class BatteryModule
{
public:
    BatteryModule();
    bool turned_on();
    void set_on(bool);
    void set_number(short);
    void set_year(short);
    void set_month(short);

    short get_number();
    short get_year();
    short get_month();

private:
    bool is_on;
    short number;
    short year;
    short month;
};


#endif // BATTERYMODULE_H
