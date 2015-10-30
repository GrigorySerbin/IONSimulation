#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <QDebug>
#include <cstdlib>


class Accumulator
{
public:
    Accumulator();

    void set_on(bool);
    void set_rand_volt(bool);
    void set_normal_volt(bool);
    void set_volt(float);
    void set_rand_temp(bool);
    void set_normal_temp(bool);
    void set_temp(short);
    void set_number(short);
    void set_year(short);
    void set_quarter(short);
    void set_ballance(bool);

    bool turned_on();
    bool get_rand_volt();
    bool get_normal_volt();
    bool get_rand_temp();
    bool get_normal_temp();
    bool get_ballance();
    float get_volt();
    short get_temp();
    short get_number();
    short get_year();
    short get_quarter();

private:
    bool is_on;
    bool rand_volt;
    bool normal_volt;
    bool rand_temp;
    bool normal_temp;
    bool ballance;

    float voltage;
    short temperature;
    short number;
    short year;
    short quarter;
};

#endif // ACCUMULATOR_H
