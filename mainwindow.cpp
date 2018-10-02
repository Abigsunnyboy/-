#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpSocket_server = new QTcpSocket(this);
    tcpClient = new QTcpSocket(this);


    QString name = QString::fromLocal8Bit("网络通讯助手");
    QString data = "9.25";
    QString version = "1.0";
    QString aurthor = "zhaoliang";
    setWindowTitle(name+" "+data +" "+ version+" "+aurthor);
    setFixedHeight(660);
    setFixedWidth(900);
    initUI();


}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow:: initUI(){

    //通用设置
    QStringList ips = getLocalIP();
    ui->comboBox_UDPAdress->addItems(ips);
    ui->lineEdit_UDPPort->setText("6000");//udp默认设置

    ui->comboBox_TCPClientLocalAdress->addItems(ips);
    ui->comboBox_TCPClientRemoteAdress->addItem("127.0.0.1");//tcpclient 默认设置
    ui->comboBox_TCPClientRemoteAdress->addItem("192.168.184.1");//tcpclient 默认设置

    ui->lineEdit_clientRemotePort->setText("6000");

    ui->comboBox_TCPServerLocalAdress->addItems(ips);
    ui->lineEdit_TCPServerRemotePort->setText("6000");
    ui->tabWidget->setCurrentIndex(0);

    selectModule(ui->tabWidget->currentIndex());
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(selectModule(int)));

    ui->lineEdit_SendInterval->setText("6000");


    QRegExp num("[0-9]+$");//端口号只能为数字
    ui->lineEdit_UDPPort->setValidator(new QRegExpValidator(num));
    ui->lineEdit_TCPServerRemotePort->setValidator(new QRegExpValidator(num));
    ui->lineEdit_clientRemotePort->setValidator(new QRegExpValidator(num));
    ui->lineEdit_SendInterval->setValidator(new QRegExpValidator(num));
    //服务端设置




}

void MainWindow::selectModule(int index){

    // int module =  ui->tabWidget->currentIndex();
    switch (index) {
    case 0:
        udpServerset();
        qDebug()<<"1";
        break;
    case 1:
        tcpClientset();
          qDebug()<<"2";
        break;
    case 2:
        tcpServerset();
          qDebug()<<"3";

        break;
    default:
        break;
    }
}

void MainWindow::udpServerset(){
   // ui->statusBar->clearMessage();
}

void MainWindow::tcpClientset(){
    //客户端设置
    lableClient_remoteIP->setVisible(true);
    lableClient_remotePort->setVisible(true);
    lableSocketStatus_client->setVisible(true);

    ui->statusBar->addWidget(lableSocketStatus_client);
    ui->statusBar->addWidget(lableClient_remoteIP);
    ui->statusBar->addWidget(lableClient_remotePort);

    lableTCPstatus->setVisible(false);
    lableTCPIP->setVisible(false);
    lableTCPPort->setVisible(false);
    lableSocketStatus->setVisible(false);

    connect(tcpClient,SIGNAL(connected()),this,SLOT(onConnectedtoServer()));
    connect(tcpClient,SIGNAL(disconnected()),this,SLOT(onDisconnectedtoServer()));
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(ClientReadyread()));

}

void MainWindow::tcpServerset(){

    lableTCPstatus->setVisible(true);
    lableTCPIP->setVisible(true);
    lableTCPPort->setVisible(true);
    lableSocketStatus->setVisible(true);

    ui->statusBar->addWidget(lableTCPstatus);
    ui->statusBar->addWidget(lableTCPIP);
    ui->statusBar->addWidget(lableTCPPort);
    ui->statusBar->addWidget(lableSocketStatus);

    lableClient_remoteIP->setVisible(false);
    lableClient_remotePort->setVisible(false);
    lableSocketStatus_client->setVisible(false);
    tcpServer = new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this ,SLOT (onNewConnection()));

}

void MainWindow::on_pushButton_clearReceive_clicked()
{
    ui->textEdit_ReceiveData->clear();
}


