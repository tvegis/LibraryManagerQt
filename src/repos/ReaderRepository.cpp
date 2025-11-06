#include "ReaderRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "../db/DatabaseManager.h"

ReaderRepository &ReaderRepository::instance() {
    static ReaderRepository inst;
    return inst;
}

static Reader mapReader(const QSqlQuery &q) {
    Reader r;
    r.id = q.value("id").toInt();
    r.username = q.value("username").toString();
    r.passwordHash = q.value("password_hash").toString();
    r.name = q.value("name").toString();
    r.gender = q.value("gender").toString();
    r.idCard = q.value("id_card").toString();
    r.phone = q.value("phone").toString();
    r.status = q.value("status").toInt();
    return r;
}

std::optional<Reader> ReaderRepository::getById(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM readers WHERE id=:id");
    q.bindValue(":id", id);
    if (q.exec() && q.next()) return mapReader(q);
    return std::nullopt;
}

std::optional<Reader> ReaderRepository::getByUsername(const QString &username) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM readers WHERE username=:u");
    q.bindValue(":u", username);
    if (q.exec() && q.next()) return mapReader(q);
    return std::nullopt;
}

bool ReaderRepository::create(Reader &reader) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO readers(username,password_hash,name,gender,id_card,phone,status)\n"
              "VALUES(:u,:p,:n,:g,:idc,:ph,:st)");
    q.bindValue(":u", reader.username);
    q.bindValue(":p", reader.passwordHash);
    q.bindValue(":n", reader.name);
    q.bindValue(":g", reader.gender);
    q.bindValue(":idc", reader.idCard);
    q.bindValue(":ph", reader.phone);
    q.bindValue(":st", reader.status);
    if (!q.exec()) return false;
    reader.id = q.lastInsertId().toInt();
    return true;
}

bool ReaderRepository::update(const Reader &reader) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE readers SET username=:u,password_hash=:p,name=:n,gender=:g,id_card=:idc,phone=:ph,status=:st WHERE id=:id");
    q.bindValue(":u", reader.username);
    q.bindValue(":p", reader.passwordHash);
    q.bindValue(":n", reader.name);
    q.bindValue(":g", reader.gender);
    q.bindValue(":idc", reader.idCard);
    q.bindValue(":ph", reader.phone);
    q.bindValue(":st", reader.status);
    q.bindValue(":id", reader.id);
    return q.exec();
}

bool ReaderRepository::remove(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("DELETE FROM readers WHERE id=:id");
    q.bindValue(":id", id);
    return q.exec();
}

QVector<Reader> ReaderRepository::search(const QString &keyword) {
    QVector<Reader> list;
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = "SELECT * FROM readers WHERE 1=1";
    if (!keyword.isEmpty()) sql += " AND (username LIKE :kw OR name LIKE :kw OR phone LIKE :kw)";
    q.prepare(sql);
    if (!keyword.isEmpty()) q.bindValue(":kw", "%" + keyword + "%");
    if (q.exec()) {
        while (q.next()) list.push_back(mapReader(q));
    }
    return list;
}


