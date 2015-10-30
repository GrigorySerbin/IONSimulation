#include "settingswindow.h"

SettingsWindow::SettingsWindow(Accumulator* paccum, BatteryModule* pbm,
                               ControlModule* pcntlModule, InformationModule* pinfModule,
                               QWidget *parent):
    QScrollArea(parent)
{
    bm = pbm;
    accum = paccum;
    MYKmodule = pcntlModule;
    MIOKmodule = pinfModule;

    for(int i = 0; i < 78; i++)
        accumWidgets[i] = new AccumulatorWidget(i, accum + i, this);

    for(int i = 0; i < 3; i++)
        batteryWidgets[i] = new BatteryModuleWidget(i+1, bm + i, this);

    controlModuleWidget = new ControlModuleWidget(MYKmodule, this);

    informationModuleWidget = new InformationalModuleWidget(MIOKmodule, this);

    scrollAreaLayout = new QGridLayout(this);

    scrollAreaLayout->addWidget(informationModuleWidget, 0, 0, 1, 2);
    scrollAreaLayout->addWidget(controlModuleWidget, 0, 2);

    for(int i = 0; i < 3; i++)
        scrollAreaLayout->addWidget(batteryWidgets[i], 1, i);

    for(int i = 0; i < 78; i++)
        scrollAreaLayout->addWidget(accumWidgets[i], i+2, 0, 1, 3);

    scrollAreaWidget = new QWidget(this);
    scrollAreaWidget->setLayout(scrollAreaLayout);
    this->setWidget(scrollAreaWidget);
    this->setWindowTitle("Параметры");

}

SettingsWindow::~SettingsWindow()
{
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    event->accept();
}
