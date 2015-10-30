#include "informationalmodulewidget.h"

InformationalModuleWidget::InformationalModuleWidget(InformationModule *infmod,QWidget *parent) : QWidget(parent)
{
    //Создание элементов интерфейса
    numberLabel = new QLabel("Номер");
    yearLabel = new QLabel("Год");
    monthLabel = new QLabel("Месяц");
    voltageLabel = new QLabel("Напряжение");
    temperatureLabel = new QLabel("Температура");

    number = new QSpinBox;
    year = new QSpinBox;
    month = new QSpinBox;
    groupBox = new QGroupBox;
    voltage = new QDoubleSpinBox;
    temperature = new QSpinBox;

    //Создание layouts для правильного отображения элементов
    gridLayout = new QGridLayout;
    mainLayout = new QHBoxLayout;

    //Добавление виджетов в layout
    gridLayout->addWidget(voltageLabel, 0,0);
    gridLayout->addWidget(temperatureLabel, 1,0);
    gridLayout->addWidget(numberLabel, 2,0);
    gridLayout->addWidget(yearLabel, 3,0);
    gridLayout->addWidget(monthLabel, 4,0);

    gridLayout->addWidget(voltage, 0,1);
    gridLayout->addWidget(temperature, 1,1);
    gridLayout->addWidget(number, 2,1);
    gridLayout->addWidget(year, 3,1);
    gridLayout->addWidget(month, 4,1);

    groupBox->setLayout(gridLayout);
    mainLayout->addWidget(groupBox);

    //Передача главного layout виджету
    this->setLayout(mainLayout);

    groupBox->setTitle("Настройки МИОК");

    //Настройка диапазонов значений МИОК
    voltage->setRange(0.0, 6.0);
    temperature->setRange(0, 255);
    number->setRange(0, 999);
    year->setRange(0, 2100);
    month->setRange(1, 12);

    informationModuleSettings = infmod;

    //Присваивание полям виджета значений из настроек
    voltage->setValue(informationModuleSettings->get_voltage());
    temperature->setValue(informationModuleSettings->get_temperature());
    number->setValue(informationModuleSettings->get_number());
    year->setValue(informationModuleSettings->get_year());
    month->setValue(informationModuleSettings->get_month());

    //Настройка слотов/сигналов
    connect(voltage, SIGNAL(editingFinished()), this, SLOT(voltage_changed_slot()));
    connect(temperature, SIGNAL(editingFinished()), this, SLOT(temperature_changed_slot()));
    connect(number, SIGNAL(editingFinished()), this, SLOT(number_changed_slot()));
    connect(year, SIGNAL(editingFinished()), this, SLOT(year_changed_slot()));
    connect(month, SIGNAL(editingFinished()), this, SLOT(month_changed_slot()));
}

InformationalModuleWidget::~InformationalModuleWidget()
{
    delete gridLayout;
    delete groupBox;
    delete mainLayout;
}

void InformationalModuleWidget::number_changed_slot()
{
    informationModuleSettings->set_number(number->value());
}

void InformationalModuleWidget::year_changed_slot()
{
    informationModuleSettings->set_year(year->value());
}

void InformationalModuleWidget::month_changed_slot()
{
    informationModuleSettings->set_month(month->value());
}

void InformationalModuleWidget::voltage_changed_slot()
{
    informationModuleSettings->set_voltage(voltage->value());
}

void InformationalModuleWidget::temperature_changed_slot()
{
    informationModuleSettings->set_temperature(temperature->value());
}

