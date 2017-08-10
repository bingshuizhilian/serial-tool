#include "mainwindow.h"
#include "UI_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //component initialization
    componentsInitialization();

    //status bar
    ui->statusBar->addWidget(labelTimeDisp);
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
    for(auto &info : infos)
    {
        choosecoms->addItem(info.portName(), info.portName());
        comNameList.push_back(info.portName());
    }
    connect(choosecoms,  static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this, &procPortChanged);
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
    connect(baudrates,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hotUpdateSettings);
    connect(baudrates,  &QComboBox::editTextChanged, this, &hotUpdateSettings);
    auto labelBaudrate = new QLabel(tr("Baudrate"));
    layoutPortGroupBox->addWidget(labelBaudrate, 1, 0);
    layoutPortGroupBox->addWidget(baudrates, 1, 1);
    //data bits
    databits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    databits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    databits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    databits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    databits->setCurrentIndex(3);
    connect(databits,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hotUpdateSettings);
    auto labelDatabits = new QLabel(tr("Data bits"));
    layoutPortGroupBox->addWidget(labelDatabits, 2, 0);
    layoutPortGroupBox->addWidget(databits, 2, 1);
    //parity
    parity->addItem(tr("None"), QSerialPort::NoParity);
    parity->addItem(tr("Even"), QSerialPort::EvenParity);
    parity->addItem(tr("Odd"), QSerialPort::OddParity);
    parity->addItem(tr("Mark"), QSerialPort::MarkParity);
    parity->addItem(tr("Space"), QSerialPort::SpaceParity);
    connect(parity,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hotUpdateSettings);
    auto labelParity = new QLabel(tr("Parity"));
    layoutPortGroupBox->addWidget(labelParity, 3, 0);
    layoutPortGroupBox->addWidget(parity, 3, 1);
    //stop bits
    stopbits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    stopbits->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
    stopbits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    connect(stopbits,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hotUpdateSettings);
    auto labelStopbits = new QLabel(tr("Stop bits"));
    layoutPortGroupBox->addWidget(labelStopbits, 4, 0);
    layoutPortGroupBox->addWidget(stopbits, 4, 1);
    //flow control
    flowcontrol->addItem(tr("None"), QSerialPort::NoFlowControl);
    flowcontrol->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    flowcontrol->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    connect(flowcontrol,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hotUpdateSettings);
    auto labelFlowcontrol = new QLabel(tr("Flow ctrl"));
    layoutPortGroupBox->addWidget(labelFlowcontrol, 5, 0);
    layoutPortGroupBox->addWidget(flowcontrol, 5, 1);
    //theme
    pltBox->addItem(QStringLiteral("light"));
    pltBox->addItem(QStringLiteral("dark"));
    connect(pltBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hotUpdateSettings);
    auto labelTheme = new QLabel(tr("Theme"));
    layoutPortGroupBox->addWidget(labelTheme, 6, 0);
    layoutPortGroupBox->addWidget(pltBox, 6, 1);

    //serial port setting related layout
    auto comSettingGroupBox = new QGroupBox;
    comSettingGroupBox->setTitle(tr("port settings"));
    comSettingGroupBox->setLayout(layoutPortGroupBox);

    //edit and display
    auto layoutEditGroupBox = new QGridLayout;
    layoutEditGroupBox->addWidget(showhex, 0, 0);
    connect(sendhex, &sendhex->stateChanged, this, &procSendhexStateChanged);
    layoutEditGroupBox->addWidget(sendhex, 0, 1);
    layoutEditGroupBox->addWidget(send0D, 1, 0);
    layoutEditGroupBox->addWidget(send0A, 1, 1);

    auto editSettingGroupBox = new QGroupBox;
    editSettingGroupBox->setTitle(tr("edit settings"));
    editSettingGroupBox->setLayout(layoutEditGroupBox);

    //auto send
    auto layoutAutoSendGroupBox = new QGridLayout;
    layoutAutoSendGroupBox->addWidget(autosend, 0, 0);
    connect(autosend, &autosend->stateChanged, this, &procAutosendStateChanged);
    leAutoSendInterval->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    leAutoSendInterval->setFixedWidth(30);
    layoutAutoSendGroupBox->addWidget(leAutoSendInterval, 0, 1);
    leAutoSendInterval->setValidator(new QIntValidator(1, 1000*60*60*24));
    leAutoSendInterval->setText("1000");
    auto labelAutoSendInterval = new QLabel(tr("ms"));
    layoutAutoSendGroupBox->addWidget(labelAutoSendInterval, 0, 2);
    leAutoSendCounter->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    leAutoSendCounter->setFixedWidth(30);
    layoutAutoSendGroupBox->addWidget(leAutoSendCounter, 0, 3);
    leAutoSendCounter->setValidator(new QIntValidator(0, 99999999));
    leAutoSendCounter->setText("0");
    auto labelAutoSendCounter = new QLabel(tr("times"));
    layoutAutoSendGroupBox->addWidget(labelAutoSendCounter, 0, 4);

    auto autoSendGroupBox = new QGroupBox;
    autoSendGroupBox->setTitle(tr("auto send"));
    autoSendGroupBox->setLayout(layoutAutoSendGroupBox);

    //buttons
    auto layoutButtons = new QGridLayout;
    QSize btnSize(50, 30);
    btnOpenClose->setFixedSize(btnSize);
    connect(btnOpenClose, &btnOpenClose->clicked, this, &procOpenCloseButtonClicked);
    layoutButtons->addWidget(btnOpenClose, 0, 0);
    btnSave->setFixedSize(btnSize);
    connect(btnSave, &btnSave->clicked, this, &procSaveButtonClicked);
    layoutButtons->addWidget(btnSave, 0, 1);
    btnClrScrn->setFixedSize(btnSize);
    connect(btnClrScrn, &btnClrScrn->clicked, this, &procClrScrnButtonClicked);
    layoutButtons->addWidget(btnClrScrn, 0, 2);

    //global left layout
    auto layoutLeft = new QVBoxLayout;
    layoutLeft->addWidget(comSettingGroupBox);
    layoutLeft->addWidget(editSettingGroupBox);
    layoutLeft->addWidget(autoSendGroupBox);
    layoutLeft->addLayout(layoutButtons);

    //global right layout
    auto layoutRight = new QVBoxLayout;
    plntxtOutput->setReadOnly(true);
    layoutRight->addWidget(plntxtOutput);

    //sub layout of global right layout
    auto layoutRightSubInput = new QHBoxLayout;
    connect(leInput, &leInput->returnPressed, this, &procSendButtonClicked);
    connect(leInput, &leInput->returnPressed, this, &procQuickCommand);
    layoutRightSubInput->addWidget(leInput);
    btnSend->setFixedWidth(40);
    btnSend->setEnabled(false);
    connect(btnSend, &btnSend->clicked, this, &procSendButtonClicked);
    layoutRightSubInput->addWidget(btnSend);
    layoutRight->addLayout(layoutRightSubInput);

    //extra right input layout
    exInputGroup->hide();
    exInputGroupInitialization();

    //global layout
    auto layoutGlobal = new QHBoxLayout;
    layoutGlobal->addLayout(layoutLeft);
    layoutGlobal->addLayout(layoutRight);
    layoutGlobal->addWidget(exInputGroup);
    ui->centralWidget->setLayout(layoutGlobal);

    //resize the main window
    QSize size = this->size();
    size.setWidth(WINDOW_ORIGNAL_WIDTH);
    this->resize(size);

    //clock timer
    QTimer *RTCtimer = new QTimer(this);
    connect(RTCtimer, SIGNAL(timeout()), this, SLOT(showTime()));
    RTCtimer->start(1000);
    showTime();

    //com monitor timer
    QTimer *comMonitorTimer = new QTimer(this);
    connect(comMonitorTimer, SIGNAL(timeout()), this, SLOT(portMonitor()));
    comMonitorTimer->start(500);

    //additional initialization
    mySerialPort = new QSerialPort(this);
    hotUpdateSettings();
    connect(mySerialPort, &QSerialPort::readyRead, this, &MainWindow::receiveSerialData);
    connect(autoSendTimer, SIGNAL(timeout()), this, SLOT(procAutosendTimerTimeout()));
}

