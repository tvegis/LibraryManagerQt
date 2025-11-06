#pragma once
#include <QString>

struct Reader {
    int id = 0;
    QString username;
    QString passwordHash;
    QString name;
    QString gender;
    QString idCard;
    QString phone;
    int status = 1; // 1=正常, 0=注销
};


