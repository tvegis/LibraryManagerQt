#pragma once
#include <QVector>
#include <optional>
#include "../models/Book.h"

class BookRepository {
public:
    static BookRepository &instance();

    std::optional<Book> getById(int id);
    std::optional<Book> getByIsbn(const QString &isbn);
    bool create(Book &book);
    bool update(const Book &book);
    bool remove(int id);

    QVector<Book> search(const QString &keyword, const QString &category,
                         const QString &author, const QString &isbn);

    bool increaseStock(int bookId, int deltaTotal, int deltaAvailable);
    bool decreaseAvailable(int bookId, int count);
    bool increaseAvailable(int bookId, int count);
};


