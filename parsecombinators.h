#ifndef PARSECOMBINATORS_H
#define PARSECOMBINATORS_H


#include <memory>
#include <functional>
#include <QString>
#include <QVector>
#include "parser.h"
#include "buffer.h"
#include "parseresult.h"

using namespace std;

namespace Parser
{
    class Parser
    {
    public:
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer)=0;
        virtual QString toString()=0;
    };

    QStringList cutUp(const QString&);
    QStringList mapToString(QVector<shared_ptr<Parser> > &v);

    shared_ptr<Parser> loop(shared_ptr<Parser> re);
    shared_ptr<Parser> loop1(shared_ptr<Parser> re);
    shared_ptr<Parser> optional(shared_ptr<Parser> re);
    shared_ptr<Parser> choice(shared_ptr<Parser> re1, shared_ptr<Parser> re2);
    shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2);
    shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2, shared_ptr<Parser> re3);
    shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2, shared_ptr<Parser> re3,
                           shared_ptr<Parser> re4);
    shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2, shared_ptr<Parser> re3,
                           shared_ptr<Parser> re4, shared_ptr<Parser> re5);
    shared_ptr<Parser> str(QString s);
    shared_ptr<Parser> no(shared_ptr<Parser> re);
    shared_ptr<Parser> neither(shared_ptr<Parser> re1, shared_ptr<Parser> re2);

    // anyBut must read a char, can't accept EOF, but
    // checkNo accepts EOF fine
    shared_ptr<Parser> anyBut(shared_ptr<Parser> re);

    shared_ptr<Parser> check(shared_ptr<Parser> re);
    shared_ptr<Parser> checkNo(shared_ptr<Parser> re);
    shared_ptr<Parser> loopTillDifferent(shared_ptr<Parser> re);

    shared_ptr<Parser> tokenIs(function<bool(shared_ptr<Token>)> f, QString description);
    shared_ptr<Parser> charOf(QString s);
    shared_ptr<Parser> anyToken();
    shared_ptr<Parser> eof();

    shared_ptr<ParseResult> success(shared_ptr<Ast> result);
    shared_ptr<ParseResult> expected(int pos, QString expect, QString found);
    shared_ptr<ExpectedOneOf> expectedOneOf(int pos, QString found);
    shared_ptr<IllegalHere> illegalHere(int pos, QString illegal, QString found);

    class ByLexeme : public Parser
    {
        QString lexeme;
    public:
        ByLexeme(QString s): lexeme(s) { }
        virtual QString toString() { return QString("'%1'").arg(lexeme);}
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer);
    };

    class Seq : public Parser
    {
        QVector<shared_ptr<Parser> > res;
        QMap<int, QString> labels;
    public:
        Seq() { }
        void append(shared_ptr<Parser> re, QString label="_")
        {
            res.append(re);
            if(label !="_")
            {
                labels[res.count()-1] = label;
            }
        }
        virtual QString toString() { return mapToString(res).join(" "); }
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer);
    };

    class Choice: public Parser
    {
        QVector<shared_ptr<Parser> > res;
    public:
        Choice() { }
        void append(shared_ptr<Parser> re) { res.append(re); }
        virtual QString toString() { return mapToString(res).join("/ "); }
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer);
    };

    class Repetition: public Parser
    {
        shared_ptr<Parser> re;
    public:
        Repetition(shared_ptr<Parser> re): re(re) { }
        virtual QString toString() { return QString("(%1)*").arg(re->toString()); }
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer);
    };

    class Complement: public Parser
    {
        shared_ptr<Parser> re;
    public:
        Complement(shared_ptr<Parser> re): re(re) { }
        virtual QString toString() { return QString("not(%1)").arg(re->toString()); }
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer);
    };

    // Succeeds iff re succeeds, but either way doesn't move the input
    class Check : public Parser
    {
        shared_ptr<Parser> re;
    public:
        Check(shared_ptr<Parser> re): re(re) { }
        virtual QString toString() { return QString("check(%1)").arg(re->toString()); }
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer);
    };

    class Eof: public Parser
    {
    public:
        Eof() { }
        virtual QString toString() { return "(eof)"; }
        virtual shared_ptr<ParseResult> match(TokenBuffer &buffer);
    };
}
#endif // PARSECOMBINATORS_H
