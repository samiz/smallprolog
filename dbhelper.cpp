#include "dbhelper.h"

#include <QDebug>
#include <QtSql/QSqlError>
#include <QStringList>

DBHelper::DBHelper()
{
    _isOpen = false;
}


bool DBHelper::open()
{
    db = QSqlDatabase::addDatabase("QSQLITE", "pdb");
    db.setDatabaseName(":memory:");

    if(!db.open())
    {
        return false;
    }
    QSqlQuery pragma(db);
    pragma.exec("PRAGMA foreign_keys = ON;");

    prepareQueries();
    _isOpen = true;
    return true;
}

void DBHelper::prepareQueries()
{
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
        insertQueries[f->name] = QString("INSERT INTO %1(%2) VALUES(%3);")
                .arg(f->name)
               .arg(colNames.join(","))
               .arg(questionMarks.join(","));
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
}

void DBHelper::close()
{
    _isOpen = false;
    db.close();
}

void DBHelper::assert(shared_ptr<Term::Compound> row)
{
    QSqlQuery q(db);
    QString tableName = row->functor->toString();
    if(!q.prepare(insertQueries[tableName]))
    {
        qDebug() << "Error in query:" << insertQueries[tableName] << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return;
    }
    for(int i=0;i<row->args.count(); ++i)
    {
        shared_ptr<Term::Term> col = row->args[i];
        if(col->tag==Term::TermInt)
        {
            q.bindValue(i, dynamic_pointer_cast<Term::Int>(col)->value);
        }
        else if(col->tag==Term::TermSymbol)
        {
            q.addBindValue(col->toString());
        }
        else if(col->tag==Term::TermStr)
        {
            q.bindValue(i, dynamic_pointer_cast<Term::String>(col)->value);
        }
    }
    exec(q, insertQueries[tableName]);
}

bool DBHelper::find(const QString &query, QSqlQuery &result)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
         qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    if(exec(q, query))
    {
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

bool DBHelper::exec(QString query)
{
    QSqlQuery q(db);
    q.prepare(query);
    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

bool DBHelper::exec(QString query, QVariant arg1)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return false;
    }
    q.addBindValue(arg1);
    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

bool DBHelper::exec(QString query, QVariant arg1, QVariant arg2)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return false;
    }
    q.addBindValue(arg1);
    q.addBindValue(arg2);
    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

bool DBHelper::exec(QString query, QVariant arg1, QVariant arg2, QVariant arg3)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return false;
    }
    q.addBindValue(arg1);
    q.addBindValue(arg2);
    q.addBindValue(arg3);

    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

bool DBHelper::exec(QString query, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return false;
    }
    q.addBindValue(arg1);
    q.addBindValue(arg2);
    q.addBindValue(arg3);
    q.addBindValue(arg4);

    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

bool DBHelper::exec(QString query, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return false;
    }
    q.addBindValue(arg1);
    q.addBindValue(arg2);
    q.addBindValue(arg3);
    q.addBindValue(arg4);
    q.addBindValue(arg5);

    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

bool DBHelper::exec(QString query, QVariant arg1, QVariant arg2, QVariant arg3,
                           QVariant arg4, QVariant arg5, QVariant arg6)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return false;
    }
    q.addBindValue(arg1);
    q.addBindValue(arg2);
    q.addBindValue(arg3);
    q.addBindValue(arg4);
    q.addBindValue(arg5);
    q.addBindValue(arg6);

    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return result;
}

QSqlQuery DBHelper::q(QSqlQuery &q, const QString &query)
{
    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return q;
}

QSqlQuery DBHelper::q(QString query, QVariant arg1)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return q;
    }
    q.addBindValue(arg1);
    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return q;
}

QSqlQuery DBHelper::q(QString query, QVariant arg1, QVariant arg2)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return q;
    }
    q.addBindValue(arg1);
    q.addBindValue(arg2);
    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return q;
}

QSqlQuery DBHelper::q(QString query, QVariant arg1, QVariant arg2, QVariant arg3)
{
    QSqlQuery q(db);
    if(!q.prepare(query))
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
        return q;
    }
    q.addBindValue(arg1);
    q.addBindValue(arg2);
    q.addBindValue(arg3);
    bool result = q.exec();
    if(!result)
    {
        qDebug() << "Error in query:" << query << ":" << q.lastError().databaseText() << "/" << q.lastError().driverText();
    }
    return q;
}
