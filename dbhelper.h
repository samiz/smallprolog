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
    QMap<QString, QSqlQuery> insertQueries;
    QMap<QString, QString> insertQueriesText;
public:
    DBHelper();
    bool open();
    void prepareQueries();
    void createTables(QMap<QString, shared_ptr<Prolog::Fact> > &schema);
    void close();
    bool isOpen() { return _isOpen; }

    void assert(shared_ptr<Term::Compound> row);
    void insert(QString tableName, const QVector<QVariant> &args);
    void delete_(shared_ptr<Term::Compound> a, Wam &wam);
    bool find(const QString &query, QSqlQuery &result, const QVector<QVariant> &conditions);

    void beginTransaction();
    void endTransaction();

    inline QVariant termToQVariant(const shared_ptr<Term::Term> &t) { return t->toVariant(); }
private:
    void batchCheck(QSqlQuery &query, const QString &q);
    inline bool exec(QSqlQuery &q, const QString &query);

};
}
#endif // PROGRAMDATABASE_H
