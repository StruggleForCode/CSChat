#include "server.h"
#include "ui_server.h"
#include "userlist.h"

#include <QDebug>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    user = new UserList();
    setWindowTitle("chartServer");

    //实例化network
    server_check = new QTcpServer(this);
    client_check = new QTcpSocket(this);
    server_news = new QTcpServer(this);
    client_news = new QTcpSocket(this);
    clients_news = new QList<QTcpSocket *>;
    server_surface = new QTcpServer(this);
    client_surface = new QTcpSocket(this);
    clients_surface = new QList<QTcpSocket *>;

    server_Chatnews = new QTcpServer(this);
    client_Chatnews = new QTcpSocket(this);
    clients_Chatnews =new QList<QTcpSocket *>;
}



void Server::on_startButton_clicked()
{
    if(listenFlag_check == true && listenFlag_news == true){
        qDebug() << "服务器已打开";
    }
    qDebug() << "打开端口：5555 6666 7777 8888";
    ui->display_screen->append("打开端口：5555 6666 7777 8888");
    ui->display_screen->setAlignment(Qt::AlignCenter);
    //监听任何连接上5555端口的ip, 成功返回true, 用于账号检验
    listenFlag_check = server_check->listen(QHostAddress::Any, 5555);
    //监听任何连接上6666端口的ip，成功返回true，用于转发消息
    listenFlag_news = server_news->listen(QHostAddress::Any, 6666);
    //监听所有连接7777端口的ip，成功返回true，用于初始化界面
    listenFlag_surface = server_surface->listen(QHostAddress::Any, 7777);
    //监听所有连接8888端口上的ip，成功返回true, 用于群聊
    listenFlag_Chatnews = server_Chatnews->listen(QHostAddress::Any, 8888);

    if(listenFlag_news == false || listenFlag_check == false || listenFlag_surface == false || listenFlag_Chatnews == false){
        ui->tip_Label->setText("服务状态：关闭");
        qDebug() << "开启服务器失败";
        ui->display_screen->append("开启服务器失败");
        return;
    }
    ui->tip_Label->setText("服务状态：开启");
    ui->display_screen->append("成功开启服务器");
    qDebug() << "服务状态：开启";

    //新客户端连接，发射newConnect信号
    connect(server_check, &QTcpServer::newConnection,
               this, &Server::newClient_check);
    connect(server_news, SIGNAL(newConnection()),
               this, SLOT(newConnect_news()));
    connect(server_surface, SIGNAL(newConnection()),
            this, SLOT(newConnect_surface()));
    connect(server_Chatnews, SIGNAL(newConnection()),
                this, SLOT(newConnect_Chatnews()));
    //服务端销毁
    connect(server_check, &QTcpServer::destroyed,
            this,&Server::destoryServer_check);
    qDebug() << "等待连接...";
}

void Server::on_send_button_clicked()
{
    qDebug() << "群发";
    QString msg = "server";
    msg = msg + "|" + ui->message_textEdit->toPlainText() + "|" + "All";
    sendMessage_news(msg);
    ui->display_screen->append(ui->message_textEdit->toPlainText()+":群发");
    ui->display_screen->setAlignment(Qt::AlignRight);
    ui->message_textEdit->clear();
}

void Server::on_clear_button_clicked()
{
    ui->display_screen->clear();
}

void Server::on_close_button_clicked()
{
    listenFlag_check = false;
    listenFlag_news = false;
    listenFlag_surface = false;
    ui->tip_Label->setText("服务器状态：关闭");
    ui->display_screen->append("关闭端口：5555 6666 7777");
    ui->display_screen->setAlignment(Qt::AlignCenter);
    ui->display_screen->append("关闭服务器");

}

/************************************通过数据库验证账号部分 端口 5555***************************/
//新客户连接
void Server::newClient_check(){
    if(!listenFlag_check) return;
    qDebug() << "新客户端登陆:";
    //设置为假，用于新客户端登陆判断
    loginSuccessFlag_check = false;
    QTcpSocket *newClient;

    //得到新进来的socket
    newClient = server_check->nextPendingConnection();
    //连接标志
    clientJoinUp_check = true;

    qDebug() << QHostAddress(newClient->peerAddress().toIPv4Address()).toString();

    qDebug() << "等待信息";

    //有可读信息，发射readyread信号
    connect(newClient, &QTcpSocket::readyRead, this, &Server::readMessage_check);
    //断开连接，发射disconnect信号
    connect(newClient, &QTcpSocket::disconnected, this, &Server::lostClient_check);

    //在客户端列表最后添加新的socket
    clients_check.push_back(newClient);
    client_check = newClient;
}

