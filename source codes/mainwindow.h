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
    typedef enum
    {
        CMD_HELP,
        CMD_SHOW,
        CMD_HIDE,
        CMD_SAVE_CONFIG_FILE,
        CMD_LOAD_CONFIG_FILE,
        CMD_HYTERA_CUSTOMIZED,
        CMD_HYTERA_CUSTOMIZED_AT,
        CMD_HYTERA_CUSTOMIZED_DIAL,
        CMD_NULL
    }SERIAL_CMD_TYPE;

    const static int WINDOW_ORIGNAL_WIDTH = 660;
    const static int WINDOW_EXTRA_WIDTH = 360;
    const static int EXTRA_ITEM_NUMBER = 10;
    const QString SETTINGS_FILE_NAME = "settings.ini";

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
    QComboBox* theme;
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
    QGroupBox* exInputGroup;
    QCheckBox* exAutosend;
    QLineEdit* exLeAutosendInterval;
    QTimer *exAutosendTimer;
    qint32 whichExContentIsToBeSent;
    QList<QCheckBox*> exHexCheckBoxs;
    QList<QLineEdit*> exLeInputs;
    QList<QPushButton*> exSendBtns;

private slots:
    void procOpenCloseButtonClicked(void);
    void showTime(void);
    void receiveSerialData(void);
    void hotUpdateSettings(void);
    void procPortChanged(void);
    void procClrScrnButtonClicked(void);
    void procSaveButtonClicked(void);
    void procSendButtonClicked(void);
    void procSendhexStateChanged(void);
    void procAutosendStateChanged(void);
    void procAutosendTimerTimeout(void);
    void portMonitor(void);
    void procQuickCommand(void);
    void procExSendButtonClicked(int btd_id);
    void procExHexButtonClicked(int btd_id);
    void procExAutosendStateChanged(void);
    void procExAutosendTimerTimeout(void);
private:
    void openSerialPort(void);
    void closeSerialPort(void);
    void procConfigFile(SERIAL_CMD_TYPE cmd);
    void showHelpInfo(SERIAL_CMD_TYPE cmd);
    void exInputGroupInitialization(void);
    void componentsInitialization(void);
};

#endif // MAINWINDOW_H
