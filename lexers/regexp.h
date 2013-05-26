#ifndef REGEXP_H
#define REGEXP_H

#include <memory>
#include <functional>
#include <QString>
#include <QVector>
#include "./lexererror.h"
#include "./strbuffer.h"

using namespace std;

namespace Lex
{
    class RegExp
    {
    public:
        virtual shared_ptr<LexerError> match(StrBuffer &buffer)=0;
        virtual QString toString()=0;
    };

    QStringList cutUp(const QString&);
    QStringList mapToString(QVector<shared_ptr<RegExp> > &v);

    shared_ptr<RegExp> loop(shared_ptr<RegExp> re);
    shared_ptr<RegExp> loop1(shared_ptr<RegExp> re);
    shared_ptr<RegExp> optional(shared_ptr<RegExp> re);
    shared_ptr<RegExp> choice(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2);
    shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2);
    shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2, shared_ptr<RegExp> re3);
    shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2, shared_ptr<RegExp> re3,
                           shared_ptr<RegExp> re4);
    shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2, shared_ptr<RegExp> re3,
                           shared_ptr<RegExp> re4, shared_ptr<RegExp> re5);
    shared_ptr<RegExp> str(QString s);
    shared_ptr<RegExp> no(shared_ptr<RegExp> re);
    shared_ptr<RegExp> neither(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2);

    // anyBut must read a char, can't accept EOF, but
    // checkNo accepts EOF fine
    shared_ptr<RegExp> anyBut(shared_ptr<RegExp> re);

    shared_ptr<RegExp> check(shared_ptr<RegExp> re);
    shared_ptr<RegExp> checkNo(shared_ptr<RegExp> re);
    shared_ptr<RegExp> loopTillDifferent(shared_ptr<RegExp> re);

    shared_ptr<RegExp> charIs(function<bool(QChar)> f, QString description);
    shared_ptr<RegExp> charOf(QString s);
    shared_ptr<RegExp> anyChar();

    shared_ptr<LexerError> success();
    shared_ptr<LexerError> expected(int pos, QString expected, QString found);
    shared_ptr<ExpectedOneOf> expectedOneOf(int pos, QString found);
    shared_ptr<IllegalHere> illegalHere(int pos, QString illegal, QString found);

    class Str : public RegExp
    {
        QString str;
    public:
        Str(QString s): str(s) { }
        virtual QString toString() { return QString("'%1'").arg(str);}
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };

    class Char : public RegExp
    {
        std::function<bool(QChar)> test;
        QString description;
    public:
        Char(function<bool(QChar)> test, QString description) : test(test),description(description) {}
        virtual QString toString() { return description; }
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };


    // Not strictly needed but might be faster than using 'choice'
    class CharOf : public RegExp
    {
        QString str;
    public:
        CharOf(QString str) : str(str) {}
        virtual QString toString() { return cutUp(str).join(", ");}
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };

    class Seq : public RegExp
    {
        QVector<shared_ptr<RegExp> > res;
    public:
        Seq() { }
        void append(shared_ptr<RegExp> re) { res.append(re); }
        virtual QString toString() { return mapToString(res).join(" "); }
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };

    class Choice: public RegExp
    {
        QVector<shared_ptr<RegExp> > res;
    public:
        Choice() { }
        void append(shared_ptr<RegExp> re) { res.append(re); }
        virtual QString toString() { return mapToString(res).join("/ "); }
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };

    class Repetition: public RegExp
    {
        shared_ptr<RegExp> re;
    public:
        Repetition(shared_ptr<RegExp> re): re(re) { }
        virtual QString toString() { return QString("(%1)*").arg(re->toString()); }
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };

    class Complement: public RegExp
    {
        shared_ptr<RegExp> re;
    public:
        Complement(shared_ptr<RegExp> re): re(re) { }
        virtual QString toString() { return QString("not(%1)").arg(re->toString()); }
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };

    // Succeeds iff re succeeds, but either way doesn't move the input
    class Check : public RegExp
    {
        shared_ptr<RegExp> re;
    public:
        Check(shared_ptr<RegExp> re): re(re) { }
        virtual QString toString() { return QString("check(%1)").arg(re->toString()); }
        virtual shared_ptr<LexerError> match(StrBuffer &buffer);
    };
}
#endif // REGEXP_H
