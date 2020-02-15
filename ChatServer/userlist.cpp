#include "userlist.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMainWindow>
#include <QMainWindow>
#include <QTableView>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QDebug>
#include <QTime>



UserList::UserList()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("UserList.db");
    //打开
    if(!db.open()){
          QMessageBox::critical(0,
                              "Cannot open UserList.db",
                               "Unable to establish a database connection.",
                               QMessageBox::Cancel);
          return;
     }
    //指定连接
    query = new QSqlQuery(db);
    query->exec("create table User("
                "account varchar(20) primary key,"
                "name varchar(20) not null,"
                "password varchar(20) not null,"
                "sign varchar(20),"
                "headImage varchar(10),"
                "state bit default 0)");
    query->exec("insert into User values('12345','first','123','I am first','head1.png',0)");
    query->exec("insert into User values('23456','second','123','I am second','head2.png',0)");
    query->exec("insert into User values('34567','third','123','I am third','head3.png',0)");
    query->exec("insert into User values('45678','fourth','123','I am fourth','head3.png',0)");
    //得到指向数据库的指针
    db = QSqlDatabase::database();
    //数据库模型视图
    QMainWindow *sqlWindow = new QMainWindow();
    sqlWindow->resize(750,300);
    sqlWindow->setMinimumWidth(650);
    sqlWindow->setMinimumHeight(300);
    sqlWindow->setWindowTitle("UserList");
    //定制模型
    model = new QSqlQueryModel(sqlWindow);
    model->setQuery("select * from User");
    model->setHeaderData(0,Qt::Horizontal,"账号");
    model->setHeaderData(1,Qt::Horizontal,"用户名");
    model->setHeaderData(2,Qt::Horizontal,"密码");
    model->setHeaderData(3,Qt::Horizontal,"个性签名");
    model->setHeaderData(4,Qt::Horizontal,"头像");
    model->setHeaderData(5,Qt::Horizontal,"在线状态");
    //设置视图
    QTableView *view = new QTableView(sqlWindow);
    view->setModel(model);
    sqlWindow->setCentralWidget(view);
    sqlWindow->show();
}

int UserList::CheckUser(QString account, QString password){
    query->exec("select * from User");
    while(query->next()){
        QString RealAccount = query->value(0).toString();
        QString RealPassword = query->value(2).toString();
        if(account == RealAccount && password == RealPassword){
            //不能重复登陆
            if(query->value(5).toString() == "1"){
                return 2;
            }

            //更新模型视图
            model->setQuery("update user set state = 1 where account = " + account);
            model->setQuery("select * from User");
            return 1;
        }
    }
    return 0;
}

QString UserList::addUser(QString message){
    QStringList msg = message.split("|");
    qDebug() << "总长度: " << msg.length();
    //临时信息
    QString accountTemp;
    QString nameTemp;
    QString passwordTemp;
    QString signTemp;
    QString headTemp;
    QString s;
    nameTemp = msg[1];
    passwordTemp = msg[2];
    signTemp = msg[3];
    headTemp = msg[4];
    //随机生成数据库中没有的五位数并返回
    bool uniqueFlag = true;
    QString exitAccount;
    QSqlQuery query(db);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));          //随机数种子
    while(true){
        //随机数生成
        for(int i = 0; i < 5; i++){
            int rand = qrand() % 10;                                    //产生10以内的随机数
            QString s = QString::number(rand, 10);                      //转化为10进制，再转化为字符
            accountTemp += s;
            qDebug() << i <<":accountTemp=" << accountTemp;
        }
        uniqueFlag = true;
        query.exec("select * from User");
        while(query.next()){
            exitAccount = query.value(0).toString();
            if(accountTemp == exitAccount){
                uniqueFlag = false;
                break;
            }
        }
        //独一无二的账号跳出
        if(uniqueFlag == true)
            break;
    }
    qDebug() << "accountTemp:"+accountTemp;
    qDebug() << "nameTemp:"+nameTemp;
    qDebug() << "passwordTemp:"+passwordTemp;
    qDebug() << "signTemp:"+signTemp;
    qDebug() << "headTemp:"+headTemp;

    s = "insert into User values("+
            accountTemp+"," +
            nameTemp+","+
            passwordTemp+","+
            signTemp+",'"+
            headTemp+"',"+
            "0)";
    qDebug() << s;
    //数据库插入
    //注意英文字符要加''单引号，数字可以不加
    model->setQuery("insert into User values('"+
                    accountTemp+"','" +
                    nameTemp+"','"+
                    passwordTemp+"','"+
                    signTemp+"','"+
                    headTemp+"',"+
                    "0)");
    model->setQuery("select * from User");
    return accountTemp;
}

void UserList::initDB(){
    query->exec("update user set state = 0");
    model->setQuery("select * from User");
}
