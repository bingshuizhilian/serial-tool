#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTimer>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>

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
    enum CmdType
    {
        CMD_HELP,
        CMD_SHOW,
        CMD_HIDE,
        CMD_RESET,
        CMD_CLEAR_INPUT,
        CMD_SAVE_CONFIG_FILE,
        CMD_LOAD_CONFIG_FILE,
        CMD_HYTERA_CUSTOMIZED,
        CMD_HYTERA_CUSTOMIZED_AT,
        CMD_HYTERA_CUSTOMIZED_DIAL,
        CMD_NULL
    };

    const static int WINDOW_ORIGNAL_WIDTH = 660;
    const static int WINDOW_EXTRA_WIDTH = 360;
    const static int EXTRA_ITEM_NUMBER = 10;
    const QString CONFIG_FILE_NAME = "config.json";

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
    void procExSendButtonClicked(int btn_id);
    void procExHexButtonClicked(int btn_id);
    void procExAutosendStateChanged(void);
    void procExAutosendTimerTimeout(void);
private:
    void openSerialPort(void);
    void closeSerialPort(void);
    void procConfigFile(CmdType cmd);
    void showHelpInfo(CmdType cmd);
    void procResetCmd(CmdType cmd);
    void exInputGroupInitialization(void);
    void componentsInitialization(void);
};

#endif // MAINWINDOW_H
