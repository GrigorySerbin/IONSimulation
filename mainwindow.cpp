#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_settings();

    for(int i = 0; i < 78; i++)
        accumWingets[i] = new AccumulatorWidget(i, &accum[i]);

    for(int i = 0; i < 3; i++)
        batteryWidgets[i] = new BatteryModuleWidget(i+1, &bm[i]);

    controlModuleWidget = new ControlModuleWidget(&MYKmodule);

    informationModuleWidget = new InformationalModuleWidget(&MIOKmodule);

    scrollAreaLayout = new QGridLayout;

    scrollAreaLayout->addWidget(informationModuleWidget, 0, 0, 1, 2);
    scrollAreaLayout->addWidget(controlModuleWidget, 0, 2);

    for(int i = 0; i < 3; i++)
        scrollAreaLayout->addWidget(batteryWidgets[i], 1, i);

    for(int i = 0; i < 78; i++)
        scrollAreaLayout->addWidget(accumWingets[i], i+2, 0, 1, 3);

    settingsScrollArea = new QScrollArea;
    scrollAreaWidget = new QWidget;
    scrollAreaWidget->setLayout(scrollAreaLayout);
    settingsScrollArea->setWidget(scrollAreaWidget);

    serialPort = new SerialPort;

    comSelect = new QComboBox;
    comSelect->addItems(serialPort->ports());
    informationLabel = new QLabel("Выберите COM порт");
    connectButton = new QPushButton("Подключить");
    clearScrollArea = new QPushButton("Очистить");

    informationBrowser = new QTextBrowser;

    comConnectionLayout = new QGridLayout;
    mainLayout = new QGridLayout;

    comConnectionLayout->addWidget(informationLabel, 0, 0);
    comConnectionLayout->addWidget(comSelect, 0, 1);
    comConnectionLayout->addWidget(connectButton, 1, 0);
    comConnectionLayout->addWidget(clearScrollArea, 1, 1);

    settings = new QPushButton("Параметры");

    mainLayout->addWidget(settings, 0, 0);
    mainLayout->addLayout(comConnectionLayout, 0, 1);
    mainLayout->addWidget(informationBrowser, 1,0, 1, 2);

    centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);

    connect(settings, SIGNAL(pressed()), this, SLOT(settings_button_clicked()));
    connect(connectButton, SIGNAL(pressed()), this, SLOT(connect_button_clicked()));
    connect(clearScrollArea, SIGNAL(pressed()), this, SLOT(clear_button_clicked()));
    connect(serialPort, SIGNAL(ready_read_signal()), this, SLOT(process_data()));


}

MainWindow::~MainWindow()
{
    delete scrollAreaLayout;
    delete comConnectionLayout;
    delete mainLayout;

    delete comSelect;
    delete serialPort;
    delete informationLabel;
    delete connectButton;
    delete clearScrollArea;
    delete settings;
    delete informationBrowser;
    delete settingsScrollArea;
    delete centralWidget;

    save_settings();
    delete ui;
}

void MainWindow::settings_button_clicked()
{
    settingsScrollArea->show();
}

void MainWindow::connect_button_clicked()
{
    static bool status = false;
    if(!status)
        if(serialPort->setPort(comSelect->currentText()))
        {
            connectButton->setText("Отключить");
            status = true;
            return;
        }

    serialPort->close();
    connectButton->setText("Подключить");
    status = false;
}

void MainWindow::clear_button_clicked()
{
    informationBrowser->clear();
}

