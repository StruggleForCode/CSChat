#include "selectuserinterface.h"
#include "ui_selectuserinterface.h"
#include "user.h"
#include "publicchat.h"

#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QAbstractItemModel>


SelectUserInterface::SelectUserInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectUserInterface)
{
    ui->setupUi(this);
    pix.load(":image/img/head.png");
    pix = pix.scaled(QSize(80,80));
    ui->headLabel->setPixmap(pix);
    ui->headLabel->resize(QSize(80,80));
    ui->headLabel->setToolTip("头像");

    //安装事件过滤器
    this->installEventFilter(this);
    ui->user1->installEventFilter(this);
    ui->user2->installEventFilter(this);
    ui->user3->installEventFilter(this);
    ui->user4->installEventFilter(this);
    ui->common_chat->installEventFilter(this);

    //连接服务器,7777端口，请求退出
    tcpServerConnect();
}

SelectUserInterface::~SelectUserInterface()
{
    delete ui;
}

//得到初始化界面的消息
void SelectUserInterface::setInitSurface(QString account, QString name, QString sign, QString head)
{
    qDebug() << "SelectUserInterface::getInitSurface" << account;
    myAccount = account;                                        //得到自身账号
    this->setWindowTitle(account);
    ui->userNameLabel->resize(name.size()*30,30);
    ui->userNameLabel->setText(name);
    ui->userSignLabel->resize(sign.size()*30,20);
    ui->userSignLabel->setText(sign);
    if(head != ""){
        pix.load(":image/img/"+head);                       //加载图片
        pix = pix.scaled(QSize(80,80));                              //指定图片大小
        ui->headLabel->setPixmap(pix);                                   //设置头像遮罩
    }

}

bool SelectUserInterface::eventFilter(QObject *obj, QEvent *event){
    if(obj == ui->user1 || obj == ui->user2 || obj == ui->user3 || obj == ui->user4 || obj == ui->common_chat){
        if(event->type() == QEvent::MouseButtonDblClick){
            qDebug() << "MouseButtonDblClick";
            //强制转换事件类型
            QMouseEvent *dbClick = static_cast<QMouseEvent *>(event);
            //左键双击
            if(dbClick->button() == Qt::LeftButton){
                qDebug() << "左键双击";
                if(obj == ui->user1)
                {
                    yourAccount = ui->user1->text();
                }else if(obj == ui->user2){
                    yourAccount = ui->user2->text();
                }else if(obj == ui->user3){
                    yourAccount = ui->user3->text();
                }else if(obj == ui->user4){
                    yourAccount = ui->user4->text();
                }else if(obj == ui->common_chat){
                    qDebug() << "heh" << endl;
                    chat = new PublicChat(myAccount);
                    //向子窗口发送双方账号信息
                    connect(this,SIGNAL(sendChatAccount(QString)),
                            chat,SLOT(getAccount(QString)));
//                    //得到子窗口的聊天人
//                    connect(AllUser.last(),SIGNAL(sendAccount(QString)),
//                            this,SLOT(getAccount(QString)));
                    emit sendChatAccount(myAccount);
                    chat->show();
                    return QWidget::eventFilter(obj,event);
                }

                qDebug() << "联系人:"+yourAccount;
                bool exitFlag = false;
                QList<User *>::iterator it;
                for(it = AllUser.begin(); it < AllUser.end(); it++){
                    if((*it)->userTitle == yourAccount){
                        qDebug() << "已存在";
                        exitFlag = true;
                        (*it)->update();
                        break;
                    }
                }
                //当前列表中不存在，则添加新人并关联信号
                if(!exitFlag){
                    if(obj == ui->user1)
                    {
                        newUser = new User(ui->user1->text());
                    }else if(obj == ui->user2){
                        newUser = new User(ui->user2->text());
                    }else if(obj == ui->user3){
                        newUser = new User(ui->user3->text());
                    }else if(obj == ui->user4){
                        newUser = new User(ui->user4->text());
                    }

                    qDebug() << "添加新人";
                    AllUser.push_back(newUser);
                    //向子窗口发送双方账号信息
                    connect(this,SIGNAL(sendAccount(QString,QString)),
                            AllUser.last(),SLOT(getAccount(QString,QString)));
                    //得到子窗口的聊天人
                    connect(AllUser.last(),SIGNAL(sendAccount(QString)),
                            this,SLOT(getAccount(QString)));
                    //发送给子窗口
                    emit sendAccount(myAccount,yourAccount);
                    newUser->show();
                }
            }else
                return false;
        }else
            return false;
    }else if (obj == this){
        if(event->type() == QEvent::Close){
            switch( QMessageBox::information(this,
                                             tr("提示"),
                                             tr("你确定要下线?"),
                                             tr("确定"), tr("取消"),0,1))
            {
            case 0:

                //发送请求退出
                sendMessage("logout");
                event->accept();
                break;
            case 1:
            default:
                event->ignore();
                break;
            }
            return true;
        }else
            return false;
    }
    return QWidget::eventFilter(obj,event);
}

void SelectUserInterface::tcpServerConnect()
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

void SelectUserInterface::readMessage()
{
    //实例化套接字的输入流，读信息
    QDataStream in(tcpsocket);
    in.setVersion(QDataStream::Qt_5_14);

    QString greeting;
    //将信息写入greeting读取信息
    in >> greeting;
    //字符串分割
    QStringList msg = greeting.split("|");
    if(msg[0] == "logout"){
        if(myAccount == msg[1]){
            qDebug() << "logout::readMessage:" << greeting;
            qDebug() << "logout:" << "Success";

        }
    }
}

//发送
void SelectUserInterface::sendMessage(QString Msg)
{
    qDebug() << "发送消息";
    if(Msg == "logout"){
        //初始化界面
        QString string = "logout|" + myAccount;
        QByteArray message;
        //以只读打开QByteArray，并设置版本，服务端客户端要一致
        QDataStream out(&message,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_14);
        //写入输出流
        out << string;
        qDebug() << "SelectUserInterface::sendMessage:" << string;
        //发送信息
        tcpsocket->write(message);
    }
    qDebug() << "发送完成";

}

//得到要关闭的聊天人
QString SelectUserInterface::getAccount(QString account)
{
    qDebug() << "SelectUserInterface::getAccount" << account;
    yourAccount = account;
    qDebug() << "关闭聊天人" << yourAccount;
    bool exitFlag = false;
    QList<User *>::iterator it;
    for(it = AllUser.begin(); it < AllUser.end(); it++){
        if((*it)->userTitle == yourAccount){
            exitFlag = true;
            AllUser.removeOne((*it));
            (*it)->deleteLater();
            qDebug() <<"关闭成功";
            break;
        }
    }
    if(!exitFlag){
        qDebug() << "不存在聊天人" << yourAccount;
    }
    return account;
}

//得到初始化界面的消息
void SelectUserInterface::getInitSurface(QString account, QString name, QString sign, QString head)
{
    qDebug() << "SelectUserInterface::getInitSurface" << account;
    myAccount = account;                                        //得到自身账号
    this->setWindowTitle(account);
    ui->userNameLabel->resize(name.size()*30,30);
    ui->userNameLabel->setText(name);
    ui->userSignLabel->resize(sign.size()*30,20);
    ui->userSignLabel->setText(sign);
    if(head != ""){
        pix.load(":image/img/"+head);                       //加载图片
        pix = pix.scaled(QSize(80,80));                              //指定图片大小
        ui->headLabel->setPixmap(pix);                                   //设置头像遮罩
    }

}