//账号验证的反馈消息
void Server::feedbackMessage(QString msg){
    if(!listenFlag_check) {
        qDebug() << "feedbackMessage:服务端未打开";
        return;
    }
    if(!clientJoinUp_check) {
        qDebug() << "feedbackMessage:客户端未连接";
        return;
    }
    qDebug() << "feedbackMessage:发送消息";
    //添加到显示面板
    ui->display_screen->append("登录结果:"+msg);
    ui->display_screen->setAlignment(Qt::AlignCenter);
    ui->display_screen->moveCursor(QTextCursor::End);
    ui->message_textEdit->setText("");
    //发送
    client_check->write(msg.toUtf8());
    qDebug() << "Server_send:" <<msg;
    qDebug() << "feedbackMessage:发送完成";
}

//读取
void Server::readMessage_check(){
    qDebug() << "readMessage_check:读取信息";
    //通过信号的发出者找到相应的对象
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    QString str;
    QString account;
    QString password;
    //读取
    str = client->readAll();
    qDebug() << "readMessage_check:" << str;
    account = str.section(' ', 0, 0);//字符串分割
    password = str.section(' ', 1, 1);
    //添加到显示面板
    ui->display_screen->append("请求登陆");
    ui->display_screen->setAlignment(Qt::AlignCenter);
    ui->display_screen->append("登陆账号：" + account);
    ui->display_screen->append("登陆密码：" + password);
    ui->display_screen->moveCursor(QTextCursor::End);
    qDebug() << "登陆账号：" << account;
    qDebug() << "登陆密码：" << password;

    //temp = 0 时，账号密码不匹配登陆失败，temp = 1，登陆成功， temp = 2，已经在线
    int temp = user->CheckUser(account, password);
    if(temp == 0){
        loginSuccessFlag_check = false;
        feedbackMessage("loginFail");
        qDebug() << "登陆失败";
    }else if(temp == 1){
        loginSuccessFlag_check = true;
        feedbackMessage("loginSuccess");
        qDebug() << "登陆成功";
    }else if(temp == 2){
        loginSuccessFlag_check = true;
        feedbackMessage("AlreadyOnline");
    }
    qDebug() << "readMessage_check:读取完成";
}

//删除
void Server::lostClient_check(){
    qDebug() << "lostClient_check:登录销毁";
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    clients_check.removeOne(client);
    qDebug() << "删除:" << QHostAddress(client->peerAddress().toIPv4Address()).toString();
}

//发送
void Server::sendMessage_check(){

}



/***********************************登录成功，转发消息部分  端口 6666****************************/
//新客户端连接
void Server::newConnect_news(){
    if(!listenFlag_news) return;
    qDebug() << "新聊天线程：";

    //得到新连接进来的socket
    client_news = server_news->nextPendingConnection();

    //添加到聊天列表
    clients_news->append(client_news);
    clientJoinUp_news = true;

    //有可读信息发送readyRead()信号
    connect(client_news, SIGNAL(readyRead()),
            this, SLOT(readMessage_news()));
    //客户端断开连接，发送disconnected()
    connect(client_news, SIGNAL(disconnected()),
            this, SLOT(lostClient_news()));

}

//读取
void Server::readMessage_news(){
    qDebug() << "读取信息";
    QString message;
    //遍历客户端列表，所有客户端
    for(int i = 0; i < clients_news->length(); i++){
        qDebug() << "QDataStream ";
        //设置响应socket的输入流和版本
        QDataStream in(clients_news->at(i));
        in.setVersion (QDataStream::Qt_5_14);
        //读取信息，信息写入message，
        in >> message;
        //每有一个可读信息，message非空，跳出 显示
        if(!message.isEmpty()){
            qDebug() << "break";
            break;
        }
    }
    //面板显示消息
    ui->display_screen->append(tr("新消息: '%1'").arg(message));
    ui->display_screen->setAlignment(Qt::AlignLeft);
    //发送到对应客户端
    sendMessage_news(message);
    qDebug() << "Server_read:" << message;
    qDebug() << "readMessage_news:读取完成";
}

//发送
void Server::sendMessage_news(QString information){
    if(!listenFlag_news) {
         qDebug() << "sendMessage_news:服务端未打开";
         return;
     }
     if(!clientJoinUp_news) {
         qDebug() << "sendMessage_news:客户端未连接";
         return;
     }

     qDebug() << "sendMessage_news:发送消息";
     QString str = information;
     QByteArray message;
     //设置输出流只写，并设置版本
     QDataStream out(&message,QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_5_14);
     out << str;
     //遍历客户端，嵌套字写入
     for(int i = 0; i < clients_news->length(); i++){
         clients_news->at(i)->write(message);
     }
     qDebug() << "Server_send:" << str;
     qDebug() << "sendMessage_news:发送完成";
}

