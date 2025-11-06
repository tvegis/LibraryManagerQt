#include "AdminMainWindow.h"
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
#include "../repos/ReaderRepository.h"
#include "../repos/LoanRepository.h"
#include "../services/LibraryService.h"

AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle(tr("管理员端"));
    auto *tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    // 图书tab
    booksTab_ = new QWidget(this);
    {
        auto *layout = new QVBoxLayout(booksTab_);
        auto *hl = new QHBoxLayout();
        bookKeywordEdit_ = new QLineEdit(booksTab_);
        auto *btnSearch = new QPushButton(tr("查询"), booksTab_);
        auto *btnAdd = new QPushButton(tr("入库"), booksTab_);
        auto *btnEdit = new QPushButton(tr("修改"), booksTab_);
        auto *btnDel = new QPushButton(tr("下架"), booksTab_);
        hl->addWidget(bookKeywordEdit_);
        hl->addWidget(btnSearch);
        hl->addWidget(btnAdd);
        hl->addWidget(btnEdit);
        hl->addWidget(btnDel);
        layout->addLayout(hl);
        bookTable_ = new QTableWidget(0, 6, booksTab_);
        bookTable_->setHorizontalHeaderLabels({tr("ID"), tr("ISBN"), tr("书名"), tr("作者"), tr("库存"), tr("状态")});
        layout->addWidget(bookTable_);
        connect(btnSearch, &QPushButton::clicked, this, &AdminMainWindow::onSearchBooks);
        connect(btnAdd, &QPushButton::clicked, this, &AdminMainWindow::onAddBook);
        connect(btnEdit, &QPushButton::clicked, this, &AdminMainWindow::onEditBook);
        connect(btnDel, &QPushButton::clicked, this, &AdminMainWindow::onDeleteBook);
    }

    // 读者tab
    readersTab_ = new QWidget(this);
    {
        auto *layout = new QVBoxLayout(readersTab_);
        auto *hl = new QHBoxLayout();
        readerKeywordEdit_ = new QLineEdit(readersTab_);
        auto *btnSearch = new QPushButton(tr("查询"), readersTab_);
        auto *btnAdd = new QPushButton(tr("注册"), readersTab_);
        auto *btnEdit = new QPushButton(tr("修改"), readersTab_);
        auto *btnDel = new QPushButton(tr("注销"), readersTab_);
        hl->addWidget(readerKeywordEdit_);
        hl->addWidget(btnSearch);
        hl->addWidget(btnAdd);
        hl->addWidget(btnEdit);
        hl->addWidget(btnDel);
        layout->addLayout(hl);
        readerTable_ = new QTableWidget(0, 6, readersTab_);
        readerTable_->setHorizontalHeaderLabels({tr("ID"), tr("账号"), tr("姓名"), tr("性别"), tr("电话"), tr("状态")});
        layout->addWidget(readerTable_);
        connect(btnSearch, &QPushButton::clicked, this, &AdminMainWindow::onSearchReaders);
        connect(btnAdd, &QPushButton::clicked, this, &AdminMainWindow::onAddReader);
        connect(btnEdit, &QPushButton::clicked, this, &AdminMainWindow::onEditReader);
        connect(btnDel, &QPushButton::clicked, this, &AdminMainWindow::onDeleteReader);
    }

    // 借还tab
    loansTab_ = new QWidget(this);
    {
        auto *layout = new QVBoxLayout(loansTab_);
        auto *hl = new QHBoxLayout();
        auto *btnBorrow = new QPushButton(tr("借书"), loansTab_);
        auto *btnReturn = new QPushButton(tr("还书"), loansTab_);
        auto *btnOverdue = new QPushButton(tr("超期查询"), loansTab_);
        hl->addWidget(btnBorrow);
        hl->addWidget(btnReturn);
        hl->addWidget(btnOverdue);
        layout->addLayout(hl);
        connect(btnBorrow, &QPushButton::clicked, this, &AdminMainWindow::onBorrow);
        connect(btnReturn, &QPushButton::clicked, this, &AdminMainWindow::onReturn);
        connect(btnOverdue, &QPushButton::clicked, this, &AdminMainWindow::onShowOverdues);
    }

    tabs->addTab(booksTab_, tr("图书管理"));
    tabs->addTab(readersTab_, tr("读者管理"));
    tabs->addTab(loansTab_, tr("借还管理"));

    onSearchBooks();
    onSearchReaders();
}

