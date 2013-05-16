#ifndef PARSERESULT_H
#define PARSERESULT_H

#include "ast.h"
namespace Parser
{
struct ParseResult
{
    int LastPos;
    ParseResult(int LastPos) : LastPos(LastPos) { }
    virtual bool success() { return false; }
    virtual shared_ptr<Ast> value() { return shared_ptr<Ast>(); }

    virtual QString toString()=0;
    virtual QString toPartialString()=0;
};

struct Success : public ParseResult
{
    shared_ptr<Ast> Value;
    Success(shared_ptr<Ast> Value) : ParseResult(0), Value(Value) { }
    bool success() { return true;}
    QString toString() { return "";}
    QString toPartialString() { return "";}
};

struct ExpectedSingle : public ParseResult
{
    QString expected;
    QString found;
    ExpectedSingle(int lastPos, QString expected, QString found): ParseResult(lastPos),
        expected(expected),
        found(found)
    { }
    QString toString() { return QString("Expected: %1, found: %2").arg(expected).arg(found);}
    QString toPartialString() { return QString("%1").arg(expected);}
};

struct ExpectedOneOf: public ParseResult
{
    QStringList what;
    QString found;
    ExpectedOneOf(int lastPos, QString found):ParseResult(lastPos), found(found) { }
    bool success() { return false;}
    QString toString() { return QString("Expected one of:(%1), found").arg(what.join(", ")).arg(found);}
    QString toPartialString() { return QString("%1").arg(what.join(", "));}
};

struct IllegalHere: public ParseResult
{
    QString illegal, found;
    IllegalHere(int lastPos, QString illegal, QString found):ParseResult(lastPos), illegal(illegal), found(found) { }
    bool success() { return false;}
    QString toString() { return QString("Illegal: %1").arg(found);}
    QString toPartialString() { return QString("not: %1").arg(found);}
};

}
#endif // PARSERESULT_H
