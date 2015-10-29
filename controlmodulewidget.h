#ifndef CONTROLMODULEWIDGET_H
#define CONTROLMODULEWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>

#include "controlmodule.h"

class ControlModuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ControlModuleWidget(ControlModule *contrMod, QWidget *parent = 0);
    ~ControlModuleWidget();
signals:

public slots:
    void outputPower_changed(int);
    void powerBC_changed(int);
    void MIOKConnection_changed(int);
    void CKY1Connection_changed(int);
    void CKY2Connection_changed(int);
    void CKY3Connection_changed(int);
    void BCConnection_changed(int);

private:
    QCheckBox *outputPower;
    QCheckBox *powerBC;
    QCheckBox *MIOKConnection;
    QCheckBox *CKY1Connection;
    QCheckBox *CKY2Connection;
    QCheckBox *CKY3Connection;
    QCheckBox *BCConnection;
    QGroupBox *groupBox;

    QGridLayout *gridLayout;
    QHBoxLayout *mainLayout;

ControlModule *controlModuleSetttings;
};

#endif // CONTROLMODULEWIDGET_H
