#include "LibraryService.h"
#include <QDateTime>
#include "../repos/BookRepository.h"
#include "../repos/LoanRepository.h"
#include "../AppConfig.h"

LibraryService &LibraryService::instance() {
    static LibraryService inst;
    return inst;
}

std::optional<QString> LibraryService::borrowBook(int readerId, int bookId) {
    auto bookOpt = BookRepository::instance().getById(bookId);
    if (!bookOpt) return QObject::tr("图书不存在");
    if (bookOpt->availableCount <= 0) return QObject::tr("库存不足，无法借阅");
    if (!BookRepository::instance().decreaseAvailable(bookId, 1)) return QObject::tr("更新库存失败");

    Loan loan;
    loan.bookId = bookId;
    loan.readerId = readerId;
    loan.borrowTime = QDateTime::currentDateTime();
    loan.dueTime = loan.borrowTime.addDays(AppConfig::instance().loanDaysDefault());
    loan.renewTimes = 0;
    loan.fine = 0.0;
    if (!LoanRepository::instance().create(loan)) return QObject::tr("创建借阅记录失败");
    return std::nullopt;
}

std::optional<QString> LibraryService::returnBook(int loanId) {
    auto loanOpt = LoanRepository::instance().getById(loanId);
    if (!loanOpt) return QObject::tr("借阅记录不存在");
    Loan loan = *loanOpt;
    if (loan.returnTime.isValid()) return QObject::tr("该记录已归还");

    loan.returnTime = QDateTime::currentDateTime();
    // 计算罚金
    if (loan.returnTime > loan.dueTime) {
        int days = loan.dueTime.daysTo(loan.returnTime);
        if (days < 0) days = 0;
        loan.fine = days * AppConfig::instance().finePerOverdueDay();
    }
    if (!LoanRepository::instance().update(loan)) return QObject::tr("更新归还记录失败");
    if (!BookRepository::instance().increaseAvailable(loan.bookId, 1)) return QObject::tr("更新库存失败");
    return std::nullopt;
}

std::optional<QString> LibraryService::renewLoan(int loanId) {
    auto loanOpt = LoanRepository::instance().getById(loanId);
    if (!loanOpt) return QObject::tr("借阅记录不存在");
    Loan loan = *loanOpt;
    if (loan.returnTime.isValid()) return QObject::tr("该记录已归还");
    if (loan.renewTimes >= AppConfig::instance().maxRenewTimes()) return QObject::tr("续借次数已达上限");
    if (QDateTime::currentDateTime() > loan.dueTime) return QObject::tr("已超期不可续借");

    loan.dueTime = loan.dueTime.addDays(AppConfig::instance().loanDaysDefault());
    loan.renewTimes += 1;
    if (!LoanRepository::instance().update(loan)) return QObject::tr("更新续借失败");
    return std::nullopt;
}


