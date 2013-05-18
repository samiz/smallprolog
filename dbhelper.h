#ifndef PROGRAMDATABASE_H
#define PROGRAMDATABASE_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include "data/program.h"

class DBHelper
{
    QSqlDatabase db;
    bool _isOpen;
    QMap<QString, QString> insertQueries;
public:
    DBHelper();
    bool open();
    void prepareQueries();
    void createTables(QMap<QString, shared_ptr<Prolog::Fact> > &schema);
    void close();
    bool isOpen() { return _isOpen; }

    void assert(shared_ptr<Term::Compound> row);
    bool find(const QString &query, QSqlQuery &result);
    QSqlQuery q(QSqlQuery &q, const QString &query);
    QSqlQuery q(QString query, QVariant);
    QSqlQuery q(QString query, QVariant arg1, QVariant arg2);
    QSqlQuery q(QString query, QVariant arg1, QVariant arg2, QVariant arg3);
private:
    void batchCheck(QSqlQuery &query, const QString &q);
    bool exec(QSqlQuery &q, const QString &query);
    bool exec(QString query);
    bool exec(QString query, QVariant);
    bool exec(QString query, QVariant, QVariant);
    bool exec(QString query, QVariant, QVariant, QVariant);
    bool exec(QString query, QVariant, QVariant, QVariant, QVariant);
    bool exec(QString query, QVariant, QVariant, QVariant, QVariant, QVariant);
    bool exec(QString query, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant);

};

#endif // PROGRAMDATABASE_H