void MainWindow::process_data()
{

    QByteArray data = serialPort->read();

    // 6 - минимально возможная длина посылки
    if(data.size() < 6 || data[0] != startByte || data[1] != startByte)
    {
        informationBrowser->append("Посылка неверна!\n");
        return;
    }

    unsigned char num_c = (unsigned char) data[2];
    int num = (int) num_c; //количество байт, которое необходимо еще считать

    if(data.size() < num + 3)
    {
        informationBrowser->append("Посылка неверна!\n");
        return;
    }

    //подсчет контрольной суммы с байта num до пердпоследнего байта
    //(последний байт - контрольная сумма)
    unsigned char c_sum = '\x00';
    for(int i = 2; i < num + 2; i++) c_sum += data[i];
    c_sum ^= '\xFF';

    unsigned char c_sum_data = data[num + 2];

    if(c_sum != c_sum_data)
    {
        informationBrowser->append("Посылка неверна!\n");
        return;
    }

    //считывание адреса модуля
    char addr = data[3];
    //считывание байта команд
    unsigned char command_byte = data[4];
    QBitArray command = byte_to_bits(command_byte);


    switch(addr)
    {
    case MYK:
        processing_answer_for_MYK();
        break;
    case BC:
        processing_answer_for_BC();
        break;
    case MIOK:
        if (command.at(7) && command.at(5)) processing_answer_for_MIOK_eeprom_write(data);
        if (command.at(7) && !command.at(5)) processing_answer_for_MIOK_test_read();
        if (!command.at(7) && !command.at(5)) processing_answer_for_MIOK_work_read();
        break;
    case CKY1:
        if (command.at(7) && command.at(5)) processing_answer_for_CKY1_eeprom_write(data);
        if (command.at(7) && !command.at(5)) processing_answer_for_CKY1_test_read();
        if (!command.at(7) && !command.at(5)) processing_answer_for_CKY1_work_read();
        break;
    case CKY2:
        if (command.at(7) && command.at(5)) processing_answer_for_CKY2_eeprom_write(data);
        if (command.at(7) && !command.at(5)) processing_answer_for_CKY2_test_read();
        if (!command.at(7) && !command.at(5)) processing_answer_for_CKY2_work_read();
        break;
    case CKY3:
        if (command.at(7) && command.at(5)) processing_answer_for_CKY3_eeprom_write(data);
        if (command.at(7) && !command.at(5)) processing_answer_for_CKY3_test_read();
        if (!command.at(7) && !command.at(5)) processing_answer_for_CKY3_work_read();
        break;
    }


    QString str(data.toHex().toUpper());
    informationBrowser->append(str);

}

void MainWindow::load_settings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("MYK");
    MYKmodule.set_powerBC(settings.value("powerBC", false).toBool());
    MYKmodule.set_outputPower(settings.value("outputPower", false).toBool());
    MYKmodule.set_MIOKConnection(settings.value("MIOKConnection", true).toBool());
    MYKmodule.set_CKY1Connection(settings.value("CKY1Connection", true).toBool());
    MYKmodule.set_CKY2Connection(settings.value("CKY2Connection", true).toBool());
    MYKmodule.set_CKY3Connection(settings.value("CKY3Connection", true).toBool());
    MYKmodule.set_BCConnection(settings.value("BCConnection", true).toBool());
    settings.endGroup();

    settings.beginGroup("MIOK");
    MIOKmodule.set_number(settings.value("number", 0).toInt());
    MIOKmodule.set_month(settings.value("month", 0).toInt());
    MIOKmodule.set_year(settings.value("year", 0).toInt());
    MIOKmodule.set_voltage(settings.value("voltage", 0.0).toFloat());
    MIOKmodule.set_temperature(settings.value("temperature", 0).toInt());
    settings.endGroup();


    for(int i = 0; i < 3; i++)
    {
        settings.beginGroup("bm" + QString::number(i + 1));
        bm[i].set_on(settings.value("is_on", true).toBool());
        bm[i].set_number(settings.value("number", 0).toInt());
        bm[i].set_year(settings.value("year", 0).toInt());
        bm[i].set_month(settings.value("month", 0).toInt());

        for(int j = 0; j < 26; j++)
        {
            settings.beginGroup("accum" + QString::number(j + 1));

            accum[i*26 + j].set_on(settings.value("is_on", true).toBool());
            accum[i*26 + j].set_rand_volt(settings.value("rand_volt", true).toBool());
            accum[i*26 + j].set_normal_volt(settings.value("normal_volt", true).toBool());
            accum[i*26 + j].set_rand_temp(settings.value("rand_temp", true).toBool());
            accum[i*26 + j].set_normal_temp(settings.value("normal_temp", true).toBool());
            accum[i*26 + j].set_ballance(settings.value("ballance", true).toBool());

            accum[i*26 + j].set_volt(settings.value("voltage", 0.0).toFloat());
            accum[i*26 + j].set_temp(settings.value("temperature", 0.0).toInt());
            accum[i*26 + j].set_number(settings.value("number", 0.0).toInt());
            accum[i*26 + j].set_year(settings.value("year", 0.0).toInt());
            accum[i*26 + j].set_quarter(settings.value("quarter", 0.0).toInt());

            settings.endGroup();
        }

        settings.endGroup();
    }

}