void MainWindow::on_pushButton_Senddata_clicked()
{
    int module =  ui->tabWidget->currentIndex();
    QString message = ui->lineEdit_Sendedit->text();
    //    ui->lineEdit_Sendedit->clear();


    QByteArray str;
    str = message.toUtf8();

    if (ui->checkBox_SendHex->checkState())
    {
        qDebug()<<"hex";
    }
    if (ui->checkBox_TimerSend->isTristate())
    {
        qDebug()<<"timer";
    }
    switch (module) {
    case 0:
        qDebug()<<"udpServer send data";
        //      QUdpSocket qus;
        //      int PORT = ui->lineEdit_UDPPort->text().toInt();
        //      QString netadress = ui->comboBox_UDPAdress->currentText();
        //      //  qus.bind(QHostAddress("127.0.0.1"), PORT+1);
        //    for(int i=0; i<100; ++i)
        //    qus.writeDatagram(message, QHostAddress(netadress), PORT);
        break;
    case 1:
        //  tcpClientset();
        tcpClient->write(str);
        ui->textEdit_ReceiveData->setTextColor(QColor ("black"));
        ui->textEdit_ReceiveData->append(QString::fromLocal8Bit("客户端发送: ") +str);

        break;
    case 2:
        tcpServerset();

        tcpSocket_server->write(str);
        ui->textEdit_ReceiveData->setTextColor(QColor ("red"));
        ui->textEdit_ReceiveData->append(QString::fromLocal8Bit("服务端发送: ") +str);
        break;
    default:
        break;
    }
}



QStringList MainWindow:: getLocalIP(){

    //    QString hostName = QHostInfo::localHostName();
    //    QHostInfo hostName = QHostInfo::fromName(hostName);

    QStringList ips;
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, addrs) {
        QString ip = addr.toString();
        if (MainWindow::isIP(ip)) {
            ips << ip;
        }
    }
    //优先取192开头的IP,如果获取不到IP则取127.0.0.1
    QString ip = "127.0.0.1";
    foreach (QString str, ips) {
        if (str.startsWith("192.168.1") || str.startsWith("192")) {
            ip = str;
            ips.removeOne(ip);
            break;
        }
    }
    ips.insert(0,ip);//将192开头的设为默认IP

    qDebug()<< ips;
    return ips;
}


bool MainWindow::isIP(const QString &ip)//判断是否是IP地址
{
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}


//tcp 服务端程序
void MainWindow :: on_pushButton_TCPServerOpen_clicked(){//打开服务器


    if (ui->pushButton_TCPServerOpen->text()==QString::fromLocal8Bit("监听"))
    {
        QString IP = ui->comboBox_TCPServerLocalAdress->currentText();
        quint16 port = ui->lineEdit_TCPServerRemotePort->text().toInt();//取得服务器地址和端口号

        QHostAddress addr(IP);
        tcpServer->listen(addr,port);//监听

        ui->pushButton_TCPServerOpen->setText(QString::fromLocal8Bit("关闭服务器"));
        lableTCPstatus->setText(QString::fromLocal8Bit("服务器状态：正在监听..."));

        lableTCPIP->setText(QString::fromLocal8Bit("服务器地址：")+tcpServer->serverAddress().toString());
        lableTCPPort->setText(QString::fromLocal8Bit("服务器端口：")+QString::number(tcpServer->serverPort()));

    }
    else{
        ui->pushButton_TCPServerOpen->setText(QString::fromLocal8Bit("监听"));
        tcpServer->close();
        lableTCPstatus->setText(QString::fromLocal8Bit("服务器状态：服务器关闭"));
        lableTCPIP->setText(QString::fromLocal8Bit("服务器地址：")+tcpServer->serverAddress().toString());
        lableTCPPort->setText(QString::fromLocal8Bit("服务器端口：")+QString::number(tcpServer->serverPort()));
    }

}


void MainWindow::onNewConnection(){//新的连接建立后对应的槽函数
    tcpSocket_server = tcpServer->nextPendingConnection();
    connect(tcpSocket_server,SIGNAL(connected()),this, SLOT(onClientConnected()));
    onClientConnected();
    connect(tcpSocket_server,SIGNAL(disconnected()),this,SLOT(onClientDisConnected()));
    connect(tcpSocket_server,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,
            SLOT(onSocketStateChange(QAbstractSocket::SocketState socketstate)));
    onSocketStateChange(tcpSocket_server->state());
    connect(tcpSocket_server,SIGNAL(readyRead()),this,SLOT(onSocketReadyread()));
}

void MainWindow::onClientConnected(){
    ui->listWidget_clientInfo->addItem(tcpSocket_server->peerAddress().toString()+"::"+QString::number(tcpSocket_server->peerPort()));
    int n = ui->listWidget_clientInfo->count();
    ui->labCount->setText(QString::fromLocal8Bit("共 ")+QString::number(n) +QString::fromLocal8Bit(" 个连接"));
}



