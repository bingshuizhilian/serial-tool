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

    QPushButton* btn1;
    QLabel* label1;
    QPlainTextEdit* plntxt;
    QDateTime* datetime;
    QSerialPort* myserial;
    QComboBox* baudrates;
    QComboBox* choosecoms;
    QPushButton* btn2;
    QPalette* pt;
    QComboBox* ptBox;
    QPushButton* btn3;
    QPushButton* connectStatus;

private slots:
    void on_pushbutton_clicked(void);
    void showtime(void);
    void showserial(void);
    void updatesettings(void);
    void updatesettings2(void);
    void on_pushbutton_clicked2(void);
    void on_pushbutton_clicked3(void);
};

#endif // MAINWINDOW_H
