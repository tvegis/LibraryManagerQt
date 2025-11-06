#pragma once
#include <QDateTime>

struct Loan {
    int id = 0;
    int bookId = 0;
    int readerId = 0;
    QDateTime borrowTime;
    QDateTime dueTime;
    QDateTime returnTime; // isNull 表示未归还
    int renewTimes = 0;
    double fine = 0.0;
};


