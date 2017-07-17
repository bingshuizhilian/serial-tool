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
    QComboBox* baudrates;
    QComboBox* chooseComs;
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

private slots:
    void on_openclosebutton_clicked(void);
    void showtime(void);
    void showserial(void);
    void update_settings_caused_by_baudrates(void);
    void update_settings_caused_by_choosecoms(void);
    void on_clrscrnbutton_clicked(void);
    void on_savebutton_clicked(void);
    void on_sendbutton_clicked(void);
    void proc_sendhex_stateChanged(void);
};

#endif // MAINWINDOW_H
