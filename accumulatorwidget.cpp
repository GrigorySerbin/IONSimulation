#include "accumulatorwidget.h"

AccumulatorWidget::AccumulatorWidget(int accumNumber, Accumulator *settings, QWidget *parent) : QWidget(parent)
{
    //Создание элементов интерфейса
    voltLabel = new QLabel("Напряжение");
    tempLabel = new QLabel("Температура");
    numberLabel = new QLabel("Номер");
    yearLabel = new QLabel("Год");
    quarterLabel = new QLabel("Квартал");

    randVolt = new QCheckBox("rand");
    normVolt = new QCheckBox("норма");
    randTemp = new QCheckBox("rand");
    normTemp = new QCheckBox("норма");
    ballance = new QCheckBox("баллансировка");
    power = new QCheckBox("вкл.");

    voltage = new QDoubleSpinBox;
    temperature = new QSpinBox;
    number = new QSpinBox;
    year = new QSpinBox;
    quarter = new QSpinBox;
    groupBox = new QGroupBox;

    //Создание layouts для правильного отображения элементов
    gridLayout = new QGridLayout;
    mainLayout = new QHBoxLayout;

    //Добавление виджетов в layout
    gridLayout->addWidget(voltLabel, 0,0);
    gridLayout->addWidget(voltage, 0,1);
    gridLayout->addWidget(power, 0,3);

    gridLayout->addWidget(randVolt, 1,0);
    gridLayout->addWidget(normVolt, 1,1);
    gridLayout->addWidget(numberLabel, 1,2);
    gridLayout->addWidget(number, 1,3);

    gridLayout->addWidget(tempLabel, 2,0);
    gridLayout->addWidget(temperature, 2,1);
    gridLayout->addWidget(yearLabel, 2,2);
    gridLayout->addWidget(year, 2,3);

    gridLayout->addWidget(randTemp, 3,0);
    gridLayout->addWidget(normTemp, 3,1);
    gridLayout->addWidget(quarterLabel, 3,2);
    gridLayout->addWidget(quarter, 3,3);

    gridLayout->addWidget(ballance, 4,0);

    groupBox->setLayout(gridLayout);
    mainLayout->addWidget(groupBox);

    //Передача главного layout виджету
    this->setLayout(mainLayout);

    groupBox->setTitle("BM" + QString::number(accumNumber/26 + 1) +
                       " Аккумулятор " + QString::number(1 + accumNumber - (accumNumber/26)*26));

    //Настройка диапазонов значений аккумулятора
    voltage->setRange(0.0, 6.0);
    temperature->setRange(0, 255);
    number->setRange(0, 9999);
    year->setRange(0, 2100);
    quarter->setRange(1, 4);

    accumSettings = settings;

    //Присваивание полям виджета значений из настроек
    randVolt->setChecked(accumSettings->get_rand_volt());
    normVolt->setChecked(accumSettings->get_normal_volt());
    randTemp->setChecked(accumSettings->get_rand_temp());
    normTemp->setChecked(accumSettings->get_normal_temp());
    ballance->setChecked(accumSettings->get_ballance());
    power->setChecked(accumSettings->turned_on());

    voltage->setValue(accumSettings->get_volt());
    temperature->setValue(accumSettings->get_temp());
    number->setValue(accumSettings->get_number());
    year->setValue(accumSettings->get_year());
    quarter->setValue(accumSettings->get_quarter());


    //Настройка слотов/сигналов
    connect(randVolt, SIGNAL(stateChanged(int)), this, SLOT(rand_volt_slot(int)));
    connect(normVolt, SIGNAL(stateChanged(int)), this, SLOT(norm_volt_slot(int)));
    connect(randTemp, SIGNAL(stateChanged(int)), this, SLOT(rand_temp_slot(int)));
    connect(normTemp, SIGNAL(stateChanged(int)), this, SLOT(norm_temp_slot(int)));
    connect(ballance, SIGNAL(stateChanged(int)), this, SLOT(ballance_slot(int)));
    connect(power, SIGNAL(stateChanged(int)), this, SLOT(power_slot(int)));

    connect(voltage, SIGNAL(editingFinished()), this, SLOT(voltage_changed_slot()));
    connect(temperature, SIGNAL(editingFinished()), this, SLOT(temperature_changed_slot()));
    connect(number, SIGNAL(editingFinished()), this, SLOT(number_changed_slot()));
    connect(year, SIGNAL(editingFinished()), this, SLOT(year_changed_slot()));
    connect(quarter, SIGNAL(editingFinished()), this, SLOT(quarter_changed_slot()));
}

AccumulatorWidget::~AccumulatorWidget()
{
    delete gridLayout;
    delete groupBox;
    delete mainLayout;
}

void AccumulatorWidget::rand_volt_slot(int state)
{
    accumSettings->set_rand_volt(state);
}

void AccumulatorWidget::rand_temp_slot(int state)
{
    accumSettings->set_rand_temp(state);
}

void AccumulatorWidget::norm_volt_slot(int state)
{
    accumSettings->set_normal_volt(state);
}

void AccumulatorWidget::norm_temp_slot(int state)
{
    accumSettings->set_normal_temp(state);
}

void AccumulatorWidget::power_slot(int state)
{
    accumSettings->set_on(state);
}

void AccumulatorWidget::ballance_slot(int state)
{
    accumSettings->set_ballance(state);
}

void AccumulatorWidget::voltage_changed_slot()
{
    accumSettings->set_volt(voltage->value());
}

void AccumulatorWidget::temperature_changed_slot()
{
    accumSettings->set_temp(temperature->value());
}

void AccumulatorWidget::number_changed_slot()
{
    accumSettings->set_number(number->value());
}

void AccumulatorWidget::year_changed_slot()
{
    accumSettings->set_year(year->value());
}

void AccumulatorWidget::quarter_changed_slot()
{
    accumSettings->set_quarter(quarter->value());
}

