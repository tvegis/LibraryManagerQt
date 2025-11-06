#pragma once
#include <QVector>
#include <optional>
#include "../models/Reader.h"

class ReaderRepository {
public:
    static ReaderRepository &instance();

    std::optional<Reader> getById(int id);
    std::optional<Reader> getByUsername(const QString &username);
    bool create(Reader &reader);
    bool update(const Reader &reader);
    bool remove(int id);

    QVector<Reader> search(const QString &keyword);
};


