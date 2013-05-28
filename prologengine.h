#ifndef PROLOGENGINE_H
#define PROLOGENGINE_H

#include "./lexers/prologlexer.h"
#include "./lexers/sexpressionlexer.h"
#include "./compiler/prologcompiler.h"

#include "./wam/wam.h"
#include <QVariant>
#include <functional>

class PrologConsole;

class PrologEngine
{
    Prolog::PrologLexer lexer;
    SExpressionLexer SExp;
    Wam::Wam wam;
    PrologConsole *console;
public:
    QStringList errors;
public:
    PrologEngine();

    void showConsole();
    void prepareDb();
    bool load(QString code);

    void beginTransaction() { wam.beginTransaction();}
    void endTransaction() { wam.endTransaction(); }
    void insert(QString tableName, const QVector<QVariant> &args) { wam.insert(tableName, args); }

    void call(QString proc,
              QVector<QVariant> const &args,
              std::function<void(QMap<QString,QVariant>)> callBack);

    void call(QString proc,
              std::function<void(QMap<QString,QVariant>)> callBack);

    void call(QString proc,
                            QVariant arg0,
                            std::function<void (QMap<QString, QVariant>)> callBack);
    void call(QString proc,
                            QVariant arg0,
                            QVariant arg1,
                            std::function<void (QMap<QString, QVariant>)> callBack);
    void call(QString proc,
                            QVariant arg0,
                            QVariant arg1,
                            QVariant arg2,
                            std::function<void (QMap<QString, QVariant>)> callBack);
    void call(QString proc,
                            QVariant arg0,
                            QVariant arg1,
                            QVariant arg2,
                            QVariant arg3,
                            std::function<void (QMap<QString, QVariant>)> callBack);


    void call(QString proc);
    void call(QString proc, QVariant arg0);
    void call(QString proc, QVariant arg0, QVariant arg1);
    void call(QString proc, QVariant arg0, QVariant arg1, QVariant arg2);
    void call(QString proc, QVariant arg0, QVariant arg1, QVariant arg2, QVariant arg3);
    void call(QString proc, QVector<QVariant> const &args);

private:
    void initProgram(Prolog::Program &proggy);
};

#endif // PROLOGENGINE_H
