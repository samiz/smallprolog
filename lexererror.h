#ifndef LEXERERROR_H
#define LEXERERROR_H

#include <QString>
#include <QStringList>
#include <QVector>

namespace Lex
{
struct LexerError
{
    int lastPos;
    LexerError(int lastPos) : lastPos(lastPos) {}

    virtual bool success()=0;
    virtual QString toString()=0;
    virtual QString toPartialString()=0;
};

struct Success : public LexerError
{
    Success() : LexerError(0) { }
    bool success() { return true;}
    QString toString() { return "";}
    QString toPartialString() { return "";}
};

struct ExpectedSingle : public LexerError
{
    QString expected;
    QString found;
    ExpectedSingle(int lastPos, QString expected, QString found): LexerError(lastPos),
        expected(expected),
        found(found)
    { }
    bool success() { return false;}
    QString toString() { return QString("Expected: %1, found: %2").arg(expected).arg(found);}
    QString toPartialString() { return QString("%1").arg(expected);}
};

struct ExpectedOneOf: public LexerError
{
    QStringList what;
    QString found;
    ExpectedOneOf(int lastPos, QString found):LexerError(lastPos), found(found) { }
    bool success() { return false;}
    QString toString() { return QString("Expected one of:(%1), found").arg(what.join(", ")).arg(found);}
    QString toPartialString() { return QString("%1").arg(what.join(", "));}
};

struct IllegalHere: public LexerError
{
    QString illegal, found;
    IllegalHere(int lastPos, QString illegal, QString found):LexerError(lastPos), illegal(illegal), found(found) { }
    bool success() { return false;}
    QString toString() { return QString("Illegal: %1").arg(found);}
    QString toPartialString() { return QString("not: %1").arg(found);}
};

}
#endif // LEXERERROR_H
