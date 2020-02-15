#ifndef INITSURFACE_H
#define INITSURFACE_H

#include "selectuserinterface.h"

#include <QWidget>
#include <QtNetwork>
#include <QTcpSocket>
#include <QStringList>

namespace Ui {
class InitSurface;
}

class InitSurface : public QWidget
{
    Q_OBJECT

public:
    explicit InitSurface(QWidget *parent = nullptr);
    ~InitSurface();
    void setAccount(QString account);

private:
    Ui::InitSurface *ui;
    //network
    QTcpSocket *tcpsocket;
    //使用者
    QString myAccount;
    //临时信息
    QString accountTemp;
    QString nameTemp;
    QString signTemp;
    QString headTemp;
    //主界面
    SelectUserInterface *selectUser;
signals:
    //发送初始化界面消息
    void sendInitSurface(
            QString,QString,QString,QString);
    //发送给login关闭信号
    void sendClose(bool flag);
public slots:
    void sendMessage(QString Msg);              //向服务器发送信息
    void readMessage();                         //读取服务器信息
    void tcpServerConnect();                    //连接服务器
    QString getAccount(QString account);        //得到账号
};

#endif // INITSURFACE_H