//删除
void Server::lostClient_news(){
    qDebug() << "lostClient_news:关闭聊天";
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    clients_news->removeOne(client);
    qDebug() << "lostClient_news:colse";
}


/************************************群聊 端口8888*****************************************/
//新客户端连接
void Server::newConnect_Chatnews(){
    if(!listenFlag_Chatnews) return;
    qDebug() << "群聊线程：";

    //得到新连接进来的socket
    client_Chatnews = server_Chatnews->nextPendingConnection();

    //添加到聊天列表
    clients_Chatnews->append(client_Chatnews);
    clientJoinUp_Chatnews = true;

    //有可读信息发送readyRead()信号
    connect(client_Chatnews, SIGNAL(readyRead()),
            this, SLOT(readMessage_Chatnews()));
    //客户端断开连接，发送disconnected()
    connect(client_Chatnews, SIGNAL(disconnected()),
            this, SLOT(lostClient_Chatnews()));
}

void Server::readMessage_Chatnews(){
    qDebug() << "读取信息";
    QString message;
    //遍历客户端列表，所有客户端
    for(int i = 0; i < clients_Chatnews->length(); i++){
        qDebug() << "QDataStream ";
        //设置响应socket的输入流和版本
        QDataStream in(clients_Chatnews->at(i));
        in.setVersion (QDataStream::Qt_5_14);
        //读取信息，信息写入message，
        in >> message;
        //每有一个可读信息，message非空，跳出 显示
        if(!message.isEmpty()){
            qDebug() << "break";
            break;
        }
    }
    //面板显示消息
    ui->display_screen->append(tr("'%1'").arg(message));
    ui->display_screen->setAlignment(Qt::AlignLeft);
    //发送到对应客户端
    sendMessage_Chatnews(message);
    qDebug() << "Server_read:" << message;
    qDebug() << "readMessage_news:读取完成";
}

void Server::sendMessage_Chatnews(QString information){
    if(!listenFlag_Chatnews) {
         qDebug() << "sendMessage_news:服务端未打开";
         return;
     }
     if(!clientJoinUp_Chatnews) {
         qDebug() << "sendMessage_Chatnews:客户端未连接";
         return;
     }
     qDebug() << "sendMessage_Chatnews:发送消息";
     QString str = "init|";
     if(information == "init"){
         //数据库查找
         user->query->exec("select * from User");
         while(user->query->next()){
             if(user->query->value(5).toString() == "1"){
                 str = str + user->query->value(0).toString() + "|";
             }
            qDebug() << str;
         }
     }else{
         str = information;
     }
     QByteArray message;
     //设置输出流只写，并设置版本
     QDataStream out(&message,QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_5_14);
     out << str;
     //遍历客户端，嵌套字写入
     for(int i = 0; i < clients_Chatnews->length(); i++){
         clients_Chatnews->at(i)->write(message);
     }
     qDebug() << "Server_send:" << str;
     qDebug() << "sendMessage_Chatnews:发送完成";
}

//删除
void Server::lostClient_Chatnews(){
    qDebug() << "lostClient_Chatnews:关闭聊天";
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    clients_Chatnews->removeOne(client);
    qDebug() << "lostClient_Chatnews:colse";
}

/***************************反馈界面初始信息,账号注册，退出 端口7777****************************/
//新客户端连接
void Server::newConnect_surface(){
    if(!listenFlag_surface) return;
    qDebug() << "新客户端连接";

    //得到新连接进来的socket
    client_surface = server_surface->nextPendingConnection();

    //添加到聊天列表
    clients_surface->append(client_surface);
    clientJoinUp_surface = true;

    //有可读信息发送readyRead()信号
    connect(client_surface,SIGNAL(readyRead()),
            this, SLOT(readMessage_surface()));
    //断开连接时
    connect(client_surface, &QTcpSocket::disconnected,
            this, &Server::lostClient_surface);
}