void MainWindow::save_settings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("MYK");
    settings.setValue("powerBC", MYKmodule.get_powerBC());
    settings.setValue("outputPower", MYKmodule.get_outputPower());
    settings.setValue("MIOKConnection", MYKmodule.get_MIOKConnection());
    settings.setValue("CKY1Connection", MYKmodule.get_CKY1Connection());
    settings.setValue("CKY2Connection", MYKmodule.get_CKY2Connection());
    settings.setValue("CKY3Connection", MYKmodule.get_CKY3Connection());
    settings.setValue("BCConnection", MYKmodule.get_BCConnection());
    settings.endGroup();

    settings.beginGroup("MIOK");
    settings.setValue("number", MIOKmodule.get_number());
    settings.setValue("month", MIOKmodule.get_month());
    settings.setValue("year", MIOKmodule.get_year());
    settings.setValue("voltage", MIOKmodule.get_voltage());
    settings.setValue("temperature", MIOKmodule.get_temperature());
    settings.endGroup();

    for(int i = 0; i < 3; i++)
    {
        settings.beginGroup("bm" + QString::number(i + 1));

        settings.setValue("is_on", bm[i].turned_on());
        settings.setValue("number", bm[i].get_number());
        settings.setValue("year", bm[i].get_year());
        settings.setValue("month", bm[i].get_month());

        for(int j = 0; j < 26; j++)
        {
            settings.beginGroup("accum" + QString::number(j + 1));

            settings.setValue("is_on", accum[i*26 + j].turned_on());
            settings.setValue("rand_volt", accum[i*26 + j].get_rand_volt());
            settings.setValue("normal_volt", accum[i*26 + j].get_normal_volt());
            settings.setValue("rand_temp", accum[i*26 + j].get_rand_temp());
            settings.setValue("normal_temp", accum[i*26 + j].get_normal_temp());
            settings.setValue("ballance", accum[i*26 + j].get_ballance());
            settings.setValue("voltage", accum[i*26 + j].get_volt());
            settings.setValue("temperature", accum[i*26 + j].get_temp());
            settings.setValue("number", accum[i*26 + j].get_number());
            settings.setValue("year", accum[i*26 + j].get_year());
            settings.setValue("quarter", accum[i*26 + j].get_quarter());

            settings.endGroup();
        }

        settings.endGroup();
    }

}

QBitArray MainWindow::byte_to_bits(unsigned char com_byte)
{
    QBitArray bitArray(8);
    for(int i = 0; i < 8; i++) bitArray.setBit(7-i, com_byte&(1 << (7-i)));
    return bitArray;
}



