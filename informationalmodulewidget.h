#ifndef INFORMATIONALMODULEWIDGET_H
#define INFORMATIONALMODULEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QGridLayout>

#include "informationmodule.h"

class InformationalModuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InformationalModuleWidget(InformationModule *infmod, QWidget *parent = 0);
    ~InformationalModuleWidget();
signals:

public slots:
    void number_changed_slot();
    void year_changed_slot();
    void month_changed_slot();
    void voltage_changed_slot();
    void temperature_changed_slot();

private:
    QLabel *numberLabel;
    QLabel *yearLabel;
    QLabel *monthLabel;
    QLabel *voltageLabel;
    QLabel *temperatureLabel;

    QDoubleSpinBox *voltage;
    QSpinBox *temperature;
    QSpinBox *number;
    QSpinBox *year;
    QSpinBox *month;
    QGroupBox *groupBox;

    QGridLayout *gridLayout;
    QHBoxLayout *mainLayout;

    InformationModule *informationModuleSettings;
};

#endif // INFORMATIONALMODULEWIDGET_H
