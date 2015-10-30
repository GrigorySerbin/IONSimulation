#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_settings();

    this->setWindowTitle("Симулятор зарядного устройства");

    //Создание объекта СОМ порта
    serialPort = new SerialPort(this);

    //Создание элементов для выбора СОМ порта и его подключения
    comSelect = new QComboBox(this);
    comSelect->addItems(serialPort->ports());
    informationLabel = new QLabel("Выберите COM порт", this);
    connectButton = new QPushButton("Подключить", this);
    clearScrollArea = new QPushButton("Очистить", this);

    //Виджет для отображения принимаемых посылок
    informationBrowser = new QTextBrowser(this);

    //Layoutы для правильного отображения элементов
    comConnectionLayout = new QGridLayout;
    mainLayout = new QGridLayout;

    //Добавление виджетов в layout
    comConnectionLayout->addWidget(informationLabel, 0, 0);
    comConnectionLayout->addWidget(comSelect, 0, 1);
    comConnectionLayout->addWidget(connectButton, 1, 0);
    comConnectionLayout->addWidget(clearScrollArea, 1, 1);

    settings = new QPushButton("Параметры", this);

    mainLayout->addWidget(settings, 0, 0);
    mainLayout->addLayout(comConnectionLayout, 0, 1);
    mainLayout->addWidget(informationBrowser, 1,0, 1, 2);

    centralWidget = new QWidget(this);

    centralWidget->setLayout(mainLayout);

    //Назначение главному окну программы центрального виджета
    this->setCentralWidget(centralWidget);

    //Настройка сигналов/слотов
    connect(settings, SIGNAL(pressed()), this, SLOT(settings_button_clicked()));
    connect(connectButton, SIGNAL(pressed()), this, SLOT(connect_button_clicked()));
    connect(clearScrollArea, SIGNAL(pressed()), this, SLOT(clear_button_clicked()));
    connect(serialPort, SIGNAL(ready_read_signal()), this, SLOT(process_data()));
}

MainWindow::~MainWindow()
{
    save_settings();
    delete comConnectionLayout;
    delete mainLayout;
    delete ui;
}

void MainWindow::settings_button_clicked()
{
    //Создание окна настроек по нажатию кнопки
    settingsWindow = new SettingsWindow(accum, bm, &MYKmodule, &MIOKmodule);
    settingsWindow->show();

    //Соединение сигнала закрытия окна с слотом удаления окна
    connect(settingsWindow, SIGNAL(windowClosed()), this, SLOT(delete_settings_window()));
}

void MainWindow::connect_button_clicked()
{
    //Если СОМ порт подключен - оключить и наоборот
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
    //Очистка поля для вывода принимаемых посылок
    informationBrowser->clear();
}

void MainWindow::process_data()
{
    //Функция для предварительной обработки посылки и
    //выбора необходимой функции для обработки посылки

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

    //подсчет контрольной суммы с байта num до предпоследнего байта
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


    //В зависимости от байта команд выбираем необходимую функцию
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

    //Вывод посылки на экран
    QString str(data.toHex().toUpper());
    informationBrowser->append(str);

}

void MainWindow::delete_settings_window()
{
    //Слот для удаления окна настроек
    delete settingsWindow;
    settingsWindow = nullptr;
}

