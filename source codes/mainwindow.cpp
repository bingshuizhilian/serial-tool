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
    auto labelAuthorInfo = new QLabel;
    labelAuthorInfo->setStatusTip(tr("click to view my github for source code, or mail to bingshuizhilian@yeah.net to contact me"));
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
    theme->addItem(QStringLiteral("light"));
    theme->addItem(QStringLiteral("dark"));
    connect(theme,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &hotUpdateSettings);
    auto labelTheme = new QLabel(tr("Theme"));
    layoutPortGroupBox->addWidget(labelTheme, 6, 0);
    layoutPortGroupBox->addWidget(theme, 6, 1);

    //serial port setting related layout
    auto comSettingGroupBox = new QGroupBox;
    comSettingGroupBox->setTitle(tr("port settings"));
    comSettingGroupBox->setLayout(layoutPortGroupBox);

    //edit and display
    auto layoutEditGroupBox = new QGridLayout;
    showhex->setStatusTip(tr("display the following receive contents with hex decoded"));
    layoutEditGroupBox->addWidget(showhex, 0, 0);
    connect(sendhex, &sendhex->stateChanged, this, &procSendhexStateChanged);
    sendhex->setStatusTip(tr("send hex coded contents"));
    layoutEditGroupBox->addWidget(sendhex, 0, 1);
    send0D->setStatusTip(tr("add 0x0D at the end of your contents automatically, even if it's empty"));
    layoutEditGroupBox->addWidget(send0D, 1, 0);
    send0A->setStatusTip(tr("add 0x0A at the end of your contents automatically, even if it's empty"));
    layoutEditGroupBox->addWidget(send0A, 1, 1);

    auto editSettingGroupBox = new QGroupBox;
    editSettingGroupBox->setTitle(tr("edit settings"));
    editSettingGroupBox->setLayout(layoutEditGroupBox);

    //auto send
    auto layoutAutoSendGroupBox = new QGridLayout;
    autosend->setStatusTip(tr("enable or disable auto send"));
    layoutAutoSendGroupBox->addWidget(autosend, 0, 0);
    connect(autosend, &autosend->stateChanged, this, &procAutosendStateChanged);
    leAutoSendInterval->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    leAutoSendInterval->setFixedWidth(30);
    layoutAutoSendGroupBox->addWidget(leAutoSendInterval, 0, 1);
    leAutoSendInterval->setValidator(new QIntValidator(1, 1000*60*60*24));
    leAutoSendInterval->setText("1000");
    auto labelAutoSendInterval = new QLabel(tr("ms"));
    labelAutoSendInterval->setStatusTip(tr("set send frequency, range 1ms-1day"));
    layoutAutoSendGroupBox->addWidget(labelAutoSendInterval, 0, 2);
    leAutoSendCounter->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    leAutoSendCounter->setFixedWidth(30);
    layoutAutoSendGroupBox->addWidget(leAutoSendCounter, 0, 3);
    leAutoSendCounter->setValidator(new QIntValidator(0, 99999999));
    leAutoSendCounter->setText("0");
    auto labelAutoSendCounter = new QLabel(tr("times"));
    labelAutoSendCounter->setStatusTip(tr("set how many times you want to send, range 0-99999999, 0 for infinate"));
    layoutAutoSendGroupBox->addWidget(labelAutoSendCounter, 0, 4);

    auto autoSendGroupBox = new QGroupBox;
    autoSendGroupBox->setTitle(tr("auto send"));
    autoSendGroupBox->setLayout(layoutAutoSendGroupBox);

    //buttons
    auto layoutButtons = new QGridLayout;
    QSize btnSize(50, 30);
    btnOpenClose->setFixedSize(btnSize);
    btnOpenClose->setStatusTip(tr("open or close the selected serial port"));
    connect(btnOpenClose, &btnOpenClose->clicked, this, &procOpenCloseButtonClicked);
    layoutButtons->addWidget(btnOpenClose, 0, 0);
    btnSave->setFixedSize(btnSize);
    btnSave->setStatusTip(tr("save the current display contents to a timestamp-named file under the tool's path"));
    connect(btnSave, &btnSave->clicked, this, &procSaveButtonClicked);
    layoutButtons->addWidget(btnSave, 0, 1);
    btnClrScrn->setFixedSize(btnSize);
    btnClrScrn->setStatusTip(tr("clear the display contents and send bytes counter and receive bytes counter"));
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
    leInput->setStatusTip(tr("press enter with input :? to view more"));
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
    theme = new QComboBox;
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
    {
        QMessageBox::warning(this, "Warnning", "Cannot open " + fileName, QMessageBox::Yes);
        return;
    }

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

    QList<QPair<CmdType, QStringList>> cmdList;
    cmdList.push_back({ CMD_HELP, {":help", ":hlp", ":?"} });
    cmdList.push_back({ CMD_SHOW, {":show extra", ":se"} });
    cmdList.push_back({ CMD_HIDE, {":hide extra", ":he"} });
    cmdList.push_back({ CMD_RESET, {":reset", ":rst"} });
    cmdList.push_back({ CMD_CLEAR_INPUT, {":clear input", ":ci"} });
    cmdList.push_back({ CMD_SAVE_CONFIG_FILE, {":save config file", ":scf"} });
    cmdList.push_back({ CMD_LOAD_CONFIG_FILE, {":load config file", ":lcf"} });
    cmdList.push_back({ CMD_HYTERA_CUSTOMIZED, {":hytera customized", ":hc"} });
    cmdList.push_back({ CMD_HYTERA_CUSTOMIZED_AT, {":hytera at", ":ha"} });
    cmdList.push_back({ CMD_HYTERA_CUSTOMIZED_DIAL, {":hytera dial", ":hd"} });

    QString inputString = leInput->text();
    CmdType findCmd = CMD_NULL;
    foreach(auto pairElem, cmdList)
    {
        foreach(auto strListElem, pairElem.second)
        {
            if(!strListElem.compare(inputString, Qt::CaseInsensitive))
            {
                findCmd = pairElem.first;
                break;
            }
        }

        if(CMD_NULL != findCmd)
            break;
    }

    switch(findCmd)
    {
    case CMD_HELP:
    {
        showHelpInfo(findCmd);
        break;
    }
    case CMD_SHOW:
    {
        if(exInputGroup->isHidden())
        {
            exInputGroup->show();

            QSize size = this->size();
            size.setWidth(WINDOW_ORIGNAL_WIDTH + WINDOW_EXTRA_WIDTH);
            this->resize(size);
        }
        break;
    }
    case CMD_HIDE:
    {
        if(!exInputGroup->isHidden())
        {
            exInputGroup->hide();

            QSize size = this->size();
            size.setWidth(WINDOW_ORIGNAL_WIDTH);
            this->resize(size);
        }
        break;
    }
    case CMD_RESET:
    case CMD_CLEAR_INPUT:
    {
        procResetCmd(findCmd);
        break;
    }
    case CMD_SAVE_CONFIG_FILE:
    case CMD_LOAD_CONFIG_FILE:
    {
        procConfigFile(findCmd);
        break;
    }
    case CMD_HYTERA_CUSTOMIZED:
    case CMD_HYTERA_CUSTOMIZED_AT:
    case CMD_HYTERA_CUSTOMIZED_DIAL:
    {
        showHelpInfo(findCmd);
        break;
    }
    default:
        break;
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

    int totalEmptyInputNum = 0;
    for(auto i = 0; i < EXTRA_ITEM_NUMBER; ++i)
    {
        if(exLeInputs.at(i)->text().isEmpty())
        {
            ++totalEmptyInputNum;
        }
    }

    if(EXTRA_ITEM_NUMBER == totalEmptyInputNum)
    {
        return;
    }

    do
    {
        ++whichExContentIsToBeSent;
        whichExContentIsToBeSent %= EXTRA_ITEM_NUMBER;
    }while(exLeInputs.at(whichExContentIsToBeSent)->text().isEmpty());

//    qDebug() << "whichExContentIsToBeSent" << whichExContentIsToBeSent << "\n";

    if(!exLeInputs.at(whichExContentIsToBeSent)->text().isEmpty())
    {
        this->procExSendButtonClicked(whichExContentIsToBeSent);
    }
}

