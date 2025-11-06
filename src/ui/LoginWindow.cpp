#include "LoginWindow.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include "../services/AuthService.h"

LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("登录"));
    auto *root = new QVBoxLayout(this);
    auto *form = new QFormLayout();
    usernameEdit_ = new QLineEdit(this);
    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);
    form->addRow(tr("账号"), usernameEdit_);
    form->addRow(tr("密码"), passwordEdit_);
    root->addLayout(form);

    adminRadio_ = new QRadioButton(tr("管理员"), this);
    readerRadio_ = new QRadioButton(tr("读者"), this);
    readerRadio_->setChecked(true);
    auto *rbLayout = new QHBoxLayout();
    rbLayout->addWidget(readerRadio_);
    rbLayout->addWidget(adminRadio_);
    root->addLayout(rbLayout);

    loginBtn_ = new QPushButton(tr("登录"), this);
    root->addWidget(loginBtn_);
    connect(loginBtn_, &QPushButton::clicked, this, &LoginWindow::onLogin);
}

void LoginWindow::onLogin() {
    const QString u = usernameEdit_->text().trimmed();
    const QString p = passwordEdit_->text();
    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入账号与密码"));
        return;
    }
    if (adminRadio_->isChecked()) {
        auto r = AuthService::instance().loginAdmin(u, p);
        if (!r.ok) { QMessageBox::warning(this, tr("失败"), r.message); return; }
        accept();
        emit loginAdminSuccess();
    } else {
        auto r = AuthService::instance().loginReader(u, p);
        if (!r.ok) { QMessageBox::warning(this, tr("失败"), r.message); return; }
        accept();
        emit loginReaderSuccess(r.reader->id);
    }
}