void MainWindow::load_settings()
{
    //Функуия для загрузки настроек из .ini файла

    QSettings settings("settings.ini", QSettings::IniFormat);

    //Загрузка настроек для МУК
    settings.beginGroup("MYK");
    MYKmodule.set_powerBC(settings.value("powerBC", false).toBool());
    MYKmodule.set_outputPower(settings.value("outputPower", false).toBool());
    MYKmodule.set_MIOKConnection(settings.value("MIOKConnection", true).toBool());
    MYKmodule.set_CKY1Connection(settings.value("CKY1Connection", true).toBool());
    MYKmodule.set_CKY2Connection(settings.value("CKY2Connection", true).toBool());
    MYKmodule.set_CKY3Connection(settings.value("CKY3Connection", true).toBool());
    MYKmodule.set_BCConnection(settings.value("BCConnection", true).toBool());
    settings.endGroup();

    //Загрузка настроек для МИОК
    settings.beginGroup("MIOK");
    MIOKmodule.set_number(settings.value("number", 0).toInt());
    MIOKmodule.set_month(settings.value("month", 0).toInt());
    MIOKmodule.set_year(settings.value("year", 0).toInt());
    MIOKmodule.set_voltage(settings.value("voltage", 0.0).toFloat());
    MIOKmodule.set_temperature(settings.value("temperature", 0).toInt());
    settings.endGroup();


    for(int i = 0; i < 3; i++)
    {
        //Загрузка настроек для каждого батарейного модуля (БМ)
        settings.beginGroup("bm" + QString::number(i + 1));
        bm[i].set_on(settings.value("is_on", true).toBool());
        bm[i].set_number(settings.value("number", 0).toInt());
        bm[i].set_year(settings.value("year", 0).toInt());
        bm[i].set_month(settings.value("month", 0).toInt());

        for(int j = 0; j < 26; j++)
        {
            //Загрузка настроек для каждого аккумулятора
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
    //Функция для записи настроек  в .ini файл
    QSettings settings("settings.ini", QSettings::IniFormat);

    //Запсь настроек ддя МУК
    settings.beginGroup("MYK");
    settings.setValue("powerBC", MYKmodule.get_powerBC());
    settings.setValue("outputPower", MYKmodule.get_outputPower());
    settings.setValue("MIOKConnection", MYKmodule.get_MIOKConnection());
    settings.setValue("CKY1Connection", MYKmodule.get_CKY1Connection());
    settings.setValue("CKY2Connection", MYKmodule.get_CKY2Connection());
    settings.setValue("CKY3Connection", MYKmodule.get_CKY3Connection());
    settings.setValue("BCConnection", MYKmodule.get_BCConnection());
    settings.endGroup();

    //Запись настроек для МИОК
    settings.beginGroup("MIOK");
    settings.setValue("number", MIOKmodule.get_number());
    settings.setValue("month", MIOKmodule.get_month());
    settings.setValue("year", MIOKmodule.get_year());
    settings.setValue("voltage", MIOKmodule.get_voltage());
    settings.setValue("temperature", MIOKmodule.get_temperature());
    settings.endGroup();

    for(int i = 0; i < 3; i++)
    {
        //Запсь настроек для каждого БМ
        settings.beginGroup("bm" + QString::number(i + 1));

        settings.setValue("is_on", bm[i].turned_on());
        settings.setValue("number", bm[i].get_number());
        settings.setValue("year", bm[i].get_year());
        settings.setValue("month", bm[i].get_month());

        for(int j = 0; j < 26; j++)
        {
            //Запись настроек для каждого аккумулятора
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
    //Функция разбора байта команд на биты
    QBitArray bitArray(8);
    for(int i = 0; i < 8; i++) bitArray.setBit(7-i, com_byte&(1 << (7-i)));
    return bitArray;
}



void MainWindow::processing_answer_for_MYK()
{
    //Функция обработки посылки для МУКа и
    //формирования ответной посылки

    if(!MYKmodule.get_outputPower()) return;

    QByteArray answer;
    answer.append(startByteAnswer); //Стартовый байт
    answer.append(startByteAnswer); //Стартовый байт
    answer.append('\x07');          //Количество следующих байт
    answer.append(MYK);             //Адрес модуля
    unsigned char stat = byte_formation(MYKmodule.get_powerBC(),
                                        MYKmodule.get_outputPower(),
                                        0,0,0,0,0,0);
    answer.append(stat);            //Байт статуса МУК
    unsigned char err_dc = byte_formation(0,0,0,
                                          !MYKmodule.get_MIOKConnection(),
                                          !MYKmodule.get_CKY1Connection(),
                                          !MYKmodule.get_CKY2Connection(),
                                          !MYKmodule.get_CKY3Connection(),
                                          !MYKmodule.get_BCConnection());
    answer.append(err_dc);          //Байт ошибок связи с другими модулями
    answer.append('\x41');          //Установка тока
    answer.append('\x01');          //Установка старшего байта напряжения
    answer.append('\x00');          //Установка младшего байта напряжения

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < answer.size(); i++) c_sum += answer.at(i);
    c_sum ^= '\xFF';
    answer.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(answer);
}

void MainWindow::processing_answer_for_BC()
{

}

void MainWindow::processing_answer_for_MIOK_eeprom_write(QByteArray &answer)
{
    //Функция формирования ответа МИОКу в режиме записи в EEPROM

    unsigned char FN_H = answer.at(5);
    unsigned char FN_L = answer.at(6);
    short int FN = (FN_H << 8) + FN_L;  //Номер Аккумуляторной батареи
    int month = (int) answer.at(7);     //Месяц производства
    int year = (int) answer.at(8) + 2000;   //Год производства

    //Запись номера, месяца и года производства в настройки
    MIOKmodule.set_number(FN);
    MIOKmodule.set_month(month);
    MIOKmodule.set_year(year);

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x07);                  //Количество следующих байт
    responce.append(0xE1);                  //Адрес модуля
    responce.append(0xE0);                  //Байт статуса МИОК
    FN = MIOKmodule.get_number();
    FN_L = FN;
    FN_H = FN >> 8;
    responce.append(FN_H);                  //Старший байт номера
    responce.append(FN_L);                  //Младший байт номера
    responce.append(MIOKmodule.get_month());            //Месяц изготовления
    responce.append(MIOKmodule.get_year() - 2000);      //Год изготовления

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}

void MainWindow::processing_answer_for_MIOK_test_read()
{
    //Функция формирования ответа МИОКу в режиме чтения

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x09);                  //Количество следующих байт
    responce.append(0xE1);                  //Адрес модуля
    responce.append(0xC0);                  //Байт статуса МИОК
    short int FN = MIOKmodule.get_number();
    char FN_L = FN;
    char FN_H = FN >> 8;
    responce.append(FN_H);                  //Старший байт номера
    responce.append(FN_L);                  //Младший байт номера
    responce.append(MIOKmodule.get_month());            //Месяц изготовления
    responce.append(MIOKmodule.get_year() - 2000);      //Год изготовления
    short int VOLT = MIOKmodule.get_voltage() * 1000;
    char VOLT_L = VOLT;
    char VOLT_H = VOLT >> 8;
    responce.append(VOLT_H);                  //Старший байт напряжения
    responce.append(VOLT_L);                  //Младший байт напряжения

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);

}

