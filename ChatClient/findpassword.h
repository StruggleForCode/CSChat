#ifndef FINDPASSWORD_H
#define FINDPASSWORD_H

#include <QWidget>
#include <QTcpSocket>
#include <QCloseEvent>

namespace Ui {
class FindPassword;
}

class FindPassword : public QWidget
{
    Q_OBJECT

public:
    explicit FindPassword(QWidget *parent = nullptr);
    ~FindPassword();
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::FindPassword *ui;
    //network
    QTcpSocket *tcpsocket;
    QString myAccount;
signals:
    void sendFindClose(bool);
private slots:
    void on_commit_clicked();                   //发送找回请求
    void sendMessage(QString Msg);              //向服务器发送信息
    void readMessage();                         //读取服务器信息
    void tcpServerConnect();                    //连接服务器
};

#endif // FINDPASSWORD_H
