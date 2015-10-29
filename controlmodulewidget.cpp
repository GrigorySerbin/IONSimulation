#include "controlmodulewidget.h"

ControlModuleWidget::ControlModuleWidget(ControlModule *contrMod,QWidget *parent) : QWidget(parent)
{
    outputPower = new QCheckBox("вкл.");
    powerBC = new QCheckBox("вкл. питание БС");
    MIOKConnection = new QCheckBox("соединение с МИОК");
    CKY1Connection = new QCheckBox("соединение с СКУ1");
    CKY2Connection = new QCheckBox("соединение с СКУ2");
    CKY3Connection = new QCheckBox("соединение с СКУ3");
    BCConnection = new QCheckBox("соединение с БС");

    gridLayout = new QGridLayout;
    mainLayout = new QHBoxLayout;

    gridLayout->addWidget(outputPower, 0,0);
    gridLayout->addWidget(powerBC, 0,1);
    gridLayout->addWidget(MIOKConnection, 1,0);
    gridLayout->addWidget(CKY1Connection, 1,1);
    gridLayout->addWidget(CKY2Connection, 2,0);
    gridLayout->addWidget(CKY3Connection, 2,1);
    gridLayout->addWidget(BCConnection, 3,0);

    groupBox = new QGroupBox;

    groupBox->setLayout(gridLayout);
    mainLayout->addWidget(groupBox);
    this->setLayout(mainLayout);

    groupBox->setTitle("Настройки МУК");

    controlModuleSetttings = contrMod;

    outputPower->setChecked(controlModuleSetttings->get_outputPower());
    powerBC->setChecked(controlModuleSetttings->get_powerBC());
    MIOKConnection->setChecked(controlModuleSetttings->get_MIOKConnection());
    CKY1Connection->setChecked(controlModuleSetttings->get_CKY1Connection());
    CKY2Connection->setChecked(controlModuleSetttings->get_CKY2Connection());
    CKY3Connection->setChecked(controlModuleSetttings->get_CKY3Connection());
    BCConnection->setChecked(controlModuleSetttings->get_BCConnection());

    connect(outputPower, SIGNAL(stateChanged(int)), this, SLOT(outputPower_changed(int)));
    connect(powerBC, SIGNAL(stateChanged(int)), this, SLOT(powerBC_changed(int)));
    connect(MIOKConnection, SIGNAL(stateChanged(int)), this, SLOT(MIOKConnection_changed(int)));
    connect(CKY1Connection, SIGNAL(stateChanged(int)), this, SLOT(CKY1Connection_changed(int)));
    connect(CKY2Connection, SIGNAL(stateChanged(int)), this, SLOT(CKY2Connection_changed(int)));
    connect(CKY3Connection, SIGNAL(stateChanged(int)), this, SLOT(CKY3Connection_changed(int)));
    connect(BCConnection, SIGNAL(stateChanged(int)), this, SLOT(BCConnection_changed(int)));
}

ControlModuleWidget::~ControlModuleWidget()
{
    delete gridLayout;
    delete groupBox;
    delete mainLayout;
}

void ControlModuleWidget::outputPower_changed(int state)
{
    controlModuleSetttings->set_outputPower(state);
}

void ControlModuleWidget::powerBC_changed(int state)
{
    controlModuleSetttings->set_powerBC(state);
}

void ControlModuleWidget::MIOKConnection_changed(int state)
{
    controlModuleSetttings->set_MIOKConnection(state);
}

void ControlModuleWidget::CKY1Connection_changed(int state)
{
    controlModuleSetttings->set_CKY1Connection(state);
}

void ControlModuleWidget::CKY2Connection_changed(int state)
{
    controlModuleSetttings->set_CKY2Connection(state);
}

void ControlModuleWidget::CKY3Connection_changed(int state)
{
    controlModuleSetttings->set_CKY3Connection(state);
}

void ControlModuleWidget::BCConnection_changed(int state)
{
    controlModuleSetttings->set_BCConnection(state);
}

