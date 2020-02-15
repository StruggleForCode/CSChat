#ifndef SELECTUSERINTERFACE_H
#define SELECTUSERINTERFACE_H

#include "user.h"
#include "publicchat.h"

#include <QWidget>
#include <QTcpSocket>
#include <QStringList>


namespace Ui {
class SelectUserInterface;
}

class SelectUserInterface : public QWidget
{
    Q_OBJECT

public:
    explicit SelectUserInterface(QWidget *parent = nullptr);
    ~SelectUserInterface();
    bool eventFilter(QObject *obj, QEvent *event);
    void setInitSurface(QString account,QString name,
                            QString sign,QString head);//得到初始化界面的消息

private:
    Ui::SelectUserInterface *ui;
    //临时对话者
    User *newUser;
    PublicChat *chat;
    //使用者
    QString myAccount;
    //接收方
    QString yourAccount;
    //对话组列表
    QList<User *> AllUser;
    //network
    QTcpSocket *tcpsocket;
    QPixmap pix;
signals:
    void sendAccount(QString,QString);          //发送给user的getAccount
    void sendChatAccount(QString);              //发送给publicChat的getAccount
public slots:
    void getInitSurface(QString account,QString name,
                        QString sign,QString head);//得到初始化界面的消息
    QString getAccount(QString account);        //接收user的sendAccout
    void sendMessage(QString Msg);              //向服务器发送信息
    void readMessage();                         //读取服务器信息
    void tcpServerConnect();                    //连接服务器
};

#endif // SELECTUSERINTERFACE_H