void MainWindow::procExHexButtonClicked(int btn_id)
{
//    qDebug() << "btn_id" << btd_id << "\n";
    exLeInputs.at(btn_id)->clear();

    if(exHexCheckBoxs.at(btn_id)->isChecked())
    {
        QRegExp hexCodeRegex("[a-fA-F\\d ]+$");
        auto validator = new QRegExpValidator(hexCodeRegex, exLeInputs.at(btn_id));
        exLeInputs.at(btn_id)->setValidator(validator);
    }
    else
    {
        exLeInputs.at(btn_id)->setValidator(0);
    }
}

void MainWindow::procExSendButtonClicked(int btn_id)
{
    QString inputString = exLeInputs.at(btn_id)->text();

    if(mySerialPort->isOpen())
    {
        if(exHexCheckBoxs.at(btn_id)->isChecked())
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

void MainWindow::procConfigFile(CmdType cmd)
{
    if(CMD_SAVE_CONFIG_FILE == cmd)
    {
        QString fileName = QDir::currentPath() + '/' + CONFIG_FILE_NAME;
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Warnning", "Cannot open " + fileName, QMessageBox::Yes);
            return;
        }

        QJsonObject jsonSettings;
        jsonSettings.insert("ComName", choosecoms->currentText());
        jsonSettings.insert("Baudrate", baudrates->currentText());
        jsonSettings.insert("DataBits", databits->currentText());
        jsonSettings.insert("Parity", parity->currentText());
        jsonSettings.insert("StopBits", stopbits->currentText());
        jsonSettings.insert("FlowCtrl", flowcontrol->currentText());
        jsonSettings.insert("Theme", theme->currentText());

        QJsonObject jsonEdit;
        jsonEdit.insert("Showhex", showhex->isChecked());
        jsonEdit.insert("Sendhex", sendhex->isChecked());
        jsonEdit.insert("Send0D", send0D->isChecked());
        jsonEdit.insert("Send0A", send0A->isChecked());

        QJsonArray jsonExtraHexBox, jsonExtraInput;
        for(auto i = 0; i < EXTRA_ITEM_NUMBER; ++i)
        {
            jsonExtraHexBox.append(exHexCheckBoxs.at(i)->isChecked());
            jsonExtraInput.append(exLeInputs.at(i)->text());
        }

        QJsonObject jsonConfig;
        jsonConfig.insert("Settings", jsonSettings);
        jsonConfig.insert("Edit", jsonEdit);
        jsonConfig.insert("ExtraHexBox", jsonExtraHexBox);
        jsonConfig.insert("ExtraInput", jsonExtraInput);

        QJsonDocument document;
        document.setObject(jsonConfig);
        QByteArray byte_array = document.toJson(QJsonDocument::Compact);
        QString jsonEncodedString(byte_array);

        QTextStream out(&file);
        out << jsonEncodedString;
        file.close();
    }
    else if(CMD_LOAD_CONFIG_FILE == cmd)
    {
        QString fileName = QDir::currentPath() + '/' + CONFIG_FILE_NAME;
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Warnning", "Cannot open " + fileName, QMessageBox::Yes);
            return;
        }

        QTextStream in(&file);
        QString jsonEncodedString = in.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument parseDoucment = QJsonDocument::fromJson(jsonEncodedString.toLocal8Bit(), &jsonError);
        if(QJsonParseError::NoError == jsonError.error)
        {
            if(parseDoucment.isObject())
            {
                QJsonObject docObj = parseDoucment.object();
                if(docObj.contains("Settings"))
                {
                    QJsonValue value = docObj.take("Settings");
                    if(value.isObject())
                    {
                        QJsonObject settingsObj = value.toObject();

                        if(settingsObj.contains("ComName"))
                        {
                            QJsonValue value = settingsObj.take("ComName");
                            if(value.isString())
                            {
                                choosecoms->setCurrentText(value.toString());
                            }
                        }

                        if(settingsObj.contains("Baudrate"))
                        {
                            QJsonValue value = settingsObj.take("Baudrate");
                            if(value.isString())
                            {
                                baudrates->setCurrentText(value.toString());
                            }
                        }

                        if(settingsObj.contains("DataBits"))
                        {
                            QJsonValue value = settingsObj.take("DataBits");
                            if(value.isString())
                            {
                                databits->setCurrentText(value.toString());
                            }
                        }

                        if(settingsObj.contains("Parity"))
                        {
                            QJsonValue value = settingsObj.take("Parity");
                            if(value.isString())
                            {
                                parity->setCurrentText(value.toString());
                            }
                        }

                        if(settingsObj.contains("StopBits"))
                        {
                            QJsonValue value = settingsObj.take("StopBits");
                            if(value.isString())
                            {
                                stopbits->setCurrentText(value.toString());
                            }
                        }

                        if(settingsObj.contains("FlowCtrl"))
                        {
                            QJsonValue value = settingsObj.take("FlowCtrl");
                            if(value.isString())
                            {
                                flowcontrol->setCurrentText(value.toString());
                            }
                        }

                        if(settingsObj.contains("Theme"))
                        {
                            QJsonValue value = settingsObj.take("Theme");
                            if(value.isString())
                            {
                                theme->setCurrentText(value.toString());
                            }
                        }
                    }
                }

                if(docObj.contains("Edit"))
                {
                    QJsonValue value = docObj.take("Edit");
                    if(value.isObject())
                    {
                        QJsonObject editObj = value.toObject();

                        if(editObj.contains("Showhex"))
                        {
                            QJsonValue value = editObj.take("Showhex");
                            if(value.isBool())
                            {
                                showhex->setChecked(value.toBool());
                            }
                        }

                        if(editObj.contains("Sendhex"))
                        {
                            QJsonValue value = editObj.take("Sendhex");
                            if(value.isBool())
                            {
                                sendhex->setChecked(value.toBool());
                            }
                        }

                        if(editObj.contains("Send0D"))
                        {
                            QJsonValue value = editObj.take("Send0D");
                            if(value.isBool())
                            {
                                send0D->setChecked(value.toBool());
                            }
                        }

                        if(editObj.contains("Send0A"))
                        {
                            QJsonValue value = editObj.take("Send0A");
                            if(value.isBool())
                            {
                                send0A->setChecked(value.toBool());
                            }
                        }
                    }
                }

                if(docObj.contains("ExtraHexBox"))
                {
                    QJsonValue value = docObj.take("ExtraHexBox");
                    if(value.isArray())
                    {
                        QJsonArray exHexBoxArray = value.toArray();
                        for(auto i = 0; i < EXTRA_ITEM_NUMBER && i < exHexBoxArray.size(); ++i)
                        {
                            if(exHexBoxArray.at(i).isBool())
                            {
                                exHexCheckBoxs.at(i)->setChecked(exHexBoxArray.at(i).toBool());
                                if(exHexCheckBoxs.at(i)->isChecked())
                                {
                                    QRegExp hexCodeRegex("[a-fA-F\\d ]+$");
                                    auto validator = new QRegExpValidator(hexCodeRegex, exLeInputs.at(i));

                                    exLeInputs.at(i)->setValidator(validator);
                                }
                            }
                        }
                    }
                }

                if(docObj.contains("ExtraInput"))
                {
                    QJsonValue value = docObj.take("ExtraInput");
                    if(value.isArray())
                    {
                        QJsonArray exInputArray = value.toArray();
                        for(auto i = 0; i < EXTRA_ITEM_NUMBER && i < exInputArray.size(); ++i)
                        {
                            if(exInputArray.at(i).isString())
                            {
                                QString input = exInputArray.at(i).toString();
                                if(exHexCheckBoxs.at(i)->isChecked())
                                {
                                    QRegExpValidator validator(QRegExp("[a-fA-F\\d ]+$"));
                                    int pos = 0;
                                    if(QValidator::Invalid == validator.validate(input, pos))
                                    {
                                        input.clear();
                                    }
                                }

                                exLeInputs.at(i)->setText(input);
                            }
                        }
                    }
                }
            }
        }

        this->hotUpdateSettings();
    }
}