MainWindow::~MainWindow()
{
    this->closeSerialPort();
    delete ui;
}

void MainWindow::componentsInitialization(void)
{
    btnOpenClose = new QPushButton(tr("open"));
    labelTimeDisp = new QLabel;
    plntxtOutput = new QPlainTextEdit;
    datetime = new QDateTime;
    choosecoms = new QComboBox;
    baudrates = new QComboBox;
    databits = new QComboBox;
    stopbits = new QComboBox;
    parity = new QComboBox;
    flowcontrol = new QComboBox;
    btnClrScrn = new QPushButton(tr("clear"));
    pltBox = new QComboBox;
    btnSave = new QPushButton(tr("save"));
    connectStatus = new QPushButton(tr("disconnected"));
    leInput = new QLineEdit;
    btnSend = new QPushButton(tr("send"));
    send0D = new QCheckBox(tr("add 0x0D"));
    send0A = new QCheckBox(tr("add 0x0A"));
    showhex = new QCheckBox(tr("show hex"));
    sendhex = new QCheckBox(tr("send hex"));
    sendbytecounter = 0;
    receivebytecounter = 0;
    labelSendBytes = new QLabel(tr("S:0"));
    labelReceiveBytes = new QLabel(tr("R:0"));
    autosend = new QCheckBox(tr("on"));
    leAutoSendInterval = new QLineEdit;
    leAutoSendCounter = new QLineEdit;
    autoSendTimer = new QTimer;
    exInputGroup = new QGroupBox(tr("extra send options"));
    exAutosend = new QCheckBox(tr("loop to send each content with interval"));
    exLeAutosendInterval = new QLineEdit;
    exAutosendTimer = new QTimer;
    whichExContentIsToBeSent = -1;
}

