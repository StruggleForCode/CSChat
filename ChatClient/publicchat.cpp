#include "publicchat.h"
#include "ui_publicchat.h"

#include <QResizeEvent>
#include <QCloseEvent>
#include <QtNetwork>
#include <QTcpSocket>
#include <QStringList>

PublicChat::PublicChat(QString title) :
    ui(new Ui::PublicChat)
{
    ui->setupUi(this);
    QPixmap pix;
    userTitle = title;
    setWindowTitle(title);
    pix.load(":image/img/head.png");
    pix = pix.scaled(QSize(80,80));
    ui->headLabel->setPixmap(pix);
    ui->headLabel->resize(QSize(80,80));
    ui->headLabel->setToolTip("头像");

    ui->userNameLabel->setText(userTitle);
    ui->sendMsgLine->installEventFilter(this);

}

PublicChat::~PublicChat()
{
    delete ui;
}

bool PublicChat::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(event);
    if(obj == ui->sendMsgLine){
        if(event->type() == QEvent::KeyPress){
            QKeyEvent *press = static_cast<QKeyEvent *>(event);
            if(press->key() == Qt::Key_Return){
               sendMessage();
               return true;
            }
        }else
            return false;
    }if(obj == this){
        if(event->type() == QEvent::Close){
            //发送给父窗口
           // emit sendAccount(myAccount);
            return true;
        }else
            return false;
    }else
        return QWidget::eventFilter(obj,event);
    return QWidget::eventFilter(obj,event);
}

void PublicChat::getAccount(QString user1)
{
    myAccount = user1;
    qDebug() << "发送方: " << myAccount;
    //连接服务器
    tcpServerConnect();
}

void PublicChat::tcpServerConnect()
{
    //实例化socket
    tcpsocket = new QTcpSocket(this);
    //断开现有连接
    tcpsocket->abort();
    //连接到本地的8888端口
    tcpsocket->connectToHost("127.0.0.1",8888);
    //有可读信息，发送readyRead()
    connect(tcpsocket,SIGNAL(readyRead()),
            this,SLOT(readMessage()));
    init();
}

void PublicChat::readMessage(){
    //实例化套接字的输入流，读信息
    QDataStream in(tcpsocket);
    in.setVersion(QDataStream::Qt_5_14);
    QString greeting;
    //将信息写入greeting读取信息
    in >> greeting;
    //字符串分割
    qDebug() << greeting;
    QStringList msg = greeting.split("|");
    qDebug() << "接收方:" << msg[0];//my
    qDebug() << "msg:" << msg[2];
    if(msg[0] == "init"){
        ui->currentPeople->clear();
        for (int i = 1; i < msg.length(); i++){
            ui->currentPeople->append(msg[i]);
            ui->currentPeople->setAlignment(Qt::AlignCenter);
        }
        //isInit = true;
    }else if(msg[0] == "ChatALL:" && myAccount != msg[1]){
        ui->chatDisplay->append(tr("%1: %2").arg(msg[1]).arg(msg[2]));
        ui->chatDisplay->setAlignment(Qt::AlignLeft);
    }
    msg.clear();
}

void PublicChat::on_sendButton_clicked()
{
    sendMessage();
}

void PublicChat::sendMessage(){
    //string = 接收方 + | + 发送方 + 发送信息
    QString string = "ChatALL:|" + myAccount + "|" + ui->sendMsgLine->text();

    QByteArray message;
    //以只读打开QByteArray，并设置版本，服务端客户端要一致
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    //写入输出流
    out << string;
    qDebug() << "发送方-"+myAccount + ": " << string;
    //发送信息
    tcpsocket->write(message);
    //面板添加信息
    ui->chatDisplay->append(tr("%1 :%2").arg(ui->sendMsgLine->text()).arg(myAccount));       //添加消息
    ui->chatDisplay->setAlignment(Qt::AlignRight);                                       //此条要在添加消息后面，否则前一条消息跟着改变
    //qDebug() << tr("%1 -> %2: %3").arg(myAccount).arg(yourAccount).arg(ui->sendMsgLine->text());
    ui->sendMsgLine->clear();
}

void PublicChat::init(){
    QString string = "init";
    QByteArray message;
    //以只读打开QByteArray，并设置版本，服务端客户端要一致
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    //写入输出流
    out << string;
    qDebug() << "发送方-"+myAccount + ": " << string;
    //发送信息
    tcpsocket->write(message);
}
