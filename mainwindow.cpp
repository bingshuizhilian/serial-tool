#include "mainwindow.h"
#include "UI_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    btnOpenClose(new QPushButton(tr("open port"))),
    labelTimeDisp(new QLabel),
    plntxtOutput(new QPlainTextEdit),
    datetime(new QDateTime),
    choosecoms(new QComboBox),
    baudrates(new QComboBox),
    databits(new QComboBox),
    stopbits(new QComboBox),
    parity(new QComboBox),
    flowcontrol(new QComboBox),
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
    auto labelAuthorInfo = new QLabel(tr("bingshuizhilian@yeah.net"));
    labelAuthorInfo->setOpenExternalLinks(true);
    labelAuthorInfo->setText(QString::fromLocal8Bit("<style> a {text-decoration: none} </style> <a href = https://www.github.com/bingshuizhilian> contact author </a>"));
    labelAuthorInfo->show();
    ui->statusBar->addPermanentWidget(labelAuthorInfo);

    //port name
    auto layoutPortGroupBox = new QGridLayout;
    const auto infos = QSerialPortInfo::availablePorts();
    for(auto &info : infos) choosecoms->addItem(info.portName(), info.portName());
    connect(choosecoms,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &update_settings_caused_by_choosecoms);
    auto labelCom = new QLabel(tr("Com name"));
    layoutPortGroupBox->addWidget(labelCom, 0, 0);
    layoutPortGroupBox->addWidget(choosecoms, 0, 1);
    //baudrate
    baudrates->addItem(QStringLiteral("4800"), 4800);
    baudrates->addItem(QStringLiteral("9600"), 9600);
    baudrates->addItem(QStringLiteral("19200"), 19200);
    baudrates->addItem(QStringLiteral("38400"), 38400);
    baudrates->addItem(QStringLiteral("115200"), 115200);
    baudrates->addItem(QStringLiteral("460800"), 460800);
    baudrates->addItem(tr("Custom"));
    baudrates->setCurrentIndex(4);
    baudrates->setInsertPolicy(QComboBox::NoInsert);
    connect(baudrates,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hot_update_settings);
    connect(baudrates,  &QComboBox::editTextChanged, this, &hot_update_settings);
    auto labelBaudrate = new QLabel(tr("Baudrate"));
    layoutPortGroupBox->addWidget(labelBaudrate, 1, 0);
    layoutPortGroupBox->addWidget(baudrates, 1, 1);
    //data bits
    databits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    databits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    databits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    databits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    databits->setCurrentIndex(3);
    connect(databits,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hot_update_settings);
    auto labelDatabits = new QLabel(tr("Data bits"));
    layoutPortGroupBox->addWidget(labelDatabits, 2, 0);
    layoutPortGroupBox->addWidget(databits, 2, 1);
    //parity
    parity->addItem(tr("None"), QSerialPort::NoParity);
    parity->addItem(tr("Even"), QSerialPort::EvenParity);
    parity->addItem(tr("Odd"), QSerialPort::OddParity);
    parity->addItem(tr("Mark"), QSerialPort::MarkParity);
    parity->addItem(tr("Space"), QSerialPort::SpaceParity);
    connect(parity,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hot_update_settings);
    auto labelParity = new QLabel(tr("Parity"));
    layoutPortGroupBox->addWidget(labelParity, 3, 0);
    layoutPortGroupBox->addWidget(parity, 3, 1);
    //stop bits
    stopbits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    stopbits->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
    stopbits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    connect(stopbits,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hot_update_settings);
    auto labelStopbits = new QLabel(tr("Stop bits"));
    layoutPortGroupBox->addWidget(labelStopbits, 4, 0);
    layoutPortGroupBox->addWidget(stopbits, 4, 1);
    //flow control
    flowcontrol->addItem(tr("None"), QSerialPort::NoFlowControl);
    flowcontrol->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    flowcontrol->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    connect(flowcontrol,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hot_update_settings);
    auto labelFlowcontrol = new QLabel(tr("Flow ctrl"));
    layoutPortGroupBox->addWidget(labelFlowcontrol, 5, 0);
    layoutPortGroupBox->addWidget(flowcontrol, 5, 1);
    //theme
    pltBox->addItem(QStringLiteral("light"));
    pltBox->addItem(QStringLiteral("dark"));
    connect(pltBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hot_update_settings);
    auto labelTheme = new QLabel(tr("Theme"));
    layoutPortGroupBox->addWidget(labelTheme, 6, 0);
    layoutPortGroupBox->addWidget(pltBox, 6, 1);

    //serial port setting related layout
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
    auto layoutButtons = new QGridLayout;
    btnOpenClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    connect(btnOpenClose, &btnOpenClose->clicked, this, &on_openclosebutton_clicked);
    layoutButtons->addWidget(btnOpenClose, 0, 0, 2, 1);
    btnSave->setFixedHeight(30);
    connect(btnSave, &btnSave->clicked, this, &on_savebutton_clicked);
    layoutButtons->addWidget(btnSave, 0, 1, 1, 1);
    btnClrScrn->setFixedHeight(30);
    connect(btnClrScrn, &btnClrScrn->clicked, this, &on_clrscrnbutton_clicked);
    layoutButtons->addWidget(btnClrScrn, 1, 1, 1, 1);



    //global left layout
    auto layoutLeft = new QVBoxLayout;
    layoutLeft->addWidget(comSettingGroupBox);
    layoutLeft->addWidget(editSettingGroupBox);
    layoutLeft->addLayout(layoutButtons);

    //global right layout
    auto layoutRight = new QVBoxLayout;
    plntxtOutput->setReadOnly(true);
    layoutRight->addWidget(plntxtOutput);

    //sub layout of global right layout
    auto layoutRightSubInput = new QHBoxLayout;
    connect(leInput, &leInput->returnPressed, this, &on_sendbutton_clicked);
    layoutRightSubInput->addWidget(leInput);
    btnSend->setFixedWidth(40);
    btnSend->setEnabled(false);
    connect(btnSend, &btnSend->clicked, this, &on_sendbutton_clicked);
    layoutRightSubInput->addWidget(btnSend);
    layoutRight->addLayout(layoutRightSubInput);

    //global layout
    auto layoutGlobal = new QHBoxLayout;
    layoutGlobal->addLayout(layoutLeft);
    layoutGlobal->addLayout(layoutRight);
    ui->centralWidget->setLayout(layoutGlobal);

    //resize the main window
    QSize size = this->size();
    size.setWidth(700);
    this->resize(size);

    //clock timer
    QTimer *RTCtimer = new QTimer(this);
    connect(RTCtimer, SIGNAL(timeout()), this, SLOT(showtime()));
    RTCtimer->start(1000);
    showtime();

    //additional initialization
    mySerialPort = new QSerialPort(this);
    hot_update_settings();
    connect(mySerialPort, &QSerialPort::readyRead, this, &MainWindow::receive_serial_data);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openclosebutton_clicked(void)
{
    if(btnOpenClose->text() == tr("open port"))
    {
        if(mySerialPort->open(QIODevice::ReadWrite))
        {
            btnOpenClose->setText(tr("close port"));
            btnSend->setEnabled(true);
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Cannot open now!"));
        }
    }
    else if(btnOpenClose->text() == tr("close port"))
    {
        mySerialPort->close();
        btnOpenClose->setText(tr("open port"));
        btnSend->setEnabled(false);
    }

    hot_update_settings();
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

        QString sendBytesString = "S:";
        sendBytesString += QString::number(sendbytecounter);
        labelSendBytes->setText(sendBytesString);
    }
}

