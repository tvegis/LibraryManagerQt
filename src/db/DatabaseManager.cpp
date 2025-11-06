#include "DatabaseManager.h"
#include "../AppConfig.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

DatabaseManager &DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager() {
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(AppConfig::instance().databaseFilePath());
}

bool DatabaseManager::open() {
    if (db_.isOpen()) return true;
    return db_.open();
}

QSqlDatabase DatabaseManager::database() const {
    return db_;
}

bool DatabaseManager::execBatch(const QList<QString> &sqlList, QString *errorOut) {
    QSqlQuery query(db_);
    for (const QString &sql : sqlList) {
        if (!query.exec(sql)) {
            if (errorOut) *errorOut = query.lastError().text();
            return false;
        }
    }
    return true;
}

bool DatabaseManager::initializeSchema() {
    QString err;
    const QList<QString> stmts = {
        // 基础表：图书
        "CREATE TABLE IF NOT EXISTS books (\n"
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
        "  isbn TEXT UNIQUE NOT NULL,\n"
        "  title TEXT NOT NULL,\n"
        "  author TEXT,\n"
        "  publisher TEXT,\n"
        "  category TEXT,\n"
        "  price REAL DEFAULT 0,\n"
        "  total_count INTEGER DEFAULT 0,\n"
        "  available_count INTEGER DEFAULT 0,\n"
        "  summary TEXT,\n"
        "  status INTEGER DEFAULT 1\n"
        ")",

        // 读者
        "CREATE TABLE IF NOT EXISTS readers (\n"
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
        "  username TEXT UNIQUE NOT NULL,\n"
        "  password_hash TEXT NOT NULL,\n"
        "  name TEXT,\n"
        "  gender TEXT,\n"
        "  id_card TEXT,\n"
        "  phone TEXT,\n"
        "  status INTEGER DEFAULT 1\n"
        ")",

        // 管理员
        "CREATE TABLE IF NOT EXISTS admins (\n"
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
        "  username TEXT UNIQUE NOT NULL,\n"
        "  password_hash TEXT NOT NULL,\n"
        "  role INTEGER DEFAULT 1\n"
        ")",

        // 借阅记录
        "CREATE TABLE IF NOT EXISTS loans (\n"
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
        "  book_id INTEGER NOT NULL,\n"
        "  reader_id INTEGER NOT NULL,\n"
        "  borrow_time INTEGER NOT NULL,\n"
        "  due_time INTEGER NOT NULL,\n"
        "  return_time INTEGER,\n"
        "  renew_times INTEGER DEFAULT 0,\n"
        "  fine REAL DEFAULT 0,\n"
        "  FOREIGN KEY(book_id) REFERENCES books(id),\n"
        "  FOREIGN KEY(reader_id) REFERENCES readers(id)\n"
        ")",

        // 预约
        "CREATE TABLE IF NOT EXISTS reservations (\n"
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
        "  book_id INTEGER NOT NULL,\n"
        "  reader_id INTEGER NOT NULL,\n"
        "  create_time INTEGER NOT NULL,\n"
        "  status INTEGER DEFAULT 0,\n"
        "  FOREIGN KEY(book_id) REFERENCES books(id),\n"
        "  FOREIGN KEY(reader_id) REFERENCES readers(id)\n"
        ")",

        // 消息通知
        "CREATE TABLE IF NOT EXISTS messages (\n"
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
        "  reader_id INTEGER,\n"
        "  content TEXT NOT NULL,\n"
        "  create_time INTEGER NOT NULL,\n"
        "  read INTEGER DEFAULT 0,\n"
        "  FOREIGN KEY(reader_id) REFERENCES readers(id)\n"
        ")",

        // 初始管理员（如无则创建一个默认：admin/admin）
        "INSERT INTO admins(username, password_hash, role)\n"
        "  SELECT 'admin', 'admin', 9 WHERE NOT EXISTS(SELECT 1 FROM admins)"
    };

    if (!execBatch(stmts, &err)) {
        return false;
    }
    return true;
}


