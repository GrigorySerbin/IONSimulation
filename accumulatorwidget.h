#ifndef ACCUMULATORWIDGET_H
#define ACCUMULATORWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>

#include "accumulator.h"

class AccumulatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccumulatorWidget(int accumNumber, Accumulator *accumSettings, QWidget *parent = 0);
    ~AccumulatorWidget();
signals:

public slots:
    void rand_volt_slot(int);
    void rand_temp_slot(int);
    void norm_volt_slot(int);
    void norm_temp_slot(int);
    void power_slot(int);
    void ballance_slot(int);

    void voltage_changed_slot();
    void temperature_changed_slot();
    void number_changed_slot();
    void year_changed_slot();
    void quarter_changed_slot();

private:
    QLabel *voltLabel;
    QLabel *tempLabel;
    QLabel *numberLabel;
    QLabel *yearLabel;
    QLabel *quarterLabel;

    QCheckBox *randVolt;
    QCheckBox *normVolt;
    QCheckBox *randTemp;
    QCheckBox *normTemp;
    QCheckBox *ballance;
    QCheckBox *power;

    QDoubleSpinBox *voltage;
    QSpinBox *temperature;
    QSpinBox *number;
    QSpinBox *year;
    QSpinBox *quarter;
    QGroupBox *groupBox;

    QGridLayout *gridLayout;
    QHBoxLayout *mainLayout;

    Accumulator *accumSettings;

};

#endif // ACCUMULATORWIDGET_H
