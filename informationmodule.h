#ifndef INFORMATIONMODULE_H
#define INFORMATIONMODULE_H


class InformationModule
{
public:
    InformationModule();

    void set_number(short);
    void set_month(short);
    void set_year(short);
    void set_voltage(float);
    void set_temperature(short);

    short get_number();
    short get_month();
    short get_year();
    float get_voltage();
    short get_temperature();

private:
    short number;
    short month;
    short year;
    float voltage;
    short temperature;
};

#endif // INFORMATIONMODULE_H
