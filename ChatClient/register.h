#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QTcpSocket>
#include <QtNetwork>
#include <QCloseEvent>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::Register *ui;
    //network
    QTcpSocket *tcpsocket;
    QString myAccount;
    //临时信息
    QString passwordTemp;
    QString nameTemp;
    QString signTemp;
    QString headTemp;
signals:
    void sendRegisterClose(bool ture);
private slots:
    void on_commit_clicked();
    void sendMessage(QString MSg);      //向服务器发送信息
    void readMessage();                 //读取服务器信息
    void tcpServerConnect();            //连接服务器

};

#endif // REGISTER_H