void MainWindow::processing_answer_for_MIOK_work_read()
{
    //Функция формирования ответа МИОКу в режиме работы

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x08);                  //Количество следующих байт
    responce.append(0xE1);                  //Адрес модуля
    responce.append('\x00');                  //Байт статуса МИОК
    short int VOLT = MIOKmodule.get_voltage() * 1000;
    char VOLT_L = VOLT;
    char VOLT_H = VOLT >> 8;
    responce.append(VOLT_H);                  //Старший байт напряжения
    responce.append(VOLT_L);                  //Младший байт напряжения
    short int TEMP = MIOKmodule.get_temperature();
    char TEMP_L = TEMP;
    char TEMP_H = TEMP >> 8;
    responce.append(TEMP_H);                  //Старший байт температуры
    responce.append(TEMP_L);                  //Младший байт температуры
    responce.append(0x11);                    //Байт состояния МИОК

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    serialPort->write(responce);
}


void MainWindow::processing_answer_for_CKY1_eeprom_write(QByteArray &answer)
{
    //Функция формирования ответа СКУ1 в режиме записи в EEPROM

    int FN = (int) answer.at(5);        //Номер батарейного модуля (БМ)
    int month = (int) answer.at(6);     //Месяц изготовления БМ
    int year = (int) answer.at(7);      //Год изготовления БМ

    //Запись номера, месяца и года производства в настройки
    bm[0].set_number(FN);
    bm[0].set_month(month);
    bm[0].set_year(year + 2000);

    int counter = 7;
    for (int i = 0; i < 26; i++)
    {
        ++counter;
        accum[i].set_year((int)answer.at(++counter) + 2000);       //Запись в настройки года производства аккумулятора
        accum[i].set_quarter((int)answer.at(++counter));           //Запись в настройки квартала производства аккумулятора
        unsigned char FN_H = answer.at(++counter);
        unsigned char FN_L = answer.at(++counter);
        FN = (FN_H << 8) + FN_L;
        accum[i].set_number(FN);                                   //Запись в настройки номера аккумулятора
    }

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY1);                  //Адрес модуля
    responce.append(0xE0);                  //Байт статуса СКУ1
    responce.append((unsigned char) bm[0].get_number());        //Номер БМ
    responce.append((unsigned char) bm[0].get_month());         //Месяц производства БМ
    responce.append((unsigned char) (bm[0].get_year() - 2000)); //Год производства БМ

    for (int i = 0; i < 26; i++)
    {
        unsigned char num = i + 1;
        responce.append(num);                                           //Порядковый номер аккумулятора
        responce.append((unsigned char) (accum[i].get_year() - 2000));  //Год производства аккумулятора
        responce.append((unsigned char) accum[i].get_quarter());        //Квартал производства аккумулятора
        short int FN = accum[i].get_number();                           //Заводской номер аккумулятора
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);                                          //Старший байт заводского номера аккумулятора
        responce.append(FN_L);                                          //Младший байт заводского номера аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);

}

