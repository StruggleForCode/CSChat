#include "user.h"
#include "ui_user.h"

#include <QResizeEvent>
#include <QCloseEvent>
#include <QtNetwork>
#include <QTcpSocket>
#include <QStringList>

User::User(QString title) : ui(new Ui::User)
{
    qDebug() << "是否来到";
    ui->setupUi(this);
    QPixmap pix;
    userTitle = title;
    setWindowTitle(title);
    qDebug() << "是否来到";
    pix.load(":image/img/head.png");
    pix = pix.scaled(QSize(80,80));

    ui->headLabel->setPixmap(pix);
    ui->headLabel->resize(QSize(80,80));
    ui->headLabel->setToolTip("头像");

    ui->userNameLabel->setText(userTitle);
    ui->userSignLabel->setText("I'm  so handsome");
    ui->userSignLabel->setToolTip("个性签名！");

    this->installEventFilter(this);
    ui->sendMsgLine->installEventFilter(this);

}

User::~User()
{
    delete ui;
}

void User::on_sendButton_clicked()
{
    sendMessage();
}

bool User::eventFilter(QObject *obj, QEvent *event)
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
            emit sendAccount(yourAccount);
            return true;
        }else
            return false;
    }else
        return QWidget::eventFilter(obj,event);
    return QWidget::eventFilter(obj,event);
}

void User::getAccount(QString user1, QString user2)
{
    myAccount = user1;
    yourAccount = user2;
    userTitle = yourAccount;
    qDebug() << "发送方: " << myAccount;
    qDebug() << "接收方: " << yourAccount;
    //连接服务器
    tcpServerConnect();
}

void User::tcpServerConnect()
{
    //实例化socket
    tcpsocket = new QTcpSocket(this);
    //断开现有连接
    tcpsocket->abort();
    //连接到本地的6666端口
    tcpsocket->connectToHost(QHostAddress::LocalHost,6666);
    //有可读信息，发送readyRead()
    connect(tcpsocket,SIGNAL(readyRead()),
            this,SLOT(readMessage()));
}

void User::sendMessage(){
    //string = 接收方 + | + 发送方 + 发送信息
    QString string = yourAccount + "|" + myAccount + "|" + ui->sendMsgLine->text();

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
    ui->chatEidt->append(tr("%1 :%2").arg(ui->sendMsgLine->text()).arg(myAccount));       //添加消息
    ui->chatEidt->setAlignment(Qt::AlignRight);                                       //此条要在添加消息后面，否则前一条消息跟着改变
    qDebug() << tr("%1 -> %2: %3").arg(myAccount).arg(yourAccount).arg(ui->sendMsgLine->text());
    ui->sendMsgLine->clear();
}

void User::readMessage(){
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
    qDebug() << "发送方:" << msg[1];//you
    qDebug() << "msg:" << msg[2];
    if(myAccount == msg[0])
    {
        ui->chatEidt->append(tr("%1: %2").arg(msg[1]).arg(msg[2]));        //面板添加
        ui->chatEidt->setAlignment(Qt::AlignLeft);                         //此条要在添加消息后面
        qDebug() << tr("%1 -> %2: %3").arg(msg[1]).arg(msg[0]).arg(msg[2]);
    }
    if(msg[0] == "server")
    {
        ui->chatEidt->append(tr("Server: %1").arg(msg[1]));
        ui->chatEidt->setAlignment(Qt::AlignCenter);
        qDebug() << tr("Server: %1").arg(msg[1]);
    }
    msg.clear();
}