void MainWindow::procOpenCloseButtonClicked(void)
{
    if(btnOpenClose->text() == tr("open"))
    {
        this->openSerialPort();
    }
    else if(btnOpenClose->text() == tr("close"))
    {
        this->closeSerialPort();
    }

    hotUpdateSettings();
}

void MainWindow::procClrScrnButtonClicked(void)
{
    plntxtOutput->clear();
    sendbytecounter = 0;
    receivebytecounter = 0;
    labelSendBytes->setText(tr("S:0"));
    labelReceiveBytes->setText(tr("R:0"));
}

void MainWindow::procSaveButtonClicked(void)
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

void MainWindow::procSendButtonClicked(void)
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

void MainWindow::procQuickCommand(void)
{
    if(mySerialPort->isOpen())
    {
        return;
    }

    QString inputString = leInput->text();
    QStringList sourceShowCmd = { ":show extra", ":se" };
    QStringList sourceHideCmd = { ":hide extra", ":he" };

    if((sourceShowCmd.at(0) == inputString.toLower() || sourceShowCmd.at(1) == inputString.toLower()) && exInputGroup->isHidden())
    {
        exInputGroup->show();

        QSize size = this->size();
        size.setWidth(WINDOW_ORIGNAL_WIDTH + WINDOW_EXTRA_WIDTH);
        this->resize(size);

//        qDebug()<< "extraInputGroup->width:" << extraInputGroup->width() << "\n";
    }
    else if((sourceHideCmd.at(0) == inputString.toLower() || sourceHideCmd.at(1) == inputString.toLower()) && !exInputGroup->isHidden())
    {
        exInputGroup->hide();

        QSize size = this->size();
        size.setWidth(WINDOW_ORIGNAL_WIDTH);
        this->resize(size);

//        qDebug() << "this->width:" << this->width() << "\n";
    }
    else if(inputString.isEmpty())
    {
        showHelpInfo();
    }
    else
    {
        //do nothing for now
    }
}

void MainWindow::exInputGroupInitialization(void)
{
    exInputGroup->setFixedWidth(WINDOW_EXTRA_WIDTH);
    auto extraMainlayout = new QGridLayout;

    auto exAutoSendlayout = new QHBoxLayout;
    connect(exAutosend, &autosend->stateChanged, this, &procExAutosendStateChanged);
    exAutoSendlayout->addWidget(exAutosend);
    exLeAutosendInterval->setFixedWidth(30);
    exLeAutosendInterval->setValidator(new QIntValidator(3,1000*60*60*24));
    exLeAutosendInterval->setText("1000");
    exAutoSendlayout->addWidget(exLeAutosendInterval);
    auto labelExAutoSendInterval = new QLabel(tr("ms"));
    exAutoSendlayout->addWidget(labelExAutoSendInterval);

    auto exAutoSendOptionsGroup = new QGroupBox;
    exAutoSendOptionsGroup->setFixedHeight(40);
    exAutoSendOptionsGroup->setLayout(exAutoSendlayout);
    extraMainlayout->addWidget(exAutoSendOptionsGroup, 0, 0, 1, 3);

    QList<QLabel*> exLabelGroups;
    QStringList labelString = { "hex", "contents", "send" };
    for(auto i = 0; i < 3; ++i)
    {
        exLabelGroups.push_back(new QLabel);
        exLabelGroups[i]->setAlignment(Qt::AlignCenter);
        exLabelGroups[i]->setText(labelString.at(i));
        exLabelGroups[i]->setFixedHeight(12);
        extraMainlayout->addWidget(exLabelGroups[i], 1, i);
    }

    auto hexBtnGroup = new QButtonGroup;
    hexBtnGroup->setExclusive(false);
    auto sendBtnGroup = new QButtonGroup;
    for(auto i = 0; i < EXTRA_ITEM_NUMBER; ++i)
    {
        exHexCheckBoxs.push_back(new QCheckBox);
        exHexCheckBoxs.at(i)->setFixedWidth(15);
        hexBtnGroup->addButton(exHexCheckBoxs.at(i), i);
        extraMainlayout->addWidget(exHexCheckBoxs.at(i), 2 + i, 0);

        exLeInputs.push_back(new QLineEdit);
        exLeInputs.at(i)->setFixedWidth(280);
        extraMainlayout->addWidget(exLeInputs.at(i), 2 + i, 1);

        exSendBtns.push_back(new QPushButton);
        exSendBtns.at(i)->setText(QString::number(i + 1));
        exSendBtns.at(i)->setFixedWidth(25);
        exSendBtns.at(i)->setEnabled(false);
        sendBtnGroup->addButton(exSendBtns.at(i), i);
        extraMainlayout->addWidget(exSendBtns.at(i), 2 + i, 2);
    }

    exInputGroup->setLayout(extraMainlayout);
    connect(exAutosendTimer, SIGNAL(timeout()), this, SLOT(procExAutosendTimerTimeout()));
    connect(hexBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(procExHexButtonClicked(int)));
    connect(sendBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(procExSendButtonClicked(int)));
}

