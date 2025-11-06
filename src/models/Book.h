#pragma once
#include <QString>

struct Book {
    int id = 0;
    QString isbn;
    QString title;
    QString author;
    QString publisher;
    QString category;
    double price = 0.0;
    int totalCount = 0;
    int availableCount = 0;
    QString summary;
    int status = 1; // 1=上架, 0=下架
};


