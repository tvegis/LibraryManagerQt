#pragma once
#include <QMainWindow>

class QLineEdit;
class QTableWidget;

class AdminMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit AdminMainWindow(QWidget *parent = nullptr);

private slots:
    void onSearchBooks();
    void onAddBook();
    void onEditBook();
    void onDeleteBook();

    void onSearchReaders();
    void onAddReader();
    void onEditReader();
    void onDeleteReader();

    void onBorrow();
    void onReturn();
    void onShowOverdues();

private:
    void loadBooksToTable(const QVector<class Book> &books);
    void loadReadersToTable(const QVector<class Reader> &readers);

    // UI elements (built in .cpp)
    QWidget *booksTab_ = nullptr;
    QWidget *readersTab_ = nullptr;
    QWidget *loansTab_ = nullptr;

    QLineEdit *bookKeywordEdit_ = nullptr;
    QTableWidget *bookTable_ = nullptr;

    QLineEdit *readerKeywordEdit_ = nullptr;
    QTableWidget *readerTable_ = nullptr;
};


