#include "ReservationRepository.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include "../db/DatabaseManager.h"

ReservationRepository &ReservationRepository::instance() {
    static ReservationRepository inst;
    return inst;
}

static Reservation mapReservation(const QSqlQuery &q) {
    Reservation r;
    r.id = q.value("id").toInt();
    r.bookId = q.value("book_id").toInt();
    r.readerId = q.value("reader_id").toInt();
    r.createTime = QDateTime::fromSecsSinceEpoch(q.value("create_time").toLongLong());
    r.status = q.value("status").toInt();
    return r;
}

bool ReservationRepository::create(Reservation &r) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO reservations(book_id,reader_id,create_time,status) VALUES(:b,:r,:t,:s)");
    q.bindValue(":b", r.bookId);
    q.bindValue(":r", r.readerId);
    q.bindValue(":t", QDateTime::currentSecsSinceEpoch());
    q.bindValue(":s", r.status);
    if (!q.exec()) return false;
    r.id = q.lastInsertId().toInt();
    r.createTime = QDateTime::fromSecsSinceEpoch(QDateTime::currentSecsSinceEpoch());
    return true;
}

bool ReservationRepository::update(const Reservation &r) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE reservations SET book_id=:b, reader_id=:r, create_time=:t, status=:s WHERE id=:id");
    q.bindValue(":b", r.bookId);
    q.bindValue(":r", r.readerId);
    q.bindValue(":t", r.createTime.toSecsSinceEpoch());
    q.bindValue(":s", r.status);
    q.bindValue(":id", r.id);
    return q.exec();
}

QVector<Reservation> ReservationRepository::getByReader(int readerId) {
    QVector<Reservation> list;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM reservations WHERE reader_id=:r ORDER BY create_time DESC");
    q.bindValue(":r", readerId);
    if (q.exec()) while (q.next()) list.push_back(mapReservation(q));
    return list;
}

QVector<Reservation> ReservationRepository::getByBook(int bookId) {
    QVector<Reservation> list;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM reservations WHERE book_id=:b ORDER BY create_time ASC");
    q.bindValue(":b", bookId);
    if (q.exec()) while (q.next()) list.push_back(mapReservation(q));
    return list;
}


