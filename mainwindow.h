#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QString>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTextBrowser>
#include <QWidget>
#include <QBitArray>
#include <QGridLayout>
#include <QVBoxLayout>

#include "serialport.h"
#include "accumulator.h"
#include "batterymodule.h"
#include "controlmodule.h"
#include "informationmodule.h"
#include "settingswindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void settings_button_clicked();
    void connect_button_clicked();
    void clear_button_clicked();
    void process_data();
    void delete_settings_window();

private:
    Ui::MainWindow *ui;

    SerialPort *serialPort;

    QGridLayout *comConnectionLayout;
    QGridLayout *mainLayout;

    QComboBox *comSelect;
    QLabel *informationLabel;
    QPushButton *connectButton;
    QPushButton *clearScrollArea;
    QPushButton *settings;
    QTextBrowser *informationBrowser;
    QWidget *centralWidget;

    BatteryModule bm[3];
    Accumulator accum[78];
    ControlModule MYKmodule;
    InformationModule MIOKmodule;

    SettingsWindow *settingsWindow;

    void load_settings();
    void save_settings();
    QBitArray byte_to_bits(unsigned char com_byte);
    unsigned char byte_formation(int seventh, int sixth, int fifth, int fourth,
                                 int third, int second, int first, int zero);

    void processing_answer_for_MYK();
    void processing_answer_for_BC();
    void processing_answer_for_MIOK_eeprom_write(QByteArray&);
    void processing_answer_for_MIOK_test_read();
    void processing_answer_for_MIOK_work_read();

    void processing_answer_for_CKY1_eeprom_write(QByteArray&);
    void processing_answer_for_CKY1_test_read();
    void processing_answer_for_CKY1_work_read();

    void processing_answer_for_CKY2_eeprom_write(QByteArray&);
    void processing_answer_for_CKY2_test_read();
    void processing_answer_for_CKY2_work_read();

    void processing_answer_for_CKY3_eeprom_write(QByteArray&);
    void processing_answer_for_CKY3_test_read();
    void processing_answer_for_CKY3_work_read();

    static const char startByte = '\xAA';
    static const char startByteAnswer = '\x55';
    static const char CKY1 = '\xD2';
    static const char CKY2 = '\xC3';
    static const char CKY3 = '\xB4';
    static const char BC = '\xA5';
    static const char MIOK = '\xE1';
    static const char MYK = '\xF0';
};

#endif // MAINWINDOW_H
