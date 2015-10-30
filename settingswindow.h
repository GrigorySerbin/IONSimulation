#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QScrollArea>
#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QObject>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCloseEvent>

#include "accumulator.h"
#include "accumulatorwidget.h"
#include "batterymodule.h"
#include "batterymodulewidget.h"
#include "controlmodule.h"
#include "controlmodulewidget.h"
#include "informationmodule.h"
#include "informationalmodulewidget.h"


class SettingsWindow : public QScrollArea
{
    Q_OBJECT

public:
    SettingsWindow(Accumulator*, BatteryModule*,
                   ControlModule*, InformationModule*,
                   QWidget *parent = 0);
    ~SettingsWindow();
signals:
    void windowClosed();
private:
    QGridLayout *scrollAreaLayout;
    QWidget *scrollAreaWidget;

    BatteryModule *bm;
    Accumulator *accum;
    ControlModule *MYKmodule;
    InformationModule *MIOKmodule;

    AccumulatorWidget *accumWidgets[78];
    BatteryModuleWidget *batteryWidgets[3];
    ControlModuleWidget *controlModuleWidget;
    InformationalModuleWidget *informationModuleWidget;

    void closeEvent(QCloseEvent*);
};

#endif // SETTINGSWINDOW_H
