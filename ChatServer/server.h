#ifndef SERVER_H
#define SERVER_H

#include "userlist.h"

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>


QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

private slots:
    void on_startButton_clicked();
    void on_send_button_clicked();
    void on_clear_button_clicked();
    void on_close_button_clicked();

    //账号检测
    void newClient_check();                             //账号登录
    void lostClient_check();                            //删除在线的人
    void readMessage_check();                           //读取账号信息
    void sendMessage_check();                           //发送信息

    //消息转发
    void newConnect_news();                             //新客户端连接
    void sendMessage_news(QString information);         //发送消息
    void readMessage_news();                            //接收消息
    void lostClient_news();                             //删除聊天

    //初始化界面
    void newConnect_surface();                          //新客户端连接
    void sendMessage_surface(QString initMsg);          //发送消息
    void readMessage_surface();                         //接收消息
    void lostClient_surface();                          //删除

    //群聊界面
    void newConnect_Chatnews();                             //新客户端连接
    void sendMessage_Chatnews(QString information);         //发送消息
    void readMessage_Chatnews();                            //接收消息
    void lostClient_Chatnews();                             //删除聊天

    //服务器销毁
    void destoryServer_check();

    void on_pushButton_clicked();

private:
    Ui::Server *ui;
    UserList *user;

    int people;

    /************************账号检测 5555端口********************/
    //network
    QTcpServer *server_check;                           //服务
    QTcpSocket *client_check;                           //临时客户端
    QList<QTcpSocket *> clients_check;                  //客户端列
    //标志
    bool listenFlag_check = false;                      //5555端口监听
    bool clientJoinUp_check = false;                    //客户端加
    bool loginSuccessFlag_check = false;                //登陆成功标志
    //反馈消息
    void feedbackMessage(QString msg);
    /**********************转发消息 6666端口**********************/
    //network
    QTcpServer *server_news;                           //服务器
    QTcpSocket *client_news;                           //临时客户
    QList<QTcpSocket *> *clients_news;                 //客户端列表
    //标志
    bool listenFlag_news = false;                      //6666端口监听
    bool clientJoinUp_news = false;                    //客户端加入
    bool loginSuccessFlag_news = false;                //登陆成功标志
    /********************初始化界面 7777端口********************/
    //network
    QTcpServer *server_surface;                         //服务器
    QTcpSocket *client_surface;                         //临时客户端
    QList<QTcpSocket *> *clients_surface;               //客户端列表
    QString initMsg;                                   //初始化界面的消息
    //标志
    bool listenFlag_surface = false;                    //7777端口监听
    bool clientJoinUp_surface = false;                  //客户端加入
    bool loginSuccessFlag_surface = false;              //登陆成功标志
    /*******************群聊 8888端口****************************/
    //network
    QTcpServer *server_Chatnews;                           //服务器
    QTcpSocket *client_Chatnews;                           //临时客户
    QList<QTcpSocket *> *clients_Chatnews;                 //客户端列表
    //标志
    bool listenFlag_Chatnews = false;                      //8888端口监听
    bool clientJoinUp_Chatnews = false;                    //客户端加入
    bool loginSuccessFlag_Chatnews = false;                //登陆成功标志

private:

};
#endif // SERVER_H
