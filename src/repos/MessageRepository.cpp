#include "MessageRepository.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include "../db/DatabaseManager.h"

MessageRepository &MessageRepository::instance() {
    static MessageRepository inst;
    return inst;
}

static Message mapMessage(const QSqlQuery &q) {
    Message m;
    m.id = q.value("id").toInt();
    m.readerId = q.value("reader_id").toInt();
    m.content = q.value("content").toString();
    m.createTime = QDateTime::fromSecsSinceEpoch(q.value("create_time").toLongLong());
    m.read = q.value("read").toInt();
    return m;
}

bool MessageRepository::create(Message &m) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO messages(reader_id,content,create_time,read) VALUES(:r,:c,:t,0)");
    q.bindValue(":r", m.readerId);
    q.bindValue(":c", m.content);
    q.bindValue(":t", QDateTime::currentSecsSinceEpoch());
    if (!q.exec()) return false;
    m.id = q.lastInsertId().toInt();
    m.createTime = QDateTime::fromSecsSinceEpoch(QDateTime::currentSecsSinceEpoch());
    m.read = 0;
    return true;
}

bool MessageRepository::markRead(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE messages SET read=1 WHERE id=:id");
    q.bindValue(":id", id);
    return q.exec();
}

QVector<Message> MessageRepository::getByReader(int readerId) {
    QVector<Message> list;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM messages WHERE reader_id=:r ORDER BY create_time DESC");
    q.bindValue(":r", readerId);
    if (q.exec()) while (q.next()) list.push_back(mapMessage(q));
    return list;
}


