#ifndef BATTERYMODULEWIDGET_H
#define BATTERYMODULEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>

#include "batterymodule.h"

class BatteryModuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BatteryModuleWidget(int bm, BatteryModule *bmset, QWidget *parent = 0);
    ~BatteryModuleWidget();
signals:

public slots:
    void power_slot(int);
    void number_changed_slot();
    void year_changed_slot();
    void month_changed_slot();

private:
    QLabel *numberLabel;
    QLabel *yearLabel;
    QLabel *monthLabel;

    QSpinBox *number;
    QSpinBox *year;
    QSpinBox *month;
    QCheckBox *power;
    QGroupBox *groupBox;

    QGridLayout *gridLayout;
    QHBoxLayout *mainLayout;

    BatteryModule *batteryModuleSettings;
};

#endif // BATTERYMODULEWIDGET_H
