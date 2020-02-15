#include "initsurface.h"
#include "ui_initsurface.h"

InitSurface::InitSurface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InitSurface)
{
    ui->setupUi(this);
    tcpServerConnect();
}

InitSurface::~InitSurface()
{
    delete ui;
}

void InitSurface::tcpServerConnect()
{
    //实例化socket
    tcpsocket = new QTcpSocket(this);
    //断开现有连接
    tcpsocket->abort();
    //连接到本地的7777端口
    tcpsocket->connectToHost(QHostAddress::LocalHost,7777);
    //有可读信息，发送readyRead()
    connect(tcpsocket,SIGNAL(readyRead()),
            this,SLOT(readMessage()));
}

void InitSurface::readMessage()
{
    //实例化套接字的输入流，读信息
    QDataStream in(tcpsocket);
    in.setVersion(QDataStream::Qt_5_14);
    qDebug() << "InitSurface::readMessage:读取消息";
    QString greeting;
    //将信息写入greeting读取信息
    in >> greeting;
    //字符串分割
    QStringList msg = greeting.split("|");
    if(msg[0] == "initSurface"){
        accountTemp = msg[1];
        nameTemp = msg[2];
        signTemp = msg[3];
        headTemp = msg[4];
        if(myAccount == accountTemp){
            qDebug() << "InitSurface::readMessage:" << greeting;
            qDebug() << "initSurface";
            qDebug() << accountTemp;
            qDebug() << nameTemp;
            qDebug() << signTemp;
            qDebug() << headTemp;
            qDebug() << "进入主界面";
            //实例化主界面
            selectUser = new SelectUserInterface();
            selectUser->show();
            selectUser->setWindowTitle(accountTemp);
            /*selectUser->setInitSurface(accountTemp,nameTemp,
                                       signTemp,headTemp);*/
            connect(this,SIGNAL(sendInitSurface(QString,QString,QString,QString)),
                    selectUser,SLOT(getInitSurface(QString,QString,QString,QString)));
            emit sendInitSurface(accountTemp,nameTemp,
                                 signTemp,headTemp);
            disconnect(this,SIGNAL(sendInitSurface(QString,QString,QString,QString)),
                       selectUser,SLOT(getAccount(QString)));
            emit sendClose(true);
        }

    }else if(msg[0] == "register"){
        accountTemp = msg[1];
        qDebug() << "register" << accountTemp;
        qDebug() << accountTemp;
    }
    msg.clear();
    qDebug() << "InitSurface::readMessage:读取完成";
}

QString InitSurface::getAccount(QString account)
{
    qDebug() << "InitSurface::getAccount" <<account;
    myAccount = account;
    //发送
    sendMessage("initSurface");
    return account;
}

void InitSurface::setAccount(QString account)
{
    qDebug() << "InitSurface::setAccount" <<account;
    myAccount = account;
    //发送
    sendMessage("initSurface");
}

void InitSurface::sendMessage(QString Msg)
{
    qDebug() << "InitSurface::sendMessage:发送消息";
    if(Msg == "initSurface"){
        //初始化界面
        QString string = "initSurface|" + myAccount;
        QByteArray message;
        //以只读打开QByteArray，并设置版本，服务端客户端要一致
        QDataStream out(&message,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_14);
        //写入输出流
        out << string;
        qDebug() << "InitSurface::sendMessage:" << string;
        //发送信息
        tcpsocket->write(message);
    }else if(Msg == "register"){
        //注册
        qDebug() << "InitSurface::sendMessage:" << "register";
    }
    qDebug() << "InitSurface::sendMessage:发送完成";

}