void MainWindow::onClientDisConnected()
{
    ui->listWidget_clientInfo->addItem(tcpSocket_server->peerAddress().toString()+"::"+QString::number(tcpSocket_server->peerPort()));
    int n = ui->listWidget_clientInfo->count();
    ui->labCount->setText(QString::fromLocal8Bit("共 ")+ n +QString::fromLocal8Bit(" 个连接"));
}



void MainWindow::onSocketStateChange(QAbstractSocket::SocketState  socketstate){

    switch (socketstate) {
    case  QAbstractSocket::HostLookupState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"HostLookupState");
        break;
    case  QAbstractSocket::UnconnectedState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"UnconnectedState");
        break;
    case  QAbstractSocket::ConnectingState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ConnectingState");
        break;
    case  QAbstractSocket::ConnectedState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ConnectedState");
        break;
    case  QAbstractSocket::BoundState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"BoundState");
        break;
    case  QAbstractSocket::ClosingState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ClosingState");
        break;
    case  QAbstractSocket::ListeningState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ListeningState");
        break;
    default:
        break;

    }
}


void MainWindow::displayError(QAbstractSocket::SocketError){

    QMessageBox::warning(this,"Warning","socket error",QMessageBox::Yes);
}

void MainWindow::onSocketReadyread(){
    QByteArray buff = tcpSocket_server->readAll();
    qDebug()<< buff;
    QString message =(QString::fromLocal8Bit("接收自客户端：")
                      +tcpSocket_server->peerAddress().toString()+"::"+QString::number(tcpSocket_server->peerPort())+":  "+buff ).toUtf8();
    ui->textEdit_ReceiveData->setTextColor(QColor ("blue"));
    ui->textEdit_ReceiveData->append(message);

    //    while (tcpSocket_server->canReadLine()) {
    //        ui->textEdit_ReceiveData->append(QString::fromLocal8Bit("接收自客户端：")
    //            +tcpSocket_server->peerAddress().toString()+"::"+QString::number(tcpSocket_server->peerPort())+tcpSocket_server->readLine());
    //    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
          qDebug()<<event;
}

//客户端代码
void MainWindow::onConnectedtoServer(){

   // qDebug()<<"";

    lableClient_remoteIP->setText(QString::fromLocal8Bit("远程服务器地址和端口：")+tcpClient->peerAddress().toString()
                                  +"::"+QString::number(tcpClient->peerPort()));
    lableSocketStatus_client->setText(QString::fromLocal8Bit("连接状态：已连接"));

}

void MainWindow::onDisconnectedtoServer(){
    // lableSocketStatus_client->setText(QString::fromLocal8Bit("连接状态：未连接"));
     lableClient_remoteIP->setText(QString::fromLocal8Bit("远程服务器地址和端口："));
     lableSocketStatus_client->setText(QString::fromLocal8Bit("连接状态：未连接"));
}
void MainWindow::ClientReadyread(){
    QByteArray buff = tcpClient->readAll();
    qDebug()<< buff;
    QString message =(QString::fromLocal8Bit("接收自服务器：")
                      +tcpClient->peerAddress().toString()+"::"+QString::number(tcpClient->peerPort())+":  "+buff ).toUtf8();
    //  ui->textEdit_ReceiveData->append("<font color=\"#0000FF\"> \message\ </font>");
    ui->textEdit_ReceiveData->setTextColor(QColor ("green"));
    ui->textEdit_ReceiveData->append(message);
}


void MainWindow::on_pushButton_TCPClientLink_clicked()//连接或者断开客户端连接
{
    QString addr = ui->comboBox_TCPClientRemoteAdress->currentText();
    quint16 port = ui->lineEdit_TCPServerRemotePort->text().toUInt();

    if(tcpClient->state()==QAbstractSocket::UnconnectedState &&ui->pushButton_TCPClientLink->text() == QString::fromLocal8Bit("连接") ){
       tcpClient->connectToHost(addr,port);
       ui->pushButton_TCPClientLink->setText(QString::fromLocal8Bit("断开"));
        // qDebug()<<tcpClient->state();
    }

    else
    {
        ui->pushButton_TCPClientLink->setText(QString::fromLocal8Bit("连接"));
        tcpClient->disconnectFromHost();
        //  qDebug()<<tcpClient->state();
    }
}