void MainWindow::showHelpInfo(CmdType cmd)
{
    QStringList hlpInfo;

    if(CMD_HELP == cmd)
    {
        hlpInfo.push_back(tr("0.Welcome to use and spread this open source serial port tool."));
        hlpInfo.push_back(tr("1.This tool is developed under Qt creator using QT5 in C++, thanks for QT's easy-use."));
        hlpInfo.push_back(tr("2.Input <u>:show extra</u> or <u>:se</u> for extra features, and <u>:hide extra</u> or <u>:he</u> to hide them."));
        hlpInfo.push_back(tr("3.Input <u>:save config file</u> or <u>:scf</u> to save config file, and <u>:load config file</u> or <u>:lcf</u> to load config file, "
                             "take care that the tool will not save or load config file automatically."));
        hlpInfo.push_back(tr("4.Input <u>:reset</u> or <u>:rst</u> to reset the tool, and <u>:clear input</u> or <u>:ci</u> to clear extra input area."));
        hlpInfo.push_back(tr("5.Any good idea to improve this tool, click contact author."));
    }
    else if(CMD_HYTERA_CUSTOMIZED == cmd)
    {
        hlpInfo.push_back(tr("Welcome to Hytera's customized page.\n"));
        hlpInfo.push_back(tr("0.Input <u>:hytera at</u> or <u>:ha</u> to view some useful AT commands for Tetra G1.5 terminal development."));
        hlpInfo.push_back(tr("1.Input <u>:hytera dial</u> or <u>:hd</u> to view dial-based commands for Tetra G1.5 terminal development."));
    }
    else if(CMD_HYTERA_CUSTOMIZED_AT == cmd)
    {
        hlpInfo.push_back(tr("This page holds some of Hytera Tetra G1.5 radio's AT command example, it's designed for Hytera's engigeers."));
        hlpInfo.push_back(tr("\n<0> Group Manage"));
        hlpInfo.push_back(tr("  |-<0.1> DGNA - single assign / deassign"));
        hlpInfo.push_back(tr("        |-> AT+DGNA=0,0,1,500001,1,3,0"));
        hlpInfo.push_back(tr("        |-> AT+DGNA=1,0,1,500001"));
        hlpInfo.push_back(tr("  |-<0.2> DGNA - multi assign / deassign"));
        hlpInfo.push_back(tr("        |-> AT+DGNA=0,0,2,500001,1,3,0,500002,1,3,0"));
        hlpInfo.push_back(tr("        |-> AT+DGNA=1,0,2,500001,500002"));
        hlpInfo.push_back(tr("\n<1> Others"));
        hlpInfo.push_back(tr("  |-<1.1> LOG - on / off"));
        hlpInfo.push_back(tr("        |-> AT+LOG=1"));
        hlpInfo.push_back(tr("        |-> AT+LOG=0"));
    }
    else if(CMD_HYTERA_CUSTOMIZED_DIAL == cmd)
    {
        hlpInfo.push_back(tr("This page holds some of Hytera Tetra G1.5 radio's dial-based command list, it's designed for Hytera's engigeers."));

        hlpInfo.push_back(tr("\nFor PT580H series full keypad model, command list is provided as below. - PT580H系列全键盘机型支持的拨号指令列表."));
        hlpInfo.push_back(tr("<0> *#1# :MS start to output RT trace data - 输出实时TRACE数据"));
        hlpInfo.push_back(tr("<1> *#2# :MS stop to output RT trace data - 停止输出实时TRACE数据"));
        hlpInfo.push_back(tr("<2> *#4# :Freeze RTtrace Buffer - 暂停RTtrace Buffer数据存储"));
        hlpInfo.push_back(tr("<3> *#5# :Resume RTtrace Buffer - 恢复RTtrace Buffer数据存储"));
        hlpInfo.push_back(tr("<4> *#6# :Clear RTtrace Buffer - 清除RTtrace Buffer数据存储"));
        hlpInfo.push_back(tr("<5> *#7# :catch trace include checksum with stack power off - 关闭协议栈抓取含校验和的TRACE"));
        hlpInfo.push_back(tr("<6> *#8# :Get Task_RTtrace Info - 获取TRACE任务信息"));
        hlpInfo.push_back(tr("<7> *#00# :delete encrypt key directly - 紧急删除加密密钥"));
        hlpInfo.push_back(tr("<8> *#05# :show cell information - 小区信息"));
        hlpInfo.push_back(tr("<9> *#06# :show version information - 版本信息"));
        hlpInfo.push_back(tr("<10> *#07# :set MMI always on mode flag true - 上层调试接口"));
        hlpInfo.push_back(tr("<11> *#08# :set MMI always on mode flag false - 上层调试接口"));
        hlpInfo.push_back(tr("<12> *#73# :tetra packet data debug - TETRA分组数据调试"));
        hlpInfo.push_back(tr("<13> *#90# :Resume Task_RTtrace - 若TRACE任务处于挂起状态，恢复其至运行状态"));
        hlpInfo.push_back(tr("<14> *#91# :Enable RT Trace function, MS will respond RT Trace CMD - 若TRACE任务未处于挂起状态，开启终端响应实时TRACE指令的功能"));
        hlpInfo.push_back(tr("<15> *#92# :Disable RT Trace function, MS will respond RT Trace CMD - 若TRACE任务未处于挂起状态，关闭终端响应实时TRACE指令的功能"));
        hlpInfo.push_back(tr("<16> *#93# :Suspend Task_RTtrace - 若TRACE任务未处于挂起状态，切换其至挂起状态"));
        hlpInfo.push_back(tr("<17> *2222# :enter engineering mode - 工程模式"));
        hlpInfo.push_back(tr("<18> *2223# :enter degradation mode - 退化模式"));
        hlpInfo.push_back(tr("<19> *4224# :offlineLog_Clean(messageOnFlash) - 底层调试接口"));
        hlpInfo.push_back(tr("<20> *10080# :BSP_EnableMaxqSimFunc() - 底层调试接口"));
        hlpInfo.push_back(tr("<21> *10081# :BSP_DisableMaxqSimFunc() - 底层调试接口"));
        hlpInfo.push_back(tr("<22> *10086# :BSP_SetMaxqProg - 底层调试接口"));
        hlpInfo.push_back(tr("<23> *10087# :BSP_SetMaxqRst - 底层调试接口"));
        hlpInfo.push_back(tr("<24> *10088# :BSP_1850Switch(1) - 底层调试接口"));
        hlpInfo.push_back(tr("<25> *10089# :BSP_1850Switch(0) - 底层调试接口"));
        hlpInfo.push_back(tr("<26> *10097# :OTAK_MngKey_PrintKEY() - OTAK调试接口"));
        hlpInfo.push_back(tr("<27> *10098# :OTAK_TEK_PrintKEY() - OTAK调试接口"));
        hlpInfo.push_back(tr("<28> *10099# :OTAK_AssTab_PrintAssTabElmt() - OTAK调试接口"));
        hlpInfo.push_back(tr("<29> *222*07# :catch trace with stack power off - 关闭协议栈抓取TRACE"));
        hlpInfo.push_back(tr("<30> *222*08# :set MMI auto test flag true - 设置MMI自动测试标志为真"));
        hlpInfo.push_back(tr("<31> *222*09# :catch trace with stack suspend - 暂停协议栈并抓取TRACE"));
        hlpInfo.push_back(tr("<32> *222*10# :reverse the state of MMI watch dog timer log switch - 翻转MMI看门狗定时器打印开关"));
        hlpInfo.push_back(tr("<33> *222*11# :power save mode - 底层调试接口"));
        hlpInfo.push_back(tr("<34> *222*21# :BSP_Debug1() - 底层调试接口"));
        hlpInfo.push_back(tr("<35> *222*22# :BSP_Debug2() - 底层调试接口"));
        hlpInfo.push_back(tr("<36> *222*23# :BSP_Debug3() - 底层调试接口"));
        hlpInfo.push_back(tr("<37> *222*24# :BSP_Debug4() - 底层调试接口"));
        hlpInfo.push_back(tr("<38> *222*25# :BSP_Debug5() - 底层调试接口"));
        hlpInfo.push_back(tr("<39> *222*26# :BSP_Debug6() - 底层调试接口"));
        hlpInfo.push_back(tr("<40> *222*27# :catch trace include checksum with stack power off - 关闭协议栈抓取含校验和的TRACE"));
        hlpInfo.push_back(tr("<41> *222*28# :BSP_Debug7() - 底层调试接口"));
        hlpInfo.push_back(tr("<42> *333*10# :BSP_Debug8() - 底层调试接口"));
        hlpInfo.push_back(tr("<43> *333*20# :BSP_Debug9() - 底层调试接口"));
        hlpInfo.push_back(tr("<44> *66330# :delete model number file for debug - 调试用删除MN码存储文件"));
        hlpInfo.push_back(tr("<45> *66335# :display model number for debug - 调试用显示MN码"));
        hlpInfo.push_back(tr("<46> *888*25# :BSP_Dsplog_Set_group1() - 底层调试接口"));
        hlpInfo.push_back(tr("<47> *888*26# :BSP_Dsplog_Set_group2() - 底层调试接口"));
        hlpInfo.push_back(tr("<48> *888*27# :BSP_Dsplog_Set_group3() - 底层调试接口"));
        hlpInfo.push_back(tr("<49> *888*28# :BSP_Dsplog_Set_group4() - 底层调试接口"));
        hlpInfo.push_back(tr("<50> *888*29# :DspLog_Show() - 底层调试接口"));
        hlpInfo.push_back(tr("<51> *888*30# :offlineLog_Clean(messageOnFlash) - 底层调试接口"));
        hlpInfo.push_back(tr("<52> *888*31# :offlineLog_flashSave(OLL_TYPE_MANUAL) - 底层调试接口"));
        hlpInfo.push_back(tr("<53> *888*32# :offlineLog_showOops(messageOnFlash) - 底层调试接口"));
        hlpInfo.push_back(tr("<54> *888*33# :offlineLog_showOops(messageOnRam) - 底层调试接口"));
        hlpInfo.push_back(tr("<55> *888*34# :offlineLog_timeStreamLog(messageOnRam) - 底层调试接口"));
        hlpInfo.push_back(tr("<56> *888*35# :offlineLog_showUserlog(messageOnRam,ch) - 底层调试接口"));
        hlpInfo.push_back(tr("<57> *888*36# :offlineLog_showInstantTaskLog() - 底层调试接口"));
        hlpInfo.push_back(tr("<58> *888*37# :offlineLog_onOff(offlineLog_on) - 底层调试接口"));
        hlpInfo.push_back(tr("<59> *888*38# :offlineLog_onOff(offlineLog_off) - 底层调试接口"));
        hlpInfo.push_back(tr("<60> *888*39# :close trace mode - 关闭TRACE模式"));
        hlpInfo.push_back(tr("<61> *888*40# :open trace mode - 开启TRACE模式"));
        hlpInfo.push_back(tr("<62> *888*55# :get dsp type, if support E2EE, do delete key - 获取DSP类型，若支持E2EE加密则删除密钥"));
        hlpInfo.push_back(tr("<63> *888*99# :debugSystem_startUp() - 底层调试接口"));
        hlpInfo.push_back(tr("<64> *#999*101# :restart terminal and enter bt firmware update mode - 重启并进入蓝牙固件升级模式"));

        hlpInfo.push_back(tr("\nFor PT560H series limit keypad model, command list is provided as below(H is short for hang up key, 1/2/3 is short for F1/F2/F3 key)."
                             " - PT560H系列简化键盘机型支持的拨号指令列表(H代表挂机键，1/2/3代表F1/F2/F3编程键)."));
        hlpInfo.push_back(tr("<0> H112H :enter function switch menu - 功能开关菜单"));
        hlpInfo.push_back(tr("<1> H113H :enter tp debug menu - TP调试菜单"));
        hlpInfo.push_back(tr("<2> H121H :enter engineering mode menu - 工程模式菜单"));
        hlpInfo.push_back(tr("<3> H122H :enter realtime trace menu - 实时TRACE菜单"));
        hlpInfo.push_back(tr("<4> H133H :enter dsp debug menu - DSP调试菜单"));
        hlpInfo.push_back(tr("<5> H223H :enter bsp debug menu - BSP调试菜单"));
        hlpInfo.push_back(tr("<6> H233H :enter offline log menu - 离线日志菜单"));
        hlpInfo.push_back(tr("<7> H1122H :show version information - 版本信息"));
        hlpInfo.push_back(tr("<8> H1133H :enter engineering mode - 工程模式"));
        hlpInfo.push_back(tr("<9> H1232H :delete encrypt key directly - 紧急删除加密密钥"));
        hlpInfo.push_back(tr("<10> H2211H :show cell information - 小区信息"));
    }

    plntxtOutput->clear();
    foreach(auto elem, hlpInfo)
    {
        if(CMD_HELP == cmd || CMD_HYTERA_CUSTOMIZED == cmd)
        {
            plntxtOutput->appendHtml(elem);
        }
        else
        {
            plntxtOutput->appendPlainText(elem);
        }
    }

    QTextCursor cursor = plntxtOutput->textCursor();
    cursor.movePosition(QTextCursor::Start);
    plntxtOutput->setTextCursor(cursor);
}

