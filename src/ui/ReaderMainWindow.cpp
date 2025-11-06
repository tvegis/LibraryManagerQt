#include "ReaderMainWindow.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include "../repos/BookRepository.h"
#include "../repos/LoanRepository.h"
#include "../repos/ReservationRepository.h"
#include "../services/LibraryService.h"

ReaderMainWindow::ReaderMainWindow(int readerId, QWidget *parent)
    : QMainWindow(parent), readerId_(readerId) {
    setWindowTitle(tr("读者端"));
    auto *tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    // 图书检索
    QWidget *booksTab = new QWidget(this);
    {
        auto *layout = new QVBoxLayout(booksTab);
        auto *hl = new QHBoxLayout();
        bookKeywordEdit_ = new QLineEdit(booksTab);
        auto *btnSearch = new QPushButton(tr("检索"), booksTab);
        auto *btnReserve = new QPushButton(tr("预约(按图书ID)"), booksTab);
        hl->addWidget(bookKeywordEdit_);
        hl->addWidget(btnSearch);
        hl->addWidget(btnReserve);
        layout->addLayout(hl);
        bookTable_ = new QTableWidget(0, 6, booksTab);
        bookTable_->setHorizontalHeaderLabels({tr("ID"), tr("ISBN"), tr("书名"), tr("作者"), tr("可借"), tr("状态")});
        layout->addWidget(bookTable_);
        connect(btnSearch, &QPushButton::clicked, this, &ReaderMainWindow::onSearchBooks);
        connect(btnReserve, &QPushButton::clicked, this, &ReaderMainWindow::onReserveByBookId);
    }

    // 当前借阅
    QWidget *loansTab = new QWidget(this);
    {
        auto *layout = new QVBoxLayout(loansTab);
        auto *hl = new QHBoxLayout();
        auto *btnReload = new QPushButton(tr("刷新"), loansTab);
        auto *btnRenew = new QPushButton(tr("续借(选中)"), loansTab);
        hl->addWidget(btnReload);
        hl->addWidget(btnRenew);
        layout->addLayout(hl);
        loanTable_ = new QTableWidget(0, 5, loansTab);
        loanTable_->setHorizontalHeaderLabels({tr("记录ID"), tr("图书ID"), tr("借出"), tr("应还"), tr("续借次数")});
        layout->addWidget(loanTable_);
        connect(btnReload, &QPushButton::clicked, this, &ReaderMainWindow::onLoadLoans);
        connect(btnRenew, &QPushButton::clicked, this, &ReaderMainWindow::onRenewSelected);
    }

    // 历史借阅
    QWidget *historyTab = new QWidget(this);
    {
        auto *layout = new QVBoxLayout(historyTab);
        auto *btnReload = new QPushButton(tr("刷新"), historyTab);
        layout->addWidget(btnReload);
        historyTable_ = new QTableWidget(0, 6, historyTab);
        historyTable_->setHorizontalHeaderLabels({tr("记录ID"), tr("图书ID"), tr("借出"), tr("应还"), tr("归还"), tr("罚金")});
        layout->addWidget(historyTable_);
        connect(btnReload, &QPushButton::clicked, this, &ReaderMainWindow::onLoadHistory);
    }

    tabs->addTab(booksTab, tr("图书检索与预约"));
    tabs->addTab(loansTab, tr("当前借阅"));
    tabs->addTab(historyTab, tr("历史借阅"));

    onSearchBooks();
    onLoadLoans();
    onLoadHistory();
}

void ReaderMainWindow::onSearchBooks() {
    auto list = BookRepository::instance().search(bookKeywordEdit_->text().trimmed(), "", "", "");
    bookTable_->setRowCount(0);
    for (int i=0;i<list.size();++i) {
        const auto &b = list[i];
        bookTable_->insertRow(i);
        bookTable_->setItem(i,0,new QTableWidgetItem(QString::number(b.id)));
        bookTable_->setItem(i,1,new QTableWidgetItem(b.isbn));
        bookTable_->setItem(i,2,new QTableWidgetItem(b.title));
        bookTable_->setItem(i,3,new QTableWidgetItem(b.author));
        bookTable_->setItem(i,4,new QTableWidgetItem(QString::number(b.availableCount)));
        bookTable_->setItem(i,5,new QTableWidgetItem(b.status==1?tr("上架"):tr("下架")));
    }
}

void ReaderMainWindow::onLoadLoans() {
    auto list = LoanRepository::instance().getActiveByReader(readerId_);
    loanTable_->setRowCount(0);
    for (int i=0;i<list.size();++i) {
        const auto &l = list[i];
        loanTable_->insertRow(i);
        loanTable_->setItem(i,0,new QTableWidgetItem(QString::number(l.id)));
        loanTable_->setItem(i,1,new QTableWidgetItem(QString::number(l.bookId)));
        loanTable_->setItem(i,2,new QTableWidgetItem(l.borrowTime.toString("yyyy-MM-dd")));
        loanTable_->setItem(i,3,new QTableWidgetItem(l.dueTime.toString("yyyy-MM-dd")));
        loanTable_->setItem(i,4,new QTableWidgetItem(QString::number(l.renewTimes)));
    }
}

void ReaderMainWindow::onLoadHistory() {
    auto list = LoanRepository::instance().getHistoryByReader(readerId_);
    historyTable_->setRowCount(0);
    for (int i=0;i<list.size();++i) {
        const auto &l = list[i];
        historyTable_->insertRow(i);
        historyTable_->setItem(i,0,new QTableWidgetItem(QString::number(l.id)));
        historyTable_->setItem(i,1,new QTableWidgetItem(QString::number(l.bookId)));
        historyTable_->setItem(i,2,new QTableWidgetItem(l.borrowTime.toString("yyyy-MM-dd")));
        historyTable_->setItem(i,3,new QTableWidgetItem(l.dueTime.toString("yyyy-MM-dd")));
        historyTable_->setItem(i,4,new QTableWidgetItem(l.returnTime.toString("yyyy-MM-dd")));
        historyTable_->setItem(i,5,new QTableWidgetItem(QString::number(l.fine, 'f', 2)));
    }
}

void ReaderMainWindow::onRenewSelected() {
    auto row = loanTable_->currentRow(); if (row < 0) return;
    int loanId = loanTable_->item(row,0)->text().toInt();
    auto err = LibraryService::instance().renewLoan(loanId);
    if (err) QMessageBox::warning(this, tr("失败"), *err); else QMessageBox::information(this, tr("成功"), tr("续借成功"));
    onLoadLoans();
}

void ReaderMainWindow::onReserveByBookId() {
    bool ok=false; int bookId = QInputDialog::getInt(this, tr("预约"), tr("图书ID:"), 0, 1, 1000000, 1, &ok); if (!ok) return;
    Reservation r; r.bookId = bookId; r.readerId = readerId_; r.status = 0;
    if (!ReservationRepository::instance().create(r)) QMessageBox::warning(this, tr("失败"), tr("预约失败"));
    else QMessageBox::information(this, tr("成功"), tr("预约成功，归还后将通知"));
}