void MainWindow::procExAutosendStateChanged(void)
{
    if(exAutosend->isChecked())
    {
        if("0" == exLeAutosendInterval->text())
        {
            exLeAutosendInterval->setText(tr("1"));
        }

        exLeAutosendInterval->setEnabled(false);
        exAutosendTimer->setInterval(exLeAutosendInterval->text().toInt());
        exAutosendTimer->start();
        this->procExAutosendTimerTimeout();
    }
    else
    {
        exLeAutosendInterval->setEnabled(true);
        exAutosendTimer->stop();
        whichExContentIsToBeSent = -1;
    }
}

void MainWindow::procExAutosendTimerTimeout(void)
{
    if(!mySerialPort->isOpen())
    {
        return;
    }

    do
    {
        ++whichExContentIsToBeSent;
        whichExContentIsToBeSent %= EXTRA_ITEM_NUMBER;
    }while(exLeInputs.at(whichExContentIsToBeSent)->text().isEmpty());

    qDebug() << "whichExContentIsToBeSent" << whichExContentIsToBeSent << "\n";

    if(!exLeInputs.at(whichExContentIsToBeSent)->text().isEmpty())
    {
        this->procExSendButtonClicked(whichExContentIsToBeSent);
    }
}

void MainWindow::procExHexButtonClicked(int btd_id)
{
//    qDebug() << "btd_id" << btd_id << "\n";
    exLeInputs.at(btd_id)->clear();

    if(exHexCheckBoxs.at(btd_id)->isChecked())
    {
        QRegExp regex("[a-fA-F0-9 ]+$");
        auto validator = new QRegExpValidator(regex, exLeInputs.at(btd_id));
        exLeInputs.at(btd_id)->setValidator(validator);
    }
    else
    {
        exLeInputs.at(btd_id)->setValidator(0);
    }
}

