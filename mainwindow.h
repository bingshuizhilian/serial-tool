#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTimer>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QComboBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QPushButton* btnOpenClose;
    QLabel* labelTimeDisp;
    QPlainTextEdit* plntxtOutput;
    QDateTime* datetime;
    QSerialPort* mySerialPort;
    QComboBox* choosecoms;
    QComboBox* baudrates;
    QComboBox* databits;
    QComboBox* stopbits;
    QComboBox* parity;
    QComboBox* flowcontrol;
    QPushButton* btnClrScrn;
    QComboBox* pltBox;
    QPushButton* btnSave;
    QPushButton* connectStatus;
    QLineEdit* leInput;
    QPushButton* btnSend;
    QCheckBox* send0D;
    QCheckBox* send0A;
    QCheckBox* showhex;
    QCheckBox* sendhex;
    qlonglong sendbytecounter;
    qlonglong receivebytecounter;
    QLabel* labelSendBytes;
    QLabel* labelReceiveBytes;
    QCheckBox* autosend;
    QLineEdit* leAutoSendInterval;
    QLineEdit* leAutoSendCounter;
    QTimer *autoSendTimer;
    QStringList comNameList;

private slots:
    void on_openclosebutton_clicked(void);
    void showtime(void);
    void receive_serial_data(void);
    void hot_update_settings(void);
    void proc_change_coms(void);
    void on_clrscrnbutton_clicked(void);
    void on_savebutton_clicked(void);
    void on_sendbutton_clicked(void);
    void proc_sendhex_stateChanged(void);
    void proc_autosend_stateChanged(void);
    void on_autosendtimer_timeout(void);
    void com_monitor(void);
private:
    void open_serial_port(void);
    void close_serial_port(void);
};

#endif // MAINWINDOW_H
