#pragma once
#include <QDateTime>

struct Reservation {
    int id = 0;
    int bookId = 0;
    int readerId = 0;
    QDateTime createTime;
    int status = 0; // 0=待通知,1=已通知,2=完成,3=取消
};