void MainWindow::procExSendButtonClicked(int btd_id)
{
//    qDebug() << "btd_id" << btd_id << "\n";
    QString inputString = exLeInputs.at(btd_id)->text();

    if(mySerialPort->isOpen())
    {
        if(exHexCheckBoxs.at(btd_id)->isChecked())
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

void MainWindow::showHelpInfo(void)
{
    QStringList hlpInfo;

    hlpInfo.push_back(tr("1.Welcome to use and spread this serial port tool!"));
    hlpInfo.push_back(tr("2.Click contact author linkage at right corner for source code, or mail the author."));
    hlpInfo.push_back(tr("3.Input :show extra or :se for extra features, and :hide extra or :he to hide them."));

    foreach(auto elem, hlpInfo)
        plntxtOutput->appendPlainText(elem);
}

void MainWindow::showTime(void)
{
    labelTimeDisp->setText(datetime->currentDateTime().toString("yyyy/MM/dd  HH:mm:ss"));
}

void MainWindow::portMonitor(void)
{
    QStringList oldComNameList = comNameList;
    QStringList newComNameList;

    const auto infos = QSerialPortInfo::availablePorts();
    comNameList.clear();
    for(auto &info : infos) comNameList.push_back(info.portName());
    newComNameList = comNameList;

    if(oldComNameList.size() < newComNameList.size())
    {
        QString selectComName = choosecoms->currentText();

        foreach(auto s, oldComNameList) newComNameList.removeOne(s);
        foreach(auto s, newComNameList) choosecoms->addItem(s, s);

        choosecoms->update();
    }
    else if(oldComNameList.size() > newComNameList.size())
    {
        qint32 tmpIndex = -1;

        if(-1 == newComNameList.indexOf(choosecoms->currentText()))
        {
            if(0 != choosecoms->count())
                choosecoms->setCurrentIndex(0);
        }

        foreach(auto s, newComNameList) oldComNameList.removeOne(s);

        foreach(auto s, oldComNameList)
        {
            tmpIndex = choosecoms->findText(s);
            if(-1 != tmpIndex)
                choosecoms->removeItem(tmpIndex);
        }

        choosecoms->update();
    }
}

void MainWindow::receiveSerialData(void)
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

void MainWindow::hotUpdateSettings(void)
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
    for(auto i = 0; i < EXTRA_ITEM_NUMBER; ++i)
    {
        exLeInputs.at(i)->setPalette(plt);
    }

    if(btnOpenClose->text() == tr("close"))
    {
        QString tmpConnectStatus = "connected to ";
        tmpConnectStatus += choosecoms->itemData(choosecoms->currentIndex()).toString();
        tmpConnectStatus += " @ ";
        if (6 == baudrates->currentIndex())
            tmpConnectStatus += QString::number(baudrates->currentText().toInt());
        else
            tmpConnectStatus += QString::number(baudrates->itemData(baudrates->currentIndex()).toInt());
        tmpConnectStatus += "bps";

        connectStatus->setText(tmpConnectStatus);
        connectStatus->setStyleSheet("color:green;font:12pt;border-style:outset;");
    }
    else
    {
        connectStatus->setText("disconnected");
        connectStatus->setStyleSheet("color:gray;font:12pt;border-style:outset;");
    }
}

void MainWindow::procPortChanged(void)
{
    if(!choosecoms->currentText().isEmpty())
    {
        this->closeSerialPort();
        this->hotUpdateSettings();
    }
}

void MainWindow::procSendhexStateChanged(void)
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

void MainWindow::procAutosendStateChanged(void)
{
    if(autosend->isChecked())
    {
        if("0" == leAutoSendInterval->text())
        {
            leAutoSendInterval->setText(tr("1"));
        }

        if("0" == leAutoSendCounter->text())
        {
            leAutoSendCounter->setText(tr("∞"));
        }

        leAutoSendInterval->setEnabled(false);
        leAutoSendCounter->setEnabled(false);

        autoSendTimer->setInterval(leAutoSendInterval->text().toInt());
        autoSendTimer->start();
        this->procAutosendTimerTimeout(); 
    }
    else
    {
        if("∞" == leAutoSendCounter->text())
        {
            leAutoSendCounter->setText("0");
        }

        leAutoSendInterval->setEnabled(true);
        leAutoSendCounter->setEnabled(true);

        autoSendTimer->stop();
    }
}

void MainWindow::procAutosendTimerTimeout(void)
{
    if(mySerialPort->isOpen() && !leInput->text().isEmpty())
    {
        this->procSendButtonClicked();

        if("1" == leAutoSendCounter->text() && !leAutoSendCounter->isEnabled())
        {
            autoSendTimer->stop();
            autosend->setChecked(false);
            leAutoSendInterval->setEnabled(true);
            leAutoSendCounter->setEnabled(true);
        }

        if("0" != leAutoSendCounter->text() && "∞" != leAutoSendCounter->text())
        {
            leAutoSendCounter->setText(QString::number(leAutoSendCounter->text().toInt() - 1));
        }
    }
}

void MainWindow::openSerialPort(void)
{
    if(mySerialPort->open(QIODevice::ReadWrite))
    {
        btnOpenClose->setText(tr("close"));
        btnSend->setEnabled(true);
        for(auto i = 0; i < EXTRA_ITEM_NUMBER; ++i)
        {
            exSendBtns[i]->setEnabled(true);
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot open now!"));
    }
}

void MainWindow::closeSerialPort(void)
{
    mySerialPort->close();
    btnOpenClose->setText(tr("open"));
    btnSend->setEnabled(false);
    for(auto i = 0; i < EXTRA_ITEM_NUMBER; ++i)
    {
        exSendBtns[i]->setEnabled(false);
    }
}
