#ifndef PUBLICCHAT_H
#define PUBLICCHAT_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class PublicChat;
}

class PublicChat : public QWidget
{
    Q_OBJECT

public:
    explicit PublicChat(QString title);
    ~PublicChat();
    QString userTitle;
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::PublicChat *ui;
    QString myAccount;
    //tcp
    QTcpSocket *tcpsocket;
    void init();
    bool isInit = false;
public slots:
    void getAccount(QString user1);
    void sendMessage();             //向服务器发送信息
    void readMessage();             //读取服务器信息
    void tcpServerConnect();        //连接服务器
                      //初始化界面
private slots:
    void on_sendButton_clicked();

};

#endif // PUBLICCHAT_H
