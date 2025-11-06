#pragma once
#include <QString>
#include <QDateTime>

struct Message {
    int id = 0;
    int readerId = 0; // 0 表示系统公告
    QString content;
    QDateTime createTime;
    int read = 0;
};


