#include "mainwindow.h"
#include "UI_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    btn1(new QPushButton(tr("open serial port"))),
    label1(new QLabel),
    plntxt(new QPlainTextEdit),
    datetime(new QDateTime),
    baudrates(new QComboBox),
    choosecoms(new QComboBox),
    btn2(new QPushButton(tr("clear screen"))),
    ptBox(new QComboBox),
    btn3(new QPushButton(tr("save as text"))),
    connectStatus(new QPushButton(tr("disconnected")))
{
    ui->setupUi(this);
    ui->statusBar->addWidget(label1);
    connectStatus->setFixedSize(110, 30);
    connectStatus->setDisabled(true);
    ui->statusBar->addWidget(connectStatus);
    ui->mainToolBar->close();
    setWindowTitle(tr("serial port tool"));
    auto label2 = new QLabel(tr("designed by y18077"));
    ui->statusBar->addPermanentWidget(label2);

    auto layout1 = new QVBoxLayout;
    baudrates->addItem(QStringLiteral("4800"), 4800);
    baudrates->addItem(QStringLiteral("9600"), 9600);
    baudrates->addItem(QStringLiteral("19200"), 19200);
    baudrates->addItem(QStringLiteral("115200"), 115200);
    baudrates->addItem(QStringLiteral("460800"), 460800);
    baudrates->setCurrentIndex(3);
    connect(baudrates,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &updatesettings);
    layout1->addWidget(baudrates);
    const auto infos = QSerialPortInfo::availablePorts();
    for(auto &info : infos) choosecoms->addItem(info.portName(), info.portName());
    connect(choosecoms,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &updatesettings2);
    layout1->addWidget(choosecoms);
    ptBox->addItem(QStringLiteral("day theme"));
    ptBox->addItem(QStringLiteral("night theme"));
    connect(ptBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &updatesettings);
    layout1->addWidget(ptBox);
    btn1->setFixedSize(110, 30);
    connect(btn1, &btn1->clicked, this, &on_pushbutton_clicked);
    layout1->addWidget(btn1);
    btn3->setFixedSize(110, 30);
    connect(btn3, &btn3->clicked, this, &on_pushbutton_clicked3);
    layout1->addWidget(btn3);
    btn2->setFixedSize(110, 30);
    connect(btn2, &btn2->clicked, this, &on_pushbutton_clicked2);
    layout1->addWidget(btn2);


    auto layout2 = new QHBoxLayout;
    layout2->addLayout(layout1);
    plntxt->setReadOnly(true);
    layout2->addWidget(plntxt);
    ui->centralWidget->setLayout(layout2);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showtime()));
    timer->start(1000);
    showtime();

    myserial = new QSerialPort(this);
    updatesettings();
    connect(myserial, &QSerialPort::readyRead, this, &MainWindow::showserial);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushbutton_clicked(void)
{
    if(btn1->text() == tr("open serial port"))
    {
        if(myserial->open(QIODevice::ReadWrite))
        {
            btn1->setText(tr("close serial port"));
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Cannot open now!"));
        }
    }
    else if(btn1->text() == tr("close serial port"))
    {
        myserial->close();
        btn1->setText(tr("open serial port"));
    }

    updatesettings();
}

void MainWindow::on_pushbutton_clicked2(void)
{
    plntxt->clear();
}

void MainWindow::on_pushbutton_clicked3(void)
{
    QString folderName = "/logfiles/";
    QString dirPath = QDir::currentPath() + folderName;
    QDir dir(dirPath);
    if(!dir.exists())
        dir.mkdir(dirPath);
    dir.cd(dirPath);

    QString timeInfo = datetime->currentDateTime().toString(Qt::ISODate);
    QString fileName = "." + folderName + "logfile_" + timeInfo + ".txt";
    fileName.replace(':', '-');
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << plntxt->toPlainText();
    file.close();
}

void MainWindow::showtime(void)
{
    label1->setText(datetime->currentDateTime().toString(Qt::ISODate));
}

void MainWindow::showserial(void)
{
    QByteArray data = myserial->readAll();
    plntxt->insertPlainText(QString(data));
    QTextCursor cursor = plntxt->textCursor();
    cursor.movePosition(QTextCursor::End);
    plntxt->setTextCursor(cursor);
}

void MainWindow::updatesettings(void)
{
    myserial->setPortName(choosecoms->itemData(choosecoms->currentIndex()).toString());
    myserial->setBaudRate(baudrates->itemData(baudrates->currentIndex()).toInt());

    auto plt = plntxt->palette();
    switch(ptBox->currentIndex())
    {
    case 0:
        plt.setColor(QPalette::Base, Qt::white);
        plt.setColor(QPalette::Text, Qt::black);
        break;
    case 1:
        plt.setColor(QPalette::Base, Qt::black);
        plt.setColor(QPalette::Text, Qt::green);
        break;
    default:;
    }
    plntxt->setPalette(plt);

    if(btn1->text() == tr("close serial port"))
    {
        connectStatus->setText("connected");
        connectStatus->setStyleSheet("color:green;font:12pt;border-style:outset;");
    }
    else
    {
        connectStatus->setText("disconnected");
        connectStatus->setStyleSheet("color:gray;font:12pt;border-style:outset;");
    }
}

void MainWindow::updatesettings2(void)
{
    if(btn1->text() == tr("close serial port"))
    {
        myserial->close();
        btn1->setText(tr("open serial port"));

        connectStatus->setText("disconnected");
        connectStatus->setStyleSheet("color:gray;font:12pt;border-style:outset;");
    }

    myserial->setPortName(choosecoms->itemData(choosecoms->currentIndex()).toString());
}
