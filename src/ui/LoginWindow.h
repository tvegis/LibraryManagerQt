#pragma once
#include <QDialog>

class QLineEdit;
class QRadioButton;
class QPushButton;

class LoginWindow : public QDialog {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginAdminSuccess();
    void loginReaderSuccess(int readerId);

private slots:
    void onLogin();

private:
    QLineEdit *usernameEdit_ = nullptr;
    QLineEdit *passwordEdit_ = nullptr;
    QRadioButton *adminRadio_ = nullptr;
    QRadioButton *readerRadio_ = nullptr;
    QPushButton *loginBtn_ = nullptr;
};


