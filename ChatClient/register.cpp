#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    this->installEventFilter(this);
}

Register::~Register()
{
    delete ui;
}

bool Register::eventFilter(QObject *obj, QEvent *event){
    if(obj == this){
        if(event->type() == QCloseEvent::Close){
            qDebug() << "关闭注册页面";
            emit sendRegisterClose(true);
        }else{
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

//发送注册请求
void Register::on_commit_clicked()
{
    //用户名
    if(ui->name_line_2->text().isEmpty()){
        ui->tip_label->setText("用户名不能为空");
        return;
    }else
        nameTemp = ui->name_line_2->text();
    //密码
    if(ui->password_line_2->text().isEmpty()){
        ui->tip_label->setText("密码不能为空");
        return;
    }
    //确认密码
    if(ui->confirmPassword_line_2->text().isEmpty()){
        ui->tip_label->setText("确认密码不能为空");
        return;
    }
    //两次密码
    if(ui->password_line_2->text() != ui->confirmPassword_line_2->text()){
        ui->tip_label->setText("两次密码不一致");
        return;
    }else
        passwordTemp = ui->confirmPassword_line_2->text();
    //签名
    if(ui->sign_line_2->text().isEmpty()){
        ui->sign_line_2->setText("I am YML");
        signTemp = ui->sign_line_2->text();
        ui->sign_line_2->clear();
    }else
        signTemp = ui->sign_line_2->text();
    //头像
    if(ui->head_line_2->text().isEmpty()){
        ui->head_line_2->setText("head.png");
        headTemp = ui->head_line_2->text();
        ui->head_line_2->clear();
    }else
        headTemp = ui->head_line_2->text();
    tcpServerConnect();
    sendMessage("register");
}

void Register::tcpServerConnect(){
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

void Register::sendMessage(QString Msg){
    qDebug() << "Register::sendMessage:发送消息";
    if(Msg == "register"){
        //初始化界面
        QString string = "register|"+nameTemp+"|"+passwordTemp+"|"
                +signTemp+"|"+headTemp;
        QByteArray message;
        //以只读打开QByteArray，并设置版本，服务端客户端要一致
        QDataStream out(&message,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_14);
        //写入输出流
        out << string;
        qDebug() << "Register::sendMessage:" << string;
        //发送信息
        tcpsocket->write(message);
    }
    qDebug() << "Register::sendMessage:发送完成";
}

void Register::readMessage(){
    //实例化套接字的输入流，读信息
    QDataStream in(tcpsocket);
    in.setVersion(QDataStream::Qt_5_4);
    qDebug() << "Register::readMessage:读取消息";
    QString greeting;
    QString account;
    QString password;
    //将信息写入greeting读取信息
    in >> greeting;
    //读取的信息
    qDebug() << "Register::readMessage:" << greeting;
    //字符串分割
    QStringList msg = greeting.split("|");
    if(msg[0] == "register"){
        account = msg[1];
        myAccount = account;
        qDebug() << "Register";
        qDebug() << "新账号:"+account;
        ui->tip_label->setText("新账号:"+account);
    }else{
        qDebug() << "注册失败";
    }
    qDebug() << "Register::readMessage:读取完成";
}