void AdminMainWindow::loadBooksToTable(const QVector<Book> &books) {
    bookTable_->setRowCount(0);
    for (int i = 0; i < books.size(); ++i) {
        const auto &b = books[i];
        bookTable_->insertRow(i);
        bookTable_->setItem(i, 0, new QTableWidgetItem(QString::number(b.id)));
        bookTable_->setItem(i, 1, new QTableWidgetItem(b.isbn));
        bookTable_->setItem(i, 2, new QTableWidgetItem(b.title));
        bookTable_->setItem(i, 3, new QTableWidgetItem(b.author));
        bookTable_->setItem(i, 4, new QTableWidgetItem(QString("%1/%2").arg(b.availableCount).arg(b.totalCount)));
        bookTable_->setItem(i, 5, new QTableWidgetItem(b.status == 1 ? tr("上架") : tr("下架")));
    }
}

void AdminMainWindow::loadReadersToTable(const QVector<Reader> &readers) {
    readerTable_->setRowCount(0);
    for (int i = 0; i < readers.size(); ++i) {
        const auto &r = readers[i];
        readerTable_->insertRow(i);
        readerTable_->setItem(i, 0, new QTableWidgetItem(QString::number(r.id)));
        readerTable_->setItem(i, 1, new QTableWidgetItem(r.username));
        readerTable_->setItem(i, 2, new QTableWidgetItem(r.name));
        readerTable_->setItem(i, 3, new QTableWidgetItem(r.gender));
        readerTable_->setItem(i, 4, new QTableWidgetItem(r.phone));
        readerTable_->setItem(i, 5, new QTableWidgetItem(r.status == 1 ? tr("正常") : tr("注销")));
    }
}

void AdminMainWindow::onSearchBooks() {
    auto list = BookRepository::instance().search(bookKeywordEdit_->text().trimmed(), "", "", "");
    loadBooksToTable(list);
}

void AdminMainWindow::onAddBook() {
    bool ok = false;
    QString isbn = QInputDialog::getText(this, tr("入库"), tr("ISBN:"), QLineEdit::Normal, {}, &ok);
    if (!ok || isbn.isEmpty()) return;
    QString title = QInputDialog::getText(this, tr("入库"), tr("书名:"), QLineEdit::Normal, {}, &ok);
    if (!ok || title.isEmpty()) return;
    int count = QInputDialog::getInt(this, tr("入库"), tr("数量:"), 1, 0, 100000, 1, &ok);
    if (!ok) return;
    Book b; b.isbn = isbn; b.title = title; b.totalCount = count; b.availableCount = count; b.status = 1;
    if (!BookRepository::instance().create(b)) { QMessageBox::warning(this, tr("失败"), tr("入库失败")); return; }
    onSearchBooks();
}

void AdminMainWindow::onEditBook() {
    auto row = bookTable_->currentRow(); if (row < 0) return;
    int id = bookTable_->item(row,0)->text().toInt();
    auto bOpt = BookRepository::instance().getById(id); if (!bOpt) return;
    Book b = *bOpt;
    bool ok = false;
    QString title = QInputDialog::getText(this, tr("修改"), tr("书名:"), QLineEdit::Normal, b.title, &ok);
    if (!ok) return;
    int total = QInputDialog::getInt(this, tr("修改"), tr("总量:"), b.totalCount, 0, 100000, 1, &ok);
    if (!ok) return;
    int avail = QInputDialog::getInt(this, tr("修改"), tr("可借:"), b.availableCount, 0, total, 1, &ok);
    if (!ok) return;
    b.title = title; b.totalCount = total; b.availableCount = avail;
    if (!BookRepository::instance().update(b)) { QMessageBox::warning(this, tr("失败"), tr("保存失败")); return; }
    onSearchBooks();
}

