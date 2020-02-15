#include "findpassword.h"
#include "ui_findpassword.h"

#include <QTcpSocket>

FindPassword::FindPassword(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindPassword)
{
    ui->setupUi(this);
}

FindPassword::~FindPassword()
{
    delete ui;
}


bool FindPassword::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == this){
        if(event->type() == QCloseEvent::Close){
            qDebug() << "关闭找回页面";
            emit sendFindClose(true);
        }else
            return false;
    }
    return QWidget::eventFilter(obj,event);
}

void FindPassword::on_commit_clicked()
{
    myAccount = ui->accout_line->text();
    if(myAccount.isEmpty()){
        ui->head_label->setText("请输入账号");
    }else{
        tcpServerConnect();
        sendMessage("findpassword");
    }
}

void FindPassword::tcpServerConnect()
{
    //实例化socket
    tcpsocket = new QTcpSocket(this);
    //断开现有连接
    tcpsocket->abort();
    //连接到本地的7777端口
    tcpsocket->connectToHost("127.0.0.1", 7777);
    //有可读信息，发送readyRead()
    connect(tcpsocket,SIGNAL(readyRead()),
            this,SLOT(readMessage()));
}

void FindPassword::sendMessage(QString Msg)
{
    qDebug() << "FindPassword::sendMessage:发送消息";
    if(Msg == "findpassword"){
        //初始化界面
        QString string = "findpassword|" + myAccount;
        QByteArray message;
        //以只读打开QByteArray，并设置版本，服务端客户端要一致
        QDataStream out(&message,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_14);
        //写入输出流
        out << string;
        qDebug() << "FindPassword::sendMessage:" << string;
        //发送信息
        tcpsocket->write(message);
    }
    qDebug() << "FindPassword::sendMessage:发送完成";
}

void FindPassword::readMessage()
{
    //实例化套接字的输入流，读信息
    QDataStream in(tcpsocket);
    in.setVersion(QDataStream::Qt_5_4);
    qDebug() << "FindPassword::readMessage:读取消息";
    QString greeting;
    QString account;
    QString password;
    //将信息写入greeting读取信息
    in >> greeting;
    //读取的信息
    qDebug() << "FindPassword::readMessage:" << greeting;
    //字符串分割
    QStringList msg = greeting.split("|");
    if(msg[0] == "findpassword"){
        account = msg[1];
        password = msg[2];
        if(myAccount == account){
            qDebug() << "findpassword";
            qDebug() << account;
            qDebug() << password;
            qDebug() << "找回成功";
            ui->accout_line->setText(account);
            ui->password_line->setText(password);
        }

    }else{
        qDebug() << "找回失败";
    }
    qDebug() << "FindPassword::readMessage:读取完成";
}



