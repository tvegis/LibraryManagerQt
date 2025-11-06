#include "LoanRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include "../db/DatabaseManager.h"

LoanRepository &LoanRepository::instance() {
    static LoanRepository inst;
    return inst;
}

static Loan mapLoan(const QSqlQuery &q) {
    Loan l;
    l.id = q.value("id").toInt();
    l.bookId = q.value("book_id").toInt();
    l.readerId = q.value("reader_id").toInt();
    l.borrowTime = QDateTime::fromSecsSinceEpoch(q.value("borrow_time").toLongLong());
    l.dueTime = QDateTime::fromSecsSinceEpoch(q.value("due_time").toLongLong());
    const QVariant rt = q.value("return_time");
    if (!rt.isNull()) l.returnTime = QDateTime::fromSecsSinceEpoch(rt.toLongLong());
    l.renewTimes = q.value("renew_times").toInt();
    l.fine = q.value("fine").toDouble();
    return l;
}

bool LoanRepository::create(Loan &loan) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO loans(book_id,reader_id,borrow_time,due_time,return_time,renew_times,fine)\n"
              "VALUES(:b,:r,:bt,:dt,NULL,:rt,:f)");
    q.bindValue(":b", loan.bookId);
    q.bindValue(":r", loan.readerId);
    q.bindValue(":bt", loan.borrowTime.toSecsSinceEpoch());
    q.bindValue(":dt", loan.dueTime.toSecsSinceEpoch());
    q.bindValue(":rt", loan.renewTimes);
    q.bindValue(":f", loan.fine);
    if (!q.exec()) return false;
    loan.id = q.lastInsertId().toInt();
    return true;
}

bool LoanRepository::update(const Loan &loan) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE loans SET book_id=:b,reader_id=:r,borrow_time=:bt,due_time=:dt,return_time=:rt,renew_times=:rn,fine=:f WHERE id=:id");
    q.bindValue(":b", loan.bookId);
    q.bindValue(":r", loan.readerId);
    q.bindValue(":bt", loan.borrowTime.toSecsSinceEpoch());
    q.bindValue(":dt", loan.dueTime.toSecsSinceEpoch());
    q.bindValue(":rt", loan.returnTime.isValid() ? QVariant::fromValue<long long>(loan.returnTime.toSecsSinceEpoch()) : QVariant());
    q.bindValue(":rn", loan.renewTimes);
    q.bindValue(":f", loan.fine);
    q.bindValue(":id", loan.id);
    return q.exec();
}

std::optional<Loan> LoanRepository::getById(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM loans WHERE id=:id");
    q.bindValue(":id", id);
    if (q.exec() && q.next()) return mapLoan(q);
    return std::nullopt;
}

QVector<Loan> LoanRepository::getActiveByReader(int readerId) {
    QVector<Loan> list;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM loans WHERE reader_id=:r AND return_time IS NULL");
    q.bindValue(":r", readerId);
    if (q.exec()) while (q.next()) list.push_back(mapLoan(q));
    return list;
}

QVector<Loan> LoanRepository::getHistoryByReader(int readerId) {
    QVector<Loan> list;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM loans WHERE reader_id=:r AND return_time IS NOT NULL ORDER BY borrow_time DESC");
    q.bindValue(":r", readerId);
    if (q.exec()) while (q.next()) list.push_back(mapLoan(q));
    return list;
}

QVector<Loan> LoanRepository::getOverdue() {
    QVector<Loan> list;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM loans WHERE return_time IS NULL AND due_time < :now");
    q.bindValue(":now", QDateTime::currentSecsSinceEpoch());
    if (q.exec()) while (q.next()) list.push_back(mapLoan(q));
    return list;
}


