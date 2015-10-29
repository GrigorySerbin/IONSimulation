#include "controlmodule.h"

ControlModule::ControlModule()
{
    powerBC = false;
    outputPower = false;
    MIOKConnection = true;
    CKY1Connection = true;
    CKY2Connection = true;
    CKY3Connection = true;
    BCConnection = true;
}

void ControlModule::set_powerBC(bool state)
{
    powerBC = state;
}

void ControlModule::set_outputPower(bool state)
{
    outputPower = state;
}

void ControlModule::set_MIOKConnection(bool state)
{
    MIOKConnection = state;
}

void ControlModule::set_CKY1Connection(bool state)
{
    CKY1Connection = state;
}

void ControlModule::set_CKY2Connection(bool state)
{
    CKY2Connection = state;
}

void ControlModule::set_CKY3Connection(bool state)
{
    CKY3Connection = state;
}

void ControlModule::set_BCConnection(bool state)
{
    BCConnection = state;
}

bool ControlModule::get_powerBC()
{
    return powerBC;
}

bool ControlModule::get_outputPower()
{
    return outputPower;
}

bool ControlModule::get_MIOKConnection()
{
    return MIOKConnection;
}

bool ControlModule::get_CKY1Connection()
{
    return CKY1Connection;
}

bool ControlModule::get_CKY2Connection()
{
    return CKY2Connection;
}

bool ControlModule::get_CKY3Connection()
{
    return CKY3Connection;
}

bool ControlModule::get_BCConnection()
{
    return BCConnection;
}

