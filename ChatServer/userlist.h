#ifndef USERLIST_H
#define USERLIST_H

#include<QSqlDatabase>
#include<QSqlQuery>
#include<QVariant>
#include<QSqlQueryModel>


class UserList
{
public:
    UserList();
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlQueryModel *model;
    QString addUser(QString message);
    int CheckUser(QString account, QString password);
    void initDB();
};

#endif // USERLIST_H
