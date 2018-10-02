#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QtNetwork>
namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initUI();
    void udpServerset();
    void tcpClientset();
    void tcpServerset();

    static QStringList getLocalIP();
    static bool isIP(const QString &ip);


  //服务器端设置
    QUdpSocket *udpSocket;
    QTimer *timer;

    QTcpSocket *tcpSocket_server;
    QTcpServer *tcpServer;


    QLabel *lableTCPstatus = new QLabel(this);

    QLabel *lableTCPIP = new QLabel(this);

    QLabel *lableTCPPort = new QLabel(this);

    QLabel *lableSocketStatus = new  QLabel(this);

  //客户端设置
    QTcpSocket *tcpClient;
    QLabel* lableSocketStatus_client = new QLabel(this);

    QLabel *lableClient_remoteIP= new QLabel(this);
    QLabel *lableClient_remotePort= new QLabel(this);
   // QLabel *lableClient= new QLabel(this);



private slots:

    //tcp槽函数
    void on_pushButton_TCPServerOpen_clicked();

    void displayError(QAbstractSocket::SocketError);


    void onNewConnection();
    void onClientConnected();
    void onClientDisConnected();
    void onSocketStateChange(QAbstractSocket::SocketState  socketstate);
    void onSocketReadyread();

//客户端槽函数设置
    void onConnectedtoServer();
    void onDisconnectedtoServer();
    void ClientReadyread();
    void on_pushButton_TCPClientLink_clicked();


//通用槽函数
    void selectModule(int index);
    void on_pushButton_Senddata_clicked();
    void on_pushButton_clearReceive_clicked();

private:
    Ui::MainWindow *ui;


protected:

    void closeEvent(QCloseEvent *event);
signals:

//    void sendData(const QString &ip, int port, const QString &data);
//    void receiveData(const QString &ip, int port, const QString &data);

};



#endif // MAINWINDOW_H


