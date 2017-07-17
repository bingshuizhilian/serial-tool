#include "mainwindow.h"
#include "UI_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    btnOpenClose(new QPushButton(tr("open serial port"))),
    labelTimeDisp(new QLabel),
    plntxtOutput(new QPlainTextEdit),
    datetime(new QDateTime),
    baudrates(new QComboBox),
    chooseComs(new QComboBox),
    btnClrScrn(new QPushButton(tr("clear screen"))),
    pltBox(new QComboBox),
    btnSave(new QPushButton(tr("save screen"))),
    connectStatus(new QPushButton(tr("disconnected"))),
    leInput(new QLineEdit),
    btnSend(new QPushButton(tr("send"))),
    send0D(new QCheckBox(tr("auto send 0x0D"))),
    send0A(new QCheckBox(tr("auto send 0x0A"))),
    showhex(new QCheckBox(tr("show in hex"))),
    sendhex(new QCheckBox(tr("send in hex"))),
    sendbytecounter(0),
    receivebytecounter(0),
    labelSendBytes(new QLabel(tr("S:0"))),
    labelReceiveBytes(new QLabel(tr("R:0")))
{
    ui->setupUi(this);

    //status bar
    ui->statusBar->addWidget(labelTimeDisp);
    connectStatus->setFixedSize(110, 30);
    connectStatus->setDisabled(true);
    ui->statusBar->addWidget(connectStatus);
    ui->statusBar->addWidget(labelSendBytes);
    ui->statusBar->addWidget(labelReceiveBytes);
    ui->mainToolBar->close();
    setWindowTitle(tr("serial port tool"));
    auto labelAuthorInfo = new QLabel(tr("designed by y18077"));
    ui->statusBar->addPermanentWidget(labelAuthorInfo);

    //port and theme
    auto layoutPortGroupBox = new QGridLayout;
    baudrates->addItem(QStringLiteral("4800"), 4800);
    baudrates->addItem(QStringLiteral("9600"), 9600);
    baudrates->addItem(QStringLiteral("19200"), 19200);
    baudrates->addItem(QStringLiteral("38400"), 38400);
    baudrates->addItem(QStringLiteral("115200"), 115200);
    baudrates->addItem(QStringLiteral("460800"), 460800);
    baudrates->setCurrentIndex(4);
    connect(baudrates,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &update_settings_caused_by_baudrates);
    auto labelBaudrate = new QLabel(tr("Baudrate"));
    layoutPortGroupBox->addWidget(labelBaudrate, 0, 0);
    layoutPortGroupBox->addWidget(baudrates, 0, 1);
    const auto infos = QSerialPortInfo::availablePorts();
    for(auto &info : infos) chooseComs->addItem(info.portName(), info.portName());
    connect(chooseComs,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &update_settings_caused_by_choosecoms);
    auto labelCom = new QLabel(tr("COM"));
    layoutPortGroupBox->addWidget(labelCom, 1, 0);
    layoutPortGroupBox->addWidget(chooseComs, 1, 1);
    pltBox->addItem(QStringLiteral("day"));
    pltBox->addItem(QStringLiteral("night"));
    connect(pltBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &update_settings_caused_by_baudrates);
    auto labelTheme = new QLabel(tr("Theme"));
    layoutPortGroupBox->addWidget(labelTheme, 2, 0);
    layoutPortGroupBox->addWidget(pltBox, 2, 1);

    auto comSettingGroupBox = new QGroupBox;
    comSettingGroupBox->setTitle(tr("port settings"));
    comSettingGroupBox->setLayout(layoutPortGroupBox);

    //edit and display
    auto layoutEditGroup = new QVBoxLayout;
    layoutEditGroup->addWidget(showhex);
    connect(sendhex, &sendhex->stateChanged, this, &proc_sendhex_stateChanged);
    layoutEditGroup->addWidget(sendhex);
    layoutEditGroup->addWidget(send0D);
    layoutEditGroup->addWidget(send0A);

    auto editSettingGroupBox = new QGroupBox;
    editSettingGroupBox->setTitle(tr("edit settings"));
    editSettingGroupBox->setLayout(layoutEditGroup);

    //buttons
    auto layoutButtons = new QVBoxLayout;
    btnOpenClose->setFixedHeight(30);
    connect(btnOpenClose, &btnOpenClose->clicked, this, &on_openclosebutton_clicked);
    layoutButtons->addWidget(btnOpenClose);
    btnSave->setFixedHeight(30);
    connect(btnSave, &btnSave->clicked, this, &on_savebutton_clicked);
    layoutButtons->addWidget(btnSave);
    btnClrScrn->setFixedHeight(30);
    connect(btnClrScrn, &btnClrScrn->clicked, this, &on_clrscrnbutton_clicked);
    layoutButtons->addWidget(btnClrScrn);

    //left layout
    auto layoutLeft = new QVBoxLayout;
    layoutLeft->addWidget(comSettingGroupBox);
    layoutLeft->addWidget(editSettingGroupBox);
    layoutLeft->addLayout(layoutButtons);

    //right layout
    auto layoutRight = new QVBoxLayout;
    plntxtOutput->setReadOnly(true);
    layoutRight->addWidget(plntxtOutput);

    //sub layout of right layout
    auto layoutRightSubInput = new QHBoxLayout;
    connect(leInput, &leInput->returnPressed, this, &on_sendbutton_clicked);
    layoutRightSubInput->addWidget(leInput);
    btnSend->setFixedWidth(40);
    btnSend->setEnabled(false);
    connect(btnSend, &btnSend->clicked, this, &on_sendbutton_clicked);
    layoutRightSubInput->addWidget(btnSend);
    layoutRight->addLayout(layoutRightSubInput);

    //global layout
    auto layoutMain = new QHBoxLayout;
    layoutMain->addLayout(layoutLeft);
    layoutMain->addLayout(layoutRight);
    ui->centralWidget->setLayout(layoutMain);

    //clock timer
    QTimer *RTCtimer = new QTimer(this);
    connect(RTCtimer, SIGNAL(timeout()), this, SLOT(showtime()));
    RTCtimer->start(1000);
    showtime();

    //additional initialization
    mySerialPort = new QSerialPort(this);
    update_settings_caused_by_baudrates();
    connect(mySerialPort, &QSerialPort::readyRead, this, &MainWindow::showserial);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openclosebutton_clicked(void)
{
    if(btnOpenClose->text() == tr("open serial port"))
    {
        if(mySerialPort->open(QIODevice::ReadWrite))
        {
            btnOpenClose->setText(tr("close serial port"));
            btnSend->setEnabled(true);
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Cannot open now!"));
        }
    }
    else if(btnOpenClose->text() == tr("close serial port"))
    {
        mySerialPort->close();
        btnOpenClose->setText(tr("open serial port"));
        btnSend->setEnabled(false);
    }

    update_settings_caused_by_baudrates();
}

void MainWindow::on_clrscrnbutton_clicked(void)
{
    plntxtOutput->clear();
    sendbytecounter = 0;
    receivebytecounter = 0;
    labelSendBytes->setText(tr("S:0"));
    labelReceiveBytes->setText(tr("R:0"));
}

void MainWindow::on_savebutton_clicked(void)
{
    QString folderName = "/savedfiles/";
    QString dirPath = QDir::currentPath() + folderName;
    QDir dir(dirPath);
    if(!dir.exists())
        dir.mkdir(dirPath);
    dir.cd(dirPath);

    QString timeInfo = datetime->currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString fileName = "." + folderName + "savedfile_" + timeInfo + ".txt";
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << plntxtOutput->toPlainText();
    file.close();
}

void MainWindow::on_sendbutton_clicked(void)
{
    QString inputString = leInput->text();

    if(mySerialPort->isOpen())
    {
        if(sendhex->isChecked())
        {
            QByteArray hexDecoded = QByteArray::fromHex(inputString.toLatin1());
            sendbytecounter += mySerialPort->write(hexDecoded);
        }
        else
        {
            sendbytecounter += mySerialPort->write(inputString.toLatin1());
        }

        if(send0D->isChecked())
        {
            mySerialPort->putChar(0x0D);
            sendbytecounter += 1;
        }

        if(send0A->isChecked())
        {
            mySerialPort->putChar(0x0A);
            sendbytecounter += 1;
        }

        QString sendBytesString = "S:0";
        sendBytesString += QString::number(sendbytecounter);
        labelSendBytes->setText(sendBytesString);
    }
}

void MainWindow::showtime(void)
{
    labelTimeDisp->setText(datetime->currentDateTime().toString("yyyy/MM/dd  HH:mm:ss"));
}

void MainWindow::showserial(void)
{
    QByteArray receiveData = mySerialPort->readAll();

    if(showhex->isChecked())
    {
        QString hexStream(receiveData.toHex());
        plntxtOutput->insertPlainText(hexStream.toUpper());
    }
    else
    {
        plntxtOutput->insertPlainText(QString(receiveData));
    }

    QTextCursor cursor = plntxtOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    plntxtOutput->setTextCursor(cursor);

    receivebytecounter += receiveData.length();
    QString receiveBytesString = "R:0";
    receiveBytesString += QString::number(receivebytecounter);
    labelReceiveBytes->setText(receiveBytesString);
}

void MainWindow::update_settings_caused_by_baudrates(void)
{
    mySerialPort->setPortName(chooseComs->itemData(chooseComs->currentIndex()).toString());
    mySerialPort->setBaudRate(baudrates->itemData(baudrates->currentIndex()).toInt());

    auto plt = plntxtOutput->palette();
    switch(pltBox->currentIndex())
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
    plntxtOutput->setPalette(plt);
    leInput->setPalette(plt);

    if(btnOpenClose->text() == tr("close serial port"))
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

void MainWindow::update_settings_caused_by_choosecoms(void)
{
    if(btnOpenClose->text() == tr("close serial port"))
    {
        mySerialPort->close();
        btnOpenClose->setText(tr("open serial port"));

        connectStatus->setText("disconnected");
        connectStatus->setStyleSheet("color:gray;font:12pt;border-style:outset;");
    }

    mySerialPort->setPortName(chooseComs->itemData(chooseComs->currentIndex()).toString());
}

void MainWindow::proc_sendhex_stateChanged(void)
{
    leInput->clear();

    if(sendhex->isChecked())
    {
        QRegExp regex("[a-fA-F0-9 ]+$");
        auto validator = new QRegExpValidator(regex, leInput);
        leInput->setValidator(validator);
    }
    else
    {
        leInput->setValidator(0);
    }
}
