#pragma once
#include <QString>

struct Admin {
    int id = 0;
    QString username;
    QString passwordHash;
    int role = 1; // 9=超级管理员, 1=普通管理员
};


