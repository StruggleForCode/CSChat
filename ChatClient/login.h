#ifndef LOGIN_H
#define LOGIN_H

#include "register.h"
#include "findpassword.h"
#include "initsurface.h"

#include <QWidget>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private:
    Ui::Login *ui;

    //network
    QTcpSocket *client;
    //定时器
    QTimer *timer;
    InitSurface *initSurface;       //主界面的初始化信息
    //标志
    bool networkAbleFlag = false;   //网络标志
    bool connectFlag = false;       //连接服务器标志
    bool loginSuccessFlag = false;  //登录成功的标志
    bool registerAccountFlag = false;//注册账号标志
    bool findPasswordFlag = false;   //找回密码标志
    //注册 找回对象
    Register *newAccountRegister = NULL;
    FindPassword *findPassword = NULL;
signals:
    void sendAccount(QString);      //发送给initSurface
private slots:
    void getClose(bool flag);       //本窗口关闭
    void getRegisterClose(bool flag);//注册页面关闭
    void getFindClose(bool flag);   //找回页面关闭
    void on_login_button_clicked(); //登陆
    void login_success();           //登录成功
    void on_register_button_clicked();  //注册账号
    void on_lost_password_button_clicked(); //查找密码
    bool connectServer();           //连接服务器
    void commitMessage();           //提交本地信息
    void readMessage();             //读取消息
};
#endif // LOGIN_H
