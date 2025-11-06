#pragma once
#include <QVector>
#include <optional>
#include "../models/Loan.h"

class LoanRepository {
public:
    static LoanRepository &instance();

    bool create(Loan &loan);
    bool update(const Loan &loan);
    std::optional<Loan> getById(int id);
    QVector<Loan> getActiveByReader(int readerId);
    QVector<Loan> getHistoryByReader(int readerId);
    QVector<Loan> getOverdue();
};


