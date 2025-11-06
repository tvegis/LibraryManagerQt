#pragma once

#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager &instance();

    bool open();
    bool initializeSchema();
    QSqlDatabase database() const;

private:
    DatabaseManager();
    bool execBatch(const QList<QString> &sqlList, QString *errorOut = nullptr);

    QSqlDatabase db_;
};