void MainWindow::processing_answer_for_MYK()
{


    QByteArray answer;
    answer.append(startByteAnswer);
    answer.append(startByteAnswer);
    answer.append('\x07');
    answer.append(MYK);
    unsigned char stat = byte_formation(MYKmodule.get_powerBC(),
                                        MYKmodule.get_outputPower(),
                                        0,0,0,0,0,0);
    answer.append(stat);
    unsigned char err_dc = byte_formation(0,0,0,
                                          !MYKmodule.get_MIOKConnection(),
                                          !MYKmodule.get_CKY1Connection(),
                                          !MYKmodule.get_CKY2Connection(),
                                          !MYKmodule.get_CKY3Connection(),
                                          !MYKmodule.get_BCConnection());
    answer.append(err_dc);
    answer.append('\x41');
    answer.append('\x01');
    answer.append('\x00');
    unsigned char c_sum = 0x00;
    for (int i = 2; i < answer.size(); i++) c_sum += answer.at(i);
    c_sum ^= '\xFF';
    answer.append(c_sum);

    serialPort->write(answer);
}

void MainWindow::processing_answer_for_BC()
{

}

void MainWindow::processing_answer_for_MIOK_eeprom_write(QByteArray &answer)
{
    unsigned char FN_H = answer.at(5);
    unsigned char FN_L = answer.at(6);
    short int FN = (FN_H << 8) + FN_L;
    int month = (int) answer.at(7);
    int year = (int) answer.at(8) + 2000;
    MIOKmodule.set_number(FN);
    MIOKmodule.set_month(month);
    MIOKmodule.set_year(year);

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x07);
    responce.append(0xE1);
    responce.append(0xE0);
    FN = MIOKmodule.get_number();
    FN_L = FN;
    FN_H = FN >> 8;
    responce.append(FN_H);
    responce.append(FN_L);
    responce.append(MIOKmodule.get_month());
    responce.append(MIOKmodule.get_year() - 2000);
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}

void MainWindow::processing_answer_for_MIOK_test_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x09);
    responce.append(0xE1);
    responce.append(0xC0);
    short int FN = MIOKmodule.get_number();
    char FN_L = FN;
    char FN_H = FN >> 8;
    responce.append(FN_H);
    responce.append(FN_L);
    responce.append(MIOKmodule.get_month());
    responce.append(MIOKmodule.get_year() - 2000);
    short int VOLT = MIOKmodule.get_voltage() * 1000;
    char VOLT_L = VOLT;
    char VOLT_H = VOLT >> 8;
    responce.append(VOLT_H);
    responce.append(VOLT_L);
    char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);

}

void MainWindow::processing_answer_for_MIOK_work_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x08);
    responce.append(0xE1);
    responce.append('\x00');
    short int VOLT = MIOKmodule.get_voltage() * 1000;
    char VOLT_L = VOLT;
    char VOLT_H = VOLT >> 8;
    responce.append(VOLT_H);
    responce.append(VOLT_L);
    short int TEMP = MIOKmodule.get_temperature();
    char TEMP_L = TEMP;
    char TEMP_H = TEMP >> 8;
    responce.append(TEMP_H);
    responce.append(TEMP_L);
    responce.append(0x11);
    char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}


void MainWindow::processing_answer_for_CKY1_eeprom_write(QByteArray &answer)
{
    int FN = (int) answer.at(5);
    int month = (int) answer.at(6);
    int year = (int) answer.at(7);
    bm[0].set_number(FN);
    bm[0].set_month(month);
    bm[0].set_year(year + 2000);

    int counter = 7;
    for (int i = 0; i < 26; i++)
    {
        ++counter;
        accum[i].set_year((int)answer.at(++counter) + 2000);
        accum[i].set_quarter((int)answer.at(++counter));
        unsigned char FN_H = answer.at(++counter);
        unsigned char FN_L = answer.at(++counter);
        FN = (FN_H << 8) + FN_L;
        accum[i].set_number(FN);
    }

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY1);
    responce.append(0xE0);
    responce.append((unsigned char) bm[0].get_number());
    responce.append((unsigned char) bm[0].get_month());
    responce.append((unsigned char) (bm[0].get_year() - 2000));

    for (int i = 0; i < 26; i++)
    {
        unsigned char num = i + 1;
        responce.append(num);
        responce.append((unsigned char) (accum[i].get_year() - 2000));
        responce.append((unsigned char) accum[i].get_quarter());
        short int FN = accum[i].get_number();
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);
        responce.append(FN_L);
    }
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
    //QString str(responce.toHex().toUpper());
    //informationBrowser->append(str);

}