void MainWindow::showtime(void)
{
    labelTimeDisp->setText(datetime->currentDateTime().toString("yyyy/MM/dd  HH:mm:ss"));
}

void MainWindow::receive_serial_data(void)
{
    QByteArray receiveData = mySerialPort->readAll();

    if(showhex->isChecked())
    {
        QString hexStream(receiveData.toHex());

        for(qint32 iter = 0; iter < receiveData.length(); ++iter)
        {
            hexStream.insert(2 + 3 * iter, " ");
        }

        plntxtOutput->insertPlainText(hexStream.toUpper());
    }
    else
    {
        plntxtOutput->insertPlainText(receiveData);
    }

    QTextCursor cursor = plntxtOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    plntxtOutput->setTextCursor(cursor);

    receivebytecounter += receiveData.length();
    QString receiveBytesString = "R:";
    receiveBytesString += QString::number(receivebytecounter);
    labelReceiveBytes->setText(receiveBytesString);
}

void MainWindow::hot_update_settings(void)
{
    if (6 == baudrates->currentIndex())
    {
        baudrates->setEditable(true);
        if("Custom" == baudrates->currentText())
            baudrates->clearEditText();
        QLineEdit *edit = baudrates->lineEdit();
        QIntValidator *intValidator = new QIntValidator(0, 4000000);
        edit->setValidator(intValidator);
        mySerialPort->setBaudRate(baudrates->currentText().toInt());
    }
    else
    {
        baudrates->setEditable(false);
        mySerialPort->setBaudRate(baudrates->itemData(baudrates->currentIndex()).toInt());
    }

    mySerialPort->setPortName(choosecoms->itemData(choosecoms->currentIndex()).toString());
    mySerialPort->setDataBits(static_cast<QSerialPort::DataBits>(databits->itemData(databits->currentIndex()).toInt()));
    mySerialPort->setParity(static_cast<QSerialPort::Parity>(parity->itemData(parity->currentIndex()).toInt()));
    mySerialPort->setStopBits(static_cast<QSerialPort::StopBits>(stopbits->itemData(stopbits->currentIndex()).toInt()));
    mySerialPort->setFlowControl(static_cast<QSerialPort::FlowControl>(flowcontrol->itemData(flowcontrol->currentIndex()).toInt()));

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

    if(btnOpenClose->text() == tr("close port"))
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
    if(btnOpenClose->text() == tr("close port"))
    {
        mySerialPort->close();
        btnOpenClose->setText(tr("open port"));

        connectStatus->setText("disconnected");
        connectStatus->setStyleSheet("color:gray;font:12pt;border-style:outset;");
    }

    mySerialPort->setPortName(choosecoms->itemData(choosecoms->currentIndex()).toString());
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