void MainWindow::processing_answer_for_CKY1_test_read()
{
    //Функция формирования ответа СКУ1 в режиме чтения

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY1);                  //Адрес модуля
    responce.append(0xC0);                  //Байт статуса СКУ1
    responce.append((unsigned char) bm[0].get_number());        //Номер БМ
    responce.append((unsigned char) bm[0].get_month());         //Месяц производства БМ
    responce.append((unsigned char) (bm[0].get_year() - 2000)); //Год производства БМ

    for (int i = 0; i < 26; i++)
    {
        unsigned char num = i + 1;
        responce.append(num);                                           //Порядковый номер аккумулятора
        responce.append((unsigned char) (accum[i].get_year() - 2000));  //Год производства аккумулятора
        responce.append((unsigned char) accum[i].get_quarter());        //Квартал производства аккумулятора
        short int FN = accum[i].get_number();                           //Заводской номер аккумулятора
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);                                          //Старший байт заводского номера аккумулятора
        responce.append(FN_L);                                          //Младший байт заводского номера аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY1_work_read()
{
    //Функция формирования ответа СКУ1 в режиме работы

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY1);                  //Адрес модуля
    responce.append(0x40);                  //Байт статуса СКУ1
    responce.append(0x38);                  //Байт состояния СКУ1
    responce.append(0xC0);                  //Байт состояния СКУ1
    for (int i = 0; i < 26; i++)
    {
        short int VOLT = accum[i].get_volt() * 1000;
        unsigned char VOLT_L = VOLT;
        unsigned char VOLT_H = VOLT >> 8;
        responce.append(VOLT_H);            //Старший байт напряжения
        responce.append(VOLT_L);            //Младший байт напряжения
        unsigned char temp = accum[i].get_temp();
        responce.append(temp);              //Байт температуры
        unsigned char Axx_F = byte_formation(accum[i].get_normal_volt(),
                                             accum[i].get_normal_volt(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_ballance(),
                                             accum[i].get_ballance(),
                                             0,0);

        responce.append(Axx_F);             //Байт состояния аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);


}


void MainWindow::processing_answer_for_CKY2_eeprom_write(QByteArray &answer)
{
    //Функция формирования ответа СКУ2 в режиме записи в EEPROM

    int FN = (int) answer.at(5);        //Номер батарейного модуля (БМ)
    int month = (int) answer.at(6);     //Месяц изготовления БМ
    int year = (int) answer.at(7);      //Год изготовления БМ

    //Запись номера, месяца и года производства в настройки
    bm[1].set_number(FN);
    bm[1].set_month(month);
    bm[1].set_year(year + 2000);

    int counter = 7;
    for (int i = 26; i < 52; i++)
    {
        ++counter;
        accum[i].set_year((int)answer.at(++counter) + 2000);       //Запись в настройки года производства аккумулятора
        accum[i].set_quarter((int)answer.at(++counter));           //Запись в настройки квартала производства аккумулятора
        unsigned char FN_H = answer.at(++counter);
        unsigned char FN_L = answer.at(++counter);
        FN = (FN_H << 8) + FN_L;
        accum[i].set_number(FN);                                   //Запись в настройки номера аккумулятора
    }

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY2);                  //Адрес модуля
    responce.append(0xE0);                  //Байт статуса СКУ2
    responce.append((unsigned char) bm[1].get_number());        //Номер БМ
    responce.append((unsigned char) bm[1].get_month());         //Месяц производства БМ
    responce.append((unsigned char) (bm[1].get_year() - 2000)); //Год производства БМ

    for (int i = 26; i < 52; i++)
    {
        unsigned char num = i - 25;
        responce.append(num);                                           //Порядковый номер аккумулятора
        responce.append((unsigned char) (accum[i].get_year() - 2000));  //Год производства аккумулятора
        responce.append((unsigned char) accum[i].get_quarter());        //Квартал производства аккумулятора
        short int FN = accum[i].get_number();                           //Заводской номер аккумулятора
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);                                          //Старший байт заводского номера аккумулятора
        responce.append(FN_L);                                          //Младший байт заводского номера аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY2_test_read()
{
    //Функция формирования ответа СКУ2 в режиме чтения

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY2);                  //Адрес модуля
    responce.append(0xC0);                  //Байт статуса СКУ2
    responce.append((unsigned char) bm[1].get_number());        //Номер БМ
    responce.append((unsigned char) bm[1].get_month());         //Месяц производства БМ
    responce.append((unsigned char) (bm[1].get_year() - 2000)); //Год производства БМ

    for (int i = 26; i < 52; i++)
    {
        unsigned char num = i - 25;
        responce.append(num);                                           //Порядковый номер аккумулятора
        responce.append((unsigned char) (accum[i].get_year() - 2000));  //Год производства аккумулятора
        responce.append((unsigned char) accum[i].get_quarter());        //Квартал производства аккумулятора
        short int FN = accum[i].get_number();                           //Заводской номер аккумулятора
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);                                          //Старший байт заводского номера аккумулятора
        responce.append(FN_L);                                          //Младший байт заводского номера аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY2_work_read()
{
    //Функция формирования ответа СКУ2 в режиме работы

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY2);                  //Адрес модуля
    responce.append(0x40);                  //Байт статуса СКУ2
    responce.append(0x38);                  //Байт состояния СКУ2
    responce.append(0xC0);                  //Байт состояния СКУ2
    for (int i = 26; i < 52; i++)
    {
        short int VOLT = accum[i].get_volt() * 1000;
        unsigned char VOLT_L = VOLT;
        unsigned char VOLT_H = VOLT >> 8;
        responce.append(VOLT_H);            //Старший байт напряжения
        responce.append(VOLT_L);            //Младший байт напряжения
        unsigned char temp = accum[i].get_temp();
        responce.append(temp);              //Байт температуры
        unsigned char Axx_F = byte_formation(accum[i].get_normal_volt(),
                                             accum[i].get_normal_volt(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_ballance(),
                                             accum[i].get_ballance(),
                                             0,0);

        responce.append(Axx_F);             //Байт состояния аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}


void MainWindow::processing_answer_for_CKY3_eeprom_write(QByteArray &answer)
{
    //Функция формирования ответа СКУ3 в режиме записи в EEPROM

    int FN = (int) answer.at(5);        //Номер батарейного модуля (БМ)
    int month = (int) answer.at(6);     //Месяц изготовления БМ
    int year = (int) answer.at(7);      //Год изготовления БМ

    //Запись номера, месяца и года производства в настройки
    bm[2].set_number(FN);
    bm[2].set_month(month);
    bm[2].set_year(year + 2000);

    int counter = 7;
    for (int i = 52; i < 78; i++)
    {
        ++counter;
        accum[i].set_year((int)answer.at(++counter) + 2000);       //Запись в настройки года производства аккумулятора
        accum[i].set_quarter((int)answer.at(++counter));           //Запись в настройки квартала производства аккумулятора
        unsigned char FN_H = answer.at(++counter);
        unsigned char FN_H = answer.at(++counter);
        unsigned char FN_L = answer.at(++counter);
        FN = (FN_H << 8) + FN_L;
        accum[i].set_number(FN);                                   //Запись в настройки номера аккумулятора
    }

    //формирование ответной посылки
    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY3);                  //Адрес модуля
    responce.append(0xE0);                  //Байт статуса СКУ2
    responce.append((unsigned char) bm[2].get_number());        //Номер БМ
    responce.append((unsigned char) bm[2].get_month());         //Месяц производства БМ
    responce.append((unsigned char) (bm[2].get_year() - 2000)); //Год производства БМ

    for (int i = 52; i < 78; i++)
    {
        unsigned char num = i - 51;
        responce.append(num);                                           //Порядковый номер аккумулятора
        responce.append((unsigned char) (accum[i].get_year() - 2000));  //Год производства аккумулятора
        responce.append((unsigned char) accum[i].get_quarter());        //Квартал производства аккумулятора
        short int FN = accum[i].get_number();                           //Заводской номер аккумулятора
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);                                          //Старший байт заводского номера аккумулятора
        responce.append(FN_L);                                          //Младший байт заводского номера аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY3_test_read()
{
    //Функция формирования ответа СКУ3 в режиме чтения

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY3);                  //Адрес модуля
    responce.append(0xC0);                  //Байт статуса СКУ2
    responce.append((unsigned char) bm[2].get_number());        //Номер БМ
    responce.append((unsigned char) bm[2].get_month());         //Месяц производства БМ
    responce.append((unsigned char) (bm[2].get_year() - 2000)); //Год производства БМ

    for (int i = 52; i < 78; i++)
    {
        unsigned char num = i - 51;
        responce.append(num);                                           //Порядковый номер аккумулятора
        responce.append((unsigned char) (accum[i].get_year() - 2000));  //Год производства аккумулятора
        responce.append((unsigned char) accum[i].get_quarter());        //Квартал производства аккумулятора
        short int FN = accum[i].get_number();                           //Заводской номер аккумулятора
        unsigned  char FN_L = FN;
        unsigned char FN_H = FN >> 8;
        responce.append(FN_H);                                          //Старший байт заводского номера аккумулятора
        responce.append(FN_L);                                          //Младший байт заводского номера аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}

void MainWindow::processing_answer_for_CKY3_work_read()
{
    //Функция формирования ответа СКУ3 в режиме работы

    QByteArray responce;
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(startByteAnswer);       //Стартовый байт
    responce.append(0x88);                  //Количество следующих байт
    responce.append(CKY3);                  //Адрес модуля
    responce.append(0x40);                  //Байт статуса СКУ3
    responce.append(0x38);                  //Байт состояния СКУ3
    responce.append(0xC0);                  //Байт состояния СКУ3
    for (int i = 52; i < 78; i++)
    {
        short int VOLT = accum[i].get_volt() * 1000;
        unsigned char VOLT_L = VOLT;
        unsigned char VOLT_H = VOLT >> 8;
        responce.append(VOLT_H);            //Старший байт напряжения
        responce.append(VOLT_L);            //Младший байт напряжения
        unsigned char temp = accum[i].get_temp();
        responce.append(temp);              //Байт температуры
        unsigned char Axx_F = byte_formation(accum[i].get_normal_volt(),
                                             accum[i].get_normal_volt(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_normal_temp(),
                                             accum[i].get_ballance(),
                                             accum[i].get_ballance(),
                                             0,0);

        responce.append(Axx_F);             //Байт состояния аккумулятора
    }

    //Подсчет контрольной суммы (с инверсией, без учета переполнения)
    unsigned char c_sum = 0x00;
    for (int i = 2; i < responce.size(); i++) c_sum += responce.at(i);
    c_sum ^= 0xFF;
    responce.append(c_sum);

    //Запись посылки в СОМ порт
    serialPort->write(responce);
}

unsigned char MainWindow::byte_formation(int seventh, int sixth, int fifth, int fourth,
                                         int third, int second, int first, int zero)
{
    //Фукция формирования байта

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
