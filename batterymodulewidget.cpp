#include "batterymodulewidget.h"

BatteryModuleWidget::BatteryModuleWidget(int bm, BatteryModule *bmset,  QWidget *parent) : QWidget(parent)
{
    //Создание элементов интерфейса
    numberLabel = new QLabel("Номер");
    yearLabel = new QLabel("Год");
    monthLabel = new QLabel("Месяц");
    power = new QCheckBox("вкл.");

    number = new QSpinBox;
    year = new QSpinBox;
    month = new QSpinBox;
    groupBox = new QGroupBox;

    //Создание layouts для виджета
    gridLayout = new QGridLayout;
    mainLayout = new QHBoxLayout;

    //Добавление в layouts элементов
    gridLayout->addWidget(numberLabel, 1,0);
    gridLayout->addWidget(yearLabel, 2,0);
    gridLayout->addWidget(monthLabel, 3,0);

    gridLayout->addWidget(power, 0,1);
    gridLayout->addWidget(number, 1,1);
    gridLayout->addWidget(year, 2,1);
    gridLayout->addWidget(month, 3,1);

    groupBox->setLayout(gridLayout);
    mainLayout->addWidget(groupBox);

    //Передача главного layout виджету
    this->setLayout(mainLayout);

    groupBox->setTitle("BM" + QString::number(bm));

    //Настройка диапазонов значений батарейного модуля
    number->setRange(0, 255);
    year->setRange(0, 2100);
    month->setRange(1, 12);

    batteryModuleSettings = bmset;

    //Присваивание полям виджета значений из настроек
    power->setChecked(batteryModuleSettings->turned_on());
    number->setValue(batteryModuleSettings->get_number());
    year->setValue(batteryModuleSettings->get_year());
    month->setValue(batteryModuleSettings->get_month());

    //Настройка слотов/сигналов
    connect(power, SIGNAL(stateChanged(int)), this, SLOT(power_slot(int)));
    connect(number, SIGNAL(editingFinished()), this, SLOT(number_changed_slot()));
    connect(year, SIGNAL(editingFinished()), this, SLOT(year_changed_slot()));
    connect(month, SIGNAL(editingFinished()), this, SLOT(month_changed_slot()));
}

BatteryModuleWidget::~BatteryModuleWidget()
{
    delete gridLayout;
    delete groupBox;
    delete mainLayout;
}

void BatteryModuleWidget::power_slot(int state)
{
    batteryModuleSettings->set_on(state);
}

void BatteryModuleWidget::number_changed_slot()
{
    batteryModuleSettings->set_number(number->value());
}

void BatteryModuleWidget::year_changed_slot()
{
    batteryModuleSettings->set_year(year->value());
}

void BatteryModuleWidget::month_changed_slot()
{
    batteryModuleSettings->set_month(month->value());
}

