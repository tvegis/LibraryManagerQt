#pragma once
#include <QString>
#include <optional>
#include "../models/Loan.h"

class LibraryService {
public:
    static LibraryService &instance();

    // 借阅还书续借
    std::optional<QString> borrowBook(int readerId, int bookId);
    std::optional<QString> returnBook(int loanId);
    std::optional<QString> renewLoan(int loanId);

private:
    LibraryService() = default;
};


