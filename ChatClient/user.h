#ifndef USER_H
#define USER_H

#include <QWidget>
#include <QTcpServer>

namespace Ui {
class User;
}

class User : public QWidget
{
    Q_OBJECT
public:
    explicit User(QString title);
    ~User();
    QString userTitle;
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::User *ui;
    QString myAccount;
    QString yourAccount;
    QTcpSocket *tcpsocket;
signals:
    void sendAccount(QString);//发送给select界面聊天人账号
public slots:
    void getAccount(QString user1,QString user2);
    void sendMessage();             //向服务器发送信息
    void readMessage();             //读取服务器信息
    void tcpServerConnect();        //连接服务器
    void on_sendButton_clicked();
};

#endif // USER_H