//读取
void Server::readMessage_surface(){
    qDebug() << "readMessage_surface:读取信息";
    QString message;            //全部消息
    QString flag;               //标志
    QString account;            //账号
    //遍历客户端列表，所有客户端
    for(int i = 0; i < clients_surface->length(); i++){
        qDebug() << "QDataStream";
        //设置响应socket的输入流和版本
        QDataStream in(clients_surface->at(i));
        in.setVersion(QDataStream::Qt_5_14);
        //读取信息，信息写入message
        in >> message;
        //每一个可读信息，message非空，跳出
        if(!message.isEmpty()){
            qDebug() << "break";
            break;
        }
    }
    qDebug() << "readMessage_surface:" << message;
    flag = message.section('|',0,0);
    account = message.section('|',1,1);
    //添加到显示面板
    if(flag == "initSurface"){
        //包含个人信息，和好友列表
        qDebug() << "请求初始化界面";
        ui->display_screen->append("请求获取界面");
        ui->display_screen->setAlignment(Qt::AlignCenter);
        ui->display_screen->append("登陆账号：" + account);

        QString initAccount;
        //数据库查找
        user->query->exec("select * from User");
        while(user->query->next()){
            initAccount = user->query->value(0).toString();
            if(account == initAccount){
                QString accountTemp = user->query->value(0).toString();
                QString nameTemp = user->query->value(1).toString();
                QString signTemp = user->query->value(3).toString();
                QString headTemp = user->query->value(4).toString();
                initMsg = "initSurface|" + accountTemp + "|" + nameTemp + "|"
                        + signTemp + "|" + headTemp;
                qDebug() << "界面初始化消息:" << initMsg;
                sendMessage_surface(initMsg);                       //发送初始化消息
                break;
            }
        }
        ui->display_screen->append("登陆成功！");
    }else if(flag == "logout"){
        ui->display_screen->append("请求退出");
        ui->display_screen->setAlignment(Qt::AlignCenter);
        ui->display_screen->append("退出账号：" + account);
        QString logoutAccount;
        //数据库查找
        user->query->exec("select *  from User");
        while(user->query->next()){
            logoutAccount = user->query->value(0).toString();
            if(account == logoutAccount){
                //更新模型视图
                user->model->setQuery("update user set state = 0 where account = " + account);
                user->model->setQuery("select * from User");
                qDebug() << "退出成功";
                break;
            }
        }
        ui->display_screen->append("退出成功");
        initMsg = "logout|" + account +"|null|null|null";
    }else if(flag == "register"){
        //新用户注册，服务器生成'账号.db'的好友列表，登录时返回给用户
        ui->display_screen->append("请求注册");
        ui->display_screen->setAlignment(Qt::AlignCenter);
        QString accountTemp = user->addUser(message);
        qDebug() << "注册成功";
        ui->display_screen->append("账号:"+accountTemp+"注册成功");
        sendMessage_surface("register|"+accountTemp);                       //发送初始化消息
    }else if(flag == "findpassword"){
        qDebug() << "找回密码";
        ui->display_screen->append("找回密码");
        ui->display_screen->setAlignment(Qt::AlignCenter);
        QString initAccount;
        //数据库查找
        user->query->exec("select * from User");
        while(user->query->next()){
            initAccount = user->query->value(0).toString();
            if(account == initAccount){
                QString accountTemp = user->query->value(0).toString();
                QString password = user->query->value(2).toString();
                initMsg = "findpassword|" + accountTemp + "|" + password + "|"
                        + "null" + "|" + "null";
                qDebug() << "账号:" << accountTemp;
                qDebug() << "找回密码:" << password;
                qDebug() << "找回成功";
                ui->display_screen->append("账号:"+account);
                ui->display_screen->append("找回密码:"+password);
                sendMessage_surface(initMsg);                       //发送初始化消息
                break;
            }
        }
    }
    qDebug() << "Server_read:" << message;
    qDebug() << "readMessage_surface:读取完成";
}

void Server::sendMessage_surface(QString Msg){
    if(!listenFlag_surface) {
         qDebug() << "sendMessage_surface:服务端未打开";
         return;
     }
     if(!clientJoinUp_surface) {
         qDebug() << "sendMessage_surface:客户端未连接";
         return;
     }
     QString flag;
     flag = Msg.section('|', 0, 0);
     qDebug() << "信息为：" << Msg;
     if(flag != "initSurface" && flag != "logout" &&
            flag != "findpassword" && flag != "register"){
        qDebug() << "sendMessage_surface:错误信息,不发送";
        qDebug() << "信息为:" << Msg;
        return;
     }
     qDebug() << "sendMessage_surface:发送消息";
     QByteArray message;
     //设置输出流只写，并设置版本
     QDataStream out(&message,QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_5_14);
     out << Msg;
     //遍历客户端，套接字写入
     for (int i = 0;i < clients_surface->length();i++)
     {
         clients_surface->at(i)->write(message);
     }
     qDebug() << "Server_send:" <<Msg;
     qDebug() << "sendMessage_surface:发送完成";
}

void Server::lostClient_surface(){
    qDebug() << "lostClient_surface:销毁";
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    clients_surface->removeOne(client);
    qDebug() << "lostClient_surface:关闭";
}

/*********************************************************************************/


//释放服务器
void Server::destoryServer_check(){
    qDebug() << "释放服务器";
    listenFlag_check = false;
    listenFlag_news = false;
    listenFlag_surface = false;
    listenFlag_Chatnews = false;
}

//释放资源
Server::~Server()
{
    delete ui;
    //释放
    delete server_check;
    delete server_news;
    delete server_surface;
    qDebug() << "释放server";
}

void Server::on_pushButton_clicked()
{
    user->initDB();
}
