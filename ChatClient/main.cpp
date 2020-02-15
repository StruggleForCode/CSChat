#include "login.h"
#include "user.h"
#include "selectuserinterface.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login w;
    w.show();
    Login w2;
    w2.show();
    Login w3;
    w3.show();
//    User w;
//    w.show();
//    SelectUserInterface w;
//    w.show();
    return a.exec();
}
