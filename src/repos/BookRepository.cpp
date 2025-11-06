#include "BookRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "../db/DatabaseManager.h"

BookRepository &BookRepository::instance() {
    static BookRepository inst;
    return inst;
}

static Book mapBook(const QSqlQuery &q) {
    Book b;
    b.id = q.value("id").toInt();
    b.isbn = q.value("isbn").toString();
    b.title = q.value("title").toString();
    b.author = q.value("author").toString();
    b.publisher = q.value("publisher").toString();
    b.category = q.value("category").toString();
    b.price = q.value("price").toDouble();
    b.totalCount = q.value("total_count").toInt();
    b.availableCount = q.value("available_count").toInt();
    b.summary = q.value("summary").toString();
    b.status = q.value("status").toInt();
    return b;
}

std::optional<Book> BookRepository::getById(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM books WHERE id=:id");
    q.bindValue(":id", id);
    if (q.exec() && q.next()) return mapBook(q);
    return std::nullopt;
}

std::optional<Book> BookRepository::getByIsbn(const QString &isbn) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT * FROM books WHERE isbn=:isbn");
    q.bindValue(":isbn", isbn);
    if (q.exec() && q.next()) return mapBook(q);
    return std::nullopt;
}

bool BookRepository::create(Book &book) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO books(isbn,title,author,publisher,category,price,total_count,available_count,summary,status)\n"
              "VALUES(:isbn,:title,:author,:publisher,:category,:price,:total,:avail,:summary,:status)");
    q.bindValue(":isbn", book.isbn);
    q.bindValue(":title", book.title);
    q.bindValue(":author", book.author);
    q.bindValue(":publisher", book.publisher);
    q.bindValue(":category", book.category);
    q.bindValue(":price", book.price);
    q.bindValue(":total", book.totalCount);
    q.bindValue(":avail", book.availableCount);
    q.bindValue(":summary", book.summary);
    q.bindValue(":status", book.status);
    if (!q.exec()) return false;
    book.id = q.lastInsertId().toInt();
    return true;
}

bool BookRepository::update(const Book &book) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE books SET isbn=:isbn,title=:title,author=:author,publisher=:publisher,category=:category,price=:price,\n"
              " total_count=:total, available_count=:avail, summary=:summary, status=:status WHERE id=:id");
    q.bindValue(":isbn", book.isbn);
    q.bindValue(":title", book.title);
    q.bindValue(":author", book.author);
    q.bindValue(":publisher", book.publisher);
    q.bindValue(":category", book.category);
    q.bindValue(":price", book.price);
    q.bindValue(":total", book.totalCount);
    q.bindValue(":avail", book.availableCount);
    q.bindValue(":summary", book.summary);
    q.bindValue(":status", book.status);
    q.bindValue(":id", book.id);
    return q.exec();
}

bool BookRepository::remove(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("DELETE FROM books WHERE id=:id");
    q.bindValue(":id", id);
    return q.exec();
}

QVector<Book> BookRepository::search(const QString &keyword, const QString &category,
                                     const QString &author, const QString &isbn) {
    QVector<Book> list;
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = "SELECT * FROM books WHERE 1=1";
    if (!keyword.isEmpty()) sql += " AND (title LIKE :kw OR author LIKE :kw)";
    if (!category.isEmpty()) sql += " AND category=:cat";
    if (!author.isEmpty()) sql += " AND author LIKE :author";
    if (!isbn.isEmpty()) sql += " AND isbn=:isbn";
    q.prepare(sql);
    if (!keyword.isEmpty()) q.bindValue(":kw", "%" + keyword + "%");
    if (!category.isEmpty()) q.bindValue(":cat", category);
    if (!author.isEmpty()) q.bindValue(":author", "%" + author + "%");
    if (!isbn.isEmpty()) q.bindValue(":isbn", isbn);
    if (q.exec()) {
        while (q.next()) list.push_back(mapBook(q));
    }
    return list;
}

bool BookRepository::increaseStock(int bookId, int deltaTotal, int deltaAvailable) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE books SET total_count=total_count+:dt, available_count=available_count+:da WHERE id=:id");
    q.bindValue(":dt", deltaTotal);
    q.bindValue(":da", deltaAvailable);
    q.bindValue(":id", bookId);
    return q.exec();
}

bool BookRepository::decreaseAvailable(int bookId, int count) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE books SET available_count=available_count-:c WHERE id=:id AND available_count>=:c");
    q.bindValue(":c", count);
    q.bindValue(":id", bookId);
    return q.exec();
}

bool BookRepository::increaseAvailable(int bookId, int count) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE books SET available_count=available_count+:c WHERE id=:id");
    q.bindValue(":c", count);
    q.bindValue(":id", bookId);
    return q.exec();
}