void MainWindow::processing_answer_for_CKY1_test_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY1);
    responce.append(0xC0);
    responce.append((unsigned char) bm[0].get_number());
    responce.append((unsigned char) bm[0].get_month());
    responce.append((unsigned char) (bm[0].get_year() - 2000));

    for (int i = 0; i < 26; i++)
    {
        unsigned char num = i + 1;
        responce.append(num);
        responce.append((unsigned char) (accum[i].get_year() - 2000));
        responce.append((unsigned char) accum[i].get_quarter());
        short int FN = accum[i].get_number();
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);
        responce.append(FN_L);
    }

    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY1_work_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY1);
    responce.append(0x40);
    responce.append(0x38);
    responce.append(0xC0);
    for (int i = 0; i < 26; i++)
    {
        short int VOLT = accum[i].get_volt() * 1000;
        unsigned char VOLT_L = VOLT;
        unsigned char VOLT_H = VOLT >> 8;
        responce.append(VOLT_H);
        responce.append(VOLT_L);
        unsigned char temp = accum[i].get_temp();
        responce.append(temp);
        unsigned char Axx_F = byte_formation(accum[i].get_normal_volt(),
                                             accum[i].get_normal_volt(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_ballance(),
                                             accum[i].get_ballance(),
                                             0,0);

        responce.append(Axx_F);
    }

    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);


}


void MainWindow::processing_answer_for_CKY2_eeprom_write(QByteArray &answer)
{
    int FN = (int) answer.at(5);
    int month = (int) answer.at(6);
    int year = (int) answer.at(7);
    bm[1].set_number(FN);
    bm[1].set_month(month);
    bm[1].set_year(year + 2000);

    int counter = 7;
    for (int i = 26; i < 52; i++)
    {
        ++counter;
        accum[i].set_year((int)answer.at(++counter) + 2000);
        accum[i].set_quarter((int)answer.at(++counter));
        unsigned char FN_H = answer.at(++counter);
        unsigned char FN_L = answer.at(++counter);
        FN = (FN_H << 8) + FN_L;
        accum[i].set_number(FN);
    }

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY2);
    responce.append(0xE0);
    responce.append((unsigned char) bm[1].get_number());
    responce.append((unsigned char) bm[1].get_month());
    responce.append((unsigned char) (bm[1].get_year() - 2000));

    for (int i = 26; i < 52; i++)
    {
        unsigned char num = i - 25;
        responce.append(num);
        responce.append((unsigned char) (accum[i].get_year() - 2000));
        responce.append((unsigned char) accum[i].get_quarter());
        short int FN = accum[i].get_number();
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);
        responce.append(FN_L);
    }
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY2_test_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY2);
    responce.append(0xC0);
    responce.append((unsigned char) bm[1].get_number());
    responce.append((unsigned char) bm[1].get_month());
    responce.append((unsigned char) (bm[1].get_year() - 2000));

    for (int i = 26; i < 52; i++)
    {
        unsigned char num = i - 25;
        responce.append(num);
        responce.append((unsigned char) (accum[i].get_year() - 2000));
        responce.append((unsigned char) accum[i].get_quarter());
        short int FN = accum[i].get_number();
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);
        responce.append(FN_L);
    }

    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY2_work_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY2);
    responce.append(0x40);
    responce.append(0x38);
    responce.append(0xC0);
    for (int i = 26; i < 52; i++)
    {
        short int VOLT = accum[i].get_volt() * 1000;
        unsigned char VOLT_L = VOLT;
        unsigned char VOLT_H = VOLT >> 8;
        responce.append(VOLT_H);
        responce.append(VOLT_L);
        unsigned char temp = accum[i].get_temp();
        responce.append(temp);
        unsigned char Axx_F = byte_formation(accum[i].get_normal_volt(),
                                             accum[i].get_normal_volt(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_ballance(),
                                             accum[i].get_ballance(),
                                             0,0);

        responce.append(Axx_F);
    }

    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}