void MainWindow::procResetCmd(CmdType cmd)
{
    if(CMD_CLEAR_INPUT == cmd || CMD_RESET == cmd)
    {
        for(auto i = 0; i < EXTRA_ITEM_NUMBER; ++i)
        {
            exHexCheckBoxs.at(i)->setChecked(false);
            exLeInputs.at(i)->clear();
        }
    }

    if(CMD_RESET == cmd)
    {
        choosecoms->setCurrentIndex(0);
        baudrates->setCurrentIndex(4);
        databits->setCurrentIndex(3);
        parity->setCurrentIndex(0);
        stopbits->setCurrentIndex(0);
        flowcontrol->setCurrentIndex(0);
        theme->setCurrentIndex(0);
        showhex->setChecked(false);
        sendhex->setChecked(false);
        send0D->setChecked(false);
        send0A->setChecked(false);
        autosend->setChecked(false);
        leAutoSendInterval->setText("1000");
        leAutoSendCounter->setText("0");
        plntxtOutput->clear();
        leInput->clear();

        this->hotUpdateSettings();
    }
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
    switch(theme->currentIndex())
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

        if(!leInput->statusTip().isEmpty())
            leInput->setStatusTip(tr(""));
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
        QRegExp hexCodeRegex("[a-fA-F\\d ]+$");
        auto validator = new QRegExpValidator(hexCodeRegex, leInput);
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
