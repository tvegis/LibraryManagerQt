#pragma once
#include <QMainWindow>

class QLineEdit;
class QTableWidget;

class ReaderMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ReaderMainWindow(int readerId, QWidget *parent = nullptr);

private slots:
    void onSearchBooks();
    void onLoadLoans();
    void onLoadHistory();
    void onRenewSelected();
    void onReserveByBookId();

private:
    int readerId_ = 0;
    QLineEdit *bookKeywordEdit_ = nullptr;
    QTableWidget *bookTable_ = nullptr;
    QTableWidget *loanTable_ = nullptr;
    QTableWidget *historyTable_ = nullptr;
};


