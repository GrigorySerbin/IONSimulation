#ifndef CONTROLMODULE_H
#define CONTROLMODULE_H


class ControlModule
{
public:
    ControlModule();
    void set_powerBC(bool);
    void set_outputPower(bool);
    void set_MIOKConnection(bool);
    void set_CKY1Connection(bool);
    void set_CKY2Connection(bool);
    void set_CKY3Connection(bool);
    void set_BCConnection(bool);

    bool get_powerBC();
    bool get_outputPower();
    bool get_MIOKConnection();
    bool get_CKY1Connection();
    bool get_CKY2Connection();
    bool get_CKY3Connection();
    bool get_BCConnection();

private:
    bool powerBC;
    bool outputPower;
    bool MIOKConnection;
    bool CKY1Connection;
    bool CKY2Connection;
    bool CKY3Connection;
    bool BCConnection;
};

#endif // CONTROLMODULE_H