void AdminMainWindow::onDeleteBook() {
    auto row = bookTable_->currentRow(); if (row < 0) return;
    int id = bookTable_->item(row,0)->text().toInt();
    auto bOpt = BookRepository::instance().getById(id); if (!bOpt) return;
    Book b = *bOpt; b.status = 0; // 下架
    if (!BookRepository::instance().update(b)) { QMessageBox::warning(this, tr("失败"), tr("下架失败")); return; }
    onSearchBooks();
}

void AdminMainWindow::onSearchReaders() {
    auto list = ReaderRepository::instance().search(readerKeywordEdit_->text().trimmed());
    loadReadersToTable(list);
}

void AdminMainWindow::onAddReader() {
    bool ok=false; QString username = QInputDialog::getText(this, tr("注册"), tr("账号:"), QLineEdit::Normal, {}, &ok); if (!ok||username.isEmpty()) return;
    QString password = QInputDialog::getText(this, tr("注册"), tr("初始密码:"), QLineEdit::Normal, {}, &ok); if (!ok||password.isEmpty()) return;
    QString name = QInputDialog::getText(this, tr("注册"), tr("姓名:"), QLineEdit::Normal, {}, &ok); if (!ok) return;
    Reader r; r.username=username; r.passwordHash=AuthService::hashPassword(password); r.name=name; r.status=1;
    if (!ReaderRepository::instance().create(r)) { QMessageBox::warning(this, tr("失败"), tr("注册失败")); return; }
    onSearchReaders();
}

void AdminMainWindow::onEditReader() {
    auto row = readerTable_->currentRow(); if (row < 0) return;
    int id = readerTable_->item(row,0)->text().toInt();
    auto rOpt = ReaderRepository::instance().getById(id); if (!rOpt) return;
    Reader r = *rOpt; bool ok=false;
    QString phone = QInputDialog::getText(this, tr("修改"), tr("手机:"), QLineEdit::Normal, r.phone, &ok); if (!ok) return;
    r.phone = phone;
    if (!ReaderRepository::instance().update(r)) { QMessageBox::warning(this, tr("失败"), tr("保存失败")); return; }
    onSearchReaders();
}

void AdminMainWindow::onDeleteReader() {
    auto row = readerTable_->currentRow(); if (row < 0) return;
    int id = readerTable_->item(row,0)->text().toInt();
    auto rOpt = ReaderRepository::instance().getById(id); if (!rOpt) return;
    Reader r = *rOpt; r.status = 0;
    if (!ReaderRepository::instance().update(r)) { QMessageBox::warning(this, tr("失败"), tr("注销失败")); return; }
    onSearchReaders();
}

void AdminMainWindow::onBorrow() {
    bool ok=false; int readerId = QInputDialog::getInt(this, tr("借书"), tr("读者ID:"), 0, 1, 1000000, 1, &ok); if (!ok) return;
    int bookId = QInputDialog::getInt(this, tr("借书"), tr("图书ID:"), 0, 1, 1000000, 1, &ok); if (!ok) return;
    auto err = LibraryService::instance().borrowBook(readerId, bookId);
    if (err) QMessageBox::warning(this, tr("失败"), *err); else QMessageBox::information(this, tr("成功"), tr("借阅成功"));
    onSearchBooks();
}

void AdminMainWindow::onReturn() {
    bool ok=false; int loanId = QInputDialog::getInt(this, tr("还书"), tr("借阅记录ID:"), 0, 1, 1000000, 1, &ok); if (!ok) return;
    auto err = LibraryService::instance().returnBook(loanId);
    if (err) QMessageBox::warning(this, tr("失败"), *err); else QMessageBox::information(this, tr("成功"), tr("归还成功"));
}

void AdminMainWindow::onShowOverdues() {
    auto list = LoanRepository::instance().getOverdue();
    QMessageBox::information(this, tr("超期"), tr("当前超期未还 %1 条").arg(list.size()));
}


