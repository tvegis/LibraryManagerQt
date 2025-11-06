#include <QApplication>
#include <QMessageBox>
#include "db/DatabaseManager.h"
#include "ui/LoginWindow.h"
#include "ui/AdminMainWindow.h"
#include "ui/ReaderMainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DatabaseManager &db = DatabaseManager::instance();
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("数据库错误"), QObject::tr("无法打开数据库，请检查权限或路径。"));
        return -1;
    }
    if (!db.initializeSchema()) {
        QMessageBox::critical(nullptr, QObject::tr("初始化错误"), QObject::tr("初始化数据库结构失败。"));
        return -1;
    }

    LoginWindow login;
    QObject::connect(&login, &LoginWindow::loginAdminSuccess, [&]() {
        auto *w = new AdminMainWindow();
        w->show();
    });
    QObject::connect(&login, &LoginWindow::loginReaderSuccess, [&](int readerId) {
        auto *w = new ReaderMainWindow(readerId);
        w->show();
    });
    login.show();
    return app.exec();
}


