#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "initsurface.h"

#include <QTime>
#include <QMessageBox>
#include <QHostInfo>
#include <QTcpSocket>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("ChatClient");
    setFixedSize(ui->Login_background->size());

    ui->password_Edit->setEchoMode(QLineEdit::Password);

//    timer = new QTimer(this);
    timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(login_success()));

}

Login::~Login()
{
    delete ui;
}

void Login::login_success(){
    qDebug() << "登陆成功";
    ui->login_button->setText("登陆");
    //先得到主界面初始化信息，在得到初始化信息界面信息后在初始化
    initSurface = new InitSurface();
    connect(this, SIGNAL(sendAccount(QString)), initSurface, SLOT(getAccount(QString)));
    emit sendAccount(ui->account_Edit->text());
    disconnect(this, SIGNAL(sendAccount(QString)), initSurface, SLOT(getAccount(QString)));
    connect(initSurface,SIGNAL(sendClose(bool)), this, SLOT(getClose(bool)));
}


void Login::on_login_button_clicked()
{
//    ui->login_button->setText("正在登陆");
    if(ui->account_Edit->text() == ""){
        ui->tip_label->setText("未输入账号");
        qDebug() << "未输入账号";
        return;
    }else if(ui->password_Edit->text() == ""){
        ui->tip_label->setText("未输入密码");
        qDebug() << "未输入密码";
        return;
    }else{
        //阻塞查找
        QHostInfo info  = QHostInfo::fromName("www.baidu.com");
        if(info.error() == QHostInfo::NoError){
            networkAbleFlag = true;
            qDebug() << "可以上网";
        }else{
            networkAbleFlag = false;
            qDebug() << "未连接网络";
            ui->tip_label->setText("未连接网络");
            ui->login_button->setText("登陆");
            return;
        }
    }

    //网络可用，连接服务器
    if(networkAbleFlag){
        ui->tip_label->setText("网络可用");
        qDebug() << "连接服务器";
        if(connectServer()){
            qDebug() << "连接服务器:成功";
            commitMessage();                                                            //提交本地账号密码，服务器检测
            qDebug() << "服务器检测";
        }else{
            qDebug() << "连接服务器:失败";
            ui->tip_label->setText("连接服务器:失败");
            return;
        }
    }else{
        ui->tip_label->setText("未连接网络");
        return;
    }
}

bool Login::connectServer(){
    //实例化客户端
    client = new QTcpSocket(this);
    //断开已有连接
    client->abort();
    //指定连接的服务器和端口
    client->connectToHost("127.0.0.1",5555);
    //连接服务器，等待10s,如果连接成功，返回true
    connectFlag = client->waitForConnected(10000);
    //连接失败
    if(connectFlag){
        ui->tip_label->setText("连接服务器：成功");
        //当有可读数据时，发射readyread信号
        connect(client,&QTcpSocket::readyRead,
                this, &Login::readMessage);
        return true;
    }else{
        ui->tip_label->setText("连接服务器：失败");
        return false;
    }
}

void Login::commitMessage(){
    if(!connectFlag) return;
    qDebug() << "提交本地账号，密码";
    QString account = ui->account_Edit->text();
    QString password = ui->password_Edit->text();
    client->write(account.toUtf8() + " " + password.toUtf8());
    qDebug() << account;
    qDebug() << password;
    qDebug() << "提交完成，等待服务器检测";
}

void Login::readMessage(){
    if(!connectFlag) return;
    qDebug() << "服务器反馈信息";
    QString str;
    str = client->readAll();
    qDebug() << str;
    QString state;
    if(str == "loginSuccess"){
        loginSuccessFlag = true;
    }else if(str == "loginFail"){
        loginSuccessFlag = false;
        state = "账号或密码错误!";
    }else if(str == "AlreadyOnline"){
        loginSuccessFlag = false;
        state = "不能重复登陆";
    }
    qDebug() << "读取完成";

    if(loginSuccessFlag){
        qDebug() << "正在登陆";
        ui->login_button->setText("正在登陆...");
        timer->start(2000);
    }else{
        ui->login_button->setText("登陆失败");
        qDebug() << "登陆失败";
        //弹出警告
        QMessageBox::warning(this,                  //父窗口
                             tr("警告"),             //窗口标题
                             state,                  //界面内容
                             QMessageBox::Yes);      //显示yes, no按钮
        ui->account_Edit->setText("");
        ui->password_Edit->setText("");
    }
}


void Login::getClose(bool flag){
    if(flag){
        ui->login_button->setText("登录");
        timer->stop();
        timer->deleteLater();
        close();
    }else{
        ui->login_button->setText("关闭失败");
    }
}

void Login::on_register_button_clicked()
{
    if(!newAccountRegister){
        qDebug() << "注册";
        newAccountRegister = new Register();
        newAccountRegister->show();
        registerAccountFlag = true;
        connect(newAccountRegister,SIGNAL(sendRegisterClose(bool)),
                       this,SLOT(getRegisterClose(bool)));
    }
}

void Login::getRegisterClose(bool flag){
    if(registerAccountFlag == flag)
    {
        qDebug() << "释放注册页面";
        newAccountRegister->deleteLater();
        newAccountRegister = NULL;
        registerAccountFlag = false;
        qDebug() << "注册页面已关闭";
    }else{
        qDebug() << "释放注册页面失败";
    }
}

void Login::on_lost_password_button_clicked()
{
    if(!findPassword){
        qDebug() << "找回密码";
        findPassword = new FindPassword();
        findPassword->show();
        findPasswordFlag = true;
        connect(findPassword,SIGNAL(sendFindClose(bool)),
                this,SLOT(getFindClose(bool)));
    }else{
        qDebug() << "找回页面已打开";
    }
}

void Login::getFindClose(bool flag){
    if(findPasswordFlag == flag){
        qDebug() << "释放关闭页面";
        findPassword->deleteLater();
        findPassword = NULL;
        findPasswordFlag = false;
    }else{
        qDebug() << "释放找回界面失败";
    }
}