void MainWindow::processing_answer_for_CKY3_eeprom_write(QByteArray &answer)
{
    int FN = (int) answer.at(5);
    int month = (int) answer.at(6);
    int year = (int) answer.at(7);
    bm[2].set_number(FN);
    bm[2].set_month(month);
    bm[2].set_year(year + 2000);

    int counter = 7;
    for (int i = 52; i < 78; i++)
    {
        ++counter;
        accum[i].set_year((int)answer.at(++counter) + 2000);
        accum[i].set_quarter((int)answer.at(++counter));
        unsigned char FN_H = answer.at(++counter);
        unsigned char FN_L = answer.at(++counter);
        FN = (FN_H << 8) + FN_L;
        accum[i].set_number(FN);
    }

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY3);
    responce.append(0xE0);
    responce.append((unsigned char) bm[2].get_number());
    responce.append((unsigned char) bm[2].get_month());
    responce.append((unsigned char) (bm[2].get_year() - 2000));

    for (int i = 52; i < 78; i++)
    {
        unsigned char num = i - 51;
        responce.append(num);
        responce.append((unsigned char) (accum[i].get_year() - 2000));
        responce.append((unsigned char) accum[i].get_quarter());
        short int FN = accum[i].get_number();
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);
        responce.append(FN_L);
    }
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY3_test_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY3);
    responce.append(0xC0);
    responce.append((unsigned char) bm[2].get_number());
    responce.append((unsigned char) bm[2].get_month());
    responce.append((unsigned char) (bm[2].get_year() - 2000));

    for (int i = 52; i < 78; i++)
    {
        unsigned char num = i - 51;
        responce.append(num);
        responce.append((unsigned char) (accum[i].get_year() - 2000));
        responce.append((unsigned char) accum[i].get_quarter());
        short int FN = accum[i].get_number();
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);
        responce.append(FN_L);
    }

    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY3_work_read()
{
    QByteArray responce;
    responce.append(startByteAnswer);
    responce.append(startByteAnswer);
    responce.append(0x88);
    responce.append(CKY3);
    responce.append(0x40);
    responce.append(0x38);
    responce.append(0xC0);
    for (int i = 52; i < 78; i++)
    {
        short int VOLT = accum[i].get_volt() * 1000;
        unsigned char VOLT_L = VOLT;
        unsigned char VOLT_H = VOLT >> 8;
        responce.append(VOLT_H);
        responce.append(VOLT_L);
        unsigned char temp = accum[i].get_temp();
        responce.append(temp);
        unsigned char Axx_F = byte_formation(accum[i].get_normal_volt(),
                                             accum[i].get_normal_volt(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_ballance(),
                                             accum[i].get_ballance(),
                                             0,0);

        responce.append(Axx_F);
    }

    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}

unsigned char MainWindow::byte_formation(int seventh, int sixth, int fifth, int fourth,
                                         int third, int second, int first, int zero)
{
    unsigned char byte = '\x00';
    byte |= seventh ? '\x01' : '\x00';
    byte <<= 1;
    byte |= sixth ? '\x01' : '\x00';
    byte <<= 1;
    byte |= fifth ? '\x01' : '\x00';
    byte <<= 1;
    byte |= fourth ? '\x01' : '\x00';
    byte <<= 1;
    byte |= third ? '\x01' : '\x00';
    byte <<= 1;
    byte |= second ? '\x01' : '\x00';
    byte <<= 1;
    byte |= first ? '\x01' : '\x00';
    byte <<= 1;
    byte |= zero ? '\x01' : '\x00';

    return byte;

}
