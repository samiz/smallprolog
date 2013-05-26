#ifndef DBHELPER_H
#define DBHELPER_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include "./data/program.h"

namespace Wam
{
class Wam;
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
    void delete_(shared_ptr<Term::Compound> a, Wam &wam);
    bool find(const QString &query, QSqlQuery &result, const QVector<QVariant> &conditions);
    QSqlQuery q(QSqlQuery &q, const QString &query);
    QSqlQuery q(QString query, QVariant);
    QSqlQuery q(QString query, QVariant arg1, QVariant arg2);
    QSqlQuery q(QString query, QVariant arg1, QVariant arg2, QVariant arg3);

    QVariant termToQVariant(const shared_ptr<Term::Term> &t);
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
}
#endif // PROGRAMDATABASE_H
