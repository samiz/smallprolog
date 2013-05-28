#include "dbhelper.h"
#include "wam/wam.h"

#include <QDebug>
#include <QtSql/QSqlError>
#include <QStringList>

namespace Wam
{
DBHelper::DBHelper()
{
    _isOpen = false;
    db = QSqlDatabase::addDatabase("QSQLITE", "pdb");
    db.setDatabaseName(":memory:");
}


bool DBHelper::open()
{
    if(!db.open())
    {
        return false;
    }
    QSqlQuery pragma(db);
    pragma.exec("PRAGMA foreign_keys = ON;");
    pragma.exec("PRAGMA synchronous = OFF");
    pragma.exec("PRAGMA journal_mode = OFF");


    prepareQueries();
    _isOpen = true;
    return true;
}

void DBHelper::prepareQueries()
{
}

void DBHelper::beginTransaction()
{
    if(!db.transaction())
    {
        qDebug() << db.lastError().text();
    }
}

void DBHelper::endTransaction()
{
    if(!db.commit())
    {
        qDebug() << db.lastError().text();
    }
}

void DBHelper::createTables(QMap<QString, shared_ptr<Prolog::Fact> > &schema)
{

    QSqlQuery q(db);
    QStringList tableCreationStrings;

    for(auto i=schema.begin(); i!=schema.end(); ++i)
    {
        QStringList tableSpec;
        QStringList colNames;
        QStringList questionMarks;
        shared_ptr<Prolog::Fact> &f = i.value();
        int i=0;
        for(auto j=f->argTypes.begin(); j!=f->argTypes.end();++j)
        {
            QString type = *j;
            QString sqlType;
            if(type=="int")
            {
                sqlType = "INTEGER NOT NULL";
            }
            else if(type=="symbol")
            {
                sqlType = "TEXT NOT NULL";
            }
            else if(type=="string")
            {
                sqlType = "TEXT NOT NULL";
            }
            colNames.append(QString("c%1").arg(i));
            questionMarks.append("?");
            tableSpec.append(QString("c%1 %2").arg(i).arg(sqlType));
            i++;
        }
        tableCreationStrings.append(
                    //QString("drop table %1;\ncreate table %2(%3);\n")
                     QString("create table %2(%3);\n")
                   // .arg(f->name)
                    .arg(f->name)
                    .arg(tableSpec.join(", "))
                    );

        QString insertQ = QString("INSERT INTO %1(%2) VALUES(%3);").arg(f->name)
                .arg(colNames.join(","))
                .arg(questionMarks.join(","));


        insertQueriesText[f->name] = insertQ;

    }

    for(int i=0; i<tableCreationStrings.count(); ++i)
    {
        QString tcs = tableCreationStrings[i].trimmed();
        if(tcs == "")
            continue;
        bool ret = q.exec(tcs);
        if(!ret)
        {
           qDebug() << q.lastError().databaseText() << "/" << q.lastError().driverText();
        }
    }
    for(auto i=insertQueriesText.begin(); i!=insertQueriesText.end();++i)
    {
        QSqlQuery iq(db);
        if(!iq.prepare(i.value()))
        {
            qDebug() << "Error in query:" << i.value() << ":" << iq.lastError().databaseText() << "/" << iq.lastError().driverText();
        }
        insertQueries[i.key()] = iq;
    }
}

void DBHelper::close()
{
    _isOpen = false;
    db.close();
}

void DBHelper::assert(shared_ptr<Term::Compound> row)
{
    const QString tableName = row->functor->toString();
    QSqlQuery &q = insertQueries[tableName];

    for(int i=0;i<row->args.count(); ++i)
    {
        shared_ptr<Term::Term> col = row->args[i];
        q.bindValue(i, termToQVariant(col));
    }
    exec(q, insertQueriesText[tableName]);
}

void DBHelper::insert(QString tableName, const QVector<QVariant> &args)
{
    QSqlQuery &q = insertQueries[tableName];

    for(int i=0;i<args.count(); ++i)
    {
        q.bindValue(i, args[i]);
    }
    exec(q, insertQueriesText[tableName]);
}

void DBHelper::delete_(shared_ptr<Term::Compound> a, Wam &wam)
{
    const QString &tableName = a->functor->toString();
    QVector<QVariant> args;
    QStringList wheres;
    for(int i=0; i<a->args.count(); ++i)
    {
        shared_ptr<Term::Term> arg = a->args[i];
        shared_ptr<Term::Term> groundArg;
        if(wam.ground(arg, groundArg))
        {
            wheres.append(QString("c%1=?").arg(i));
            args.append(termToQVariant(groundArg));
        }
    }
    QString whereClause=(args.count()!=0)?QString(" WHERE %1").arg(wheres.join(" AND ")):"";
    QString query = QString("DELETE FROM %1%2")
            .arg(tableName)
            .arg(whereClause);
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return;
    }
    for(int i=0;i<args.count(); ++i)
        q.bindValue(i, args[i]);
    exec(q, query);

}

bool DBHelper::find(const QString &query, QSqlQuery &result, const QVector<QVariant> &conditions)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
         qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    for(int i=0; i<conditions.count(); ++i)
        q.bindValue(i, conditions[i]);

    if(exec(q, query))
    {
#ifdef QT_DEBUG
        qDebug() << "Query " << query << " returned " << q.size() << "rows";
#endif
        result = q;
        return true;
    }
    return false;
}

void DBHelper::batchCheck(QSqlQuery &query, const QString &q)
{
    bool result = query.execBatch();
    if(!result)
    {
        qDebug() << "Error in query:" << q << ":" << query.lastError().databaseText() << "/" << query.lastError().driverText();
    }
    query.clear();
}

bool DBHelper::exec(QSqlQuery &q, const QString &query)
{
    bool result = q.exec();
    if(!result)
    {
        qDebug() << q.lastError().text();
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

}
