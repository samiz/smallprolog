#include "parsecombinators.h"

namespace Parser
{

QString toStr(shared_ptr<Token> c)
{
    return c->Lexeme;
}

shared_ptr<ParseResult> ByLexeme::match(TokenBuffer &buffer)
{
    TokenBuffer::State s;
    buffer.saveState(s);

    if(buffer.eof())
    {
        return expected(buffer.GetPos(), lexeme, "EOF");
    }
    shared_ptr<Token> t = buffer.readAhead();
    if(lexeme == t->Lexeme)
    {
        buffer.read();
        return success(shared_ptr<Ast>(new TokenAst(t)));
    }
    buffer.restoreState(s);
    return expected(buffer.GetPos(), lexeme, buffer.readAhead()->toString());
}


shared_ptr<ParseResult> Seq::match(TokenBuffer &buffer)
{
    TokenBuffer::State s;
    buffer.saveState(s);
    shared_ptr<SeqAst> results(new SeqAst());
    for(int i=0; i<res.count(); i++)
    {
        shared_ptr<Parser> p = res[i];

        shared_ptr<ParseResult> result = p->match(buffer);
        if(!result->success())
        {
            buffer.restoreState(s);
            return result;
        }
        if(labels.contains(i))
        {
            results->add(result->value(), labels[i]);
        }
        else
        {
            results->add(result->value());
        }
    }
    return success(results);
}

shared_ptr<ParseResult> Choice::match(TokenBuffer &buffer)
{
    QStringList errs;
    for(int i=0; i<res.count(); i++)
    {
        shared_ptr<Parser> p = res[i];
        TokenBuffer::State s;
        buffer.saveState(s);

        shared_ptr<ParseResult> result = p->match(buffer);
        if(result->success())
            return result;
        errs.append(result->toPartialString());
        buffer.restoreState(s);
    }
    QString found = buffer.eof()? "EOF" : toStr(buffer.readAhead());

    shared_ptr<ExpectedOneOf> err = expectedOneOf(buffer.GetPos(), found);
    err->what += errs;
    return err;
}

shared_ptr<ParseResult> Repetition::match(TokenBuffer &buffer)
{
    shared_ptr<VectorAst> results(new VectorAst());
    while(true)
    {
        TokenBuffer::State s;
        buffer.saveState(s);

        auto result = re->match(buffer);
        if(!result->success())
        {
            buffer.restoreState(s);
            break;
        }
        results->add(result->value());
    }
    return success(results);
}

shared_ptr<ParseResult> Complement::match(TokenBuffer &buffer)
{
    TokenBuffer::State s;
    buffer.saveState(s);
    auto result = re->match(buffer);
    if(result->success())
    {
        buffer.restoreState(s);
        QString found = buffer.eof()? "EOF" : toStr(buffer.readAhead());
        return illegalHere(buffer.GetPos(), re->toString(), found);
    }
    buffer.restoreState(s);
    return success(shared_ptr<Ast>());
}

shared_ptr<ParseResult> Check::match(TokenBuffer &buffer)
{
    TokenBuffer::State s;
    buffer.saveState(s);
    auto result = re->match(buffer);
    buffer.restoreState(s);
    return result;
}

shared_ptr<ParseResult> Eof::match(TokenBuffer &buffer)
{
    if(buffer.eof())
        return success(shared_ptr<Ast>());
    else
        return expected(buffer.GetPos(), "(eof)", buffer.readAhead()->toString());
}

QStringList cutUp(const QString&str)
{
    QStringList ret;
    for(int i=0; i<str.length(); ++i)
        ret.append(str.mid(i, 1));
    return ret;
}

QStringList mapToString(QVector<shared_ptr<Parser> > &v)
{
    QStringList ret;
    for(int i=0; i<v.count(); ++i)
        ret.append(v[i]->toString());
    return ret;
}

shared_ptr<Parser> loop(shared_ptr<Parser> re)
{
    return shared_ptr<Parser>(new Repetition(re));
}

shared_ptr<Parser> loop1(shared_ptr<Parser> re)
{
    return seq(re, loop(re));
}

shared_ptr<Parser> optional(shared_ptr<Parser> re)
{
    return choice(re, str(""));
}

shared_ptr<Parser> choice(shared_ptr<Parser> re1, shared_ptr<Parser> re2)
{
    shared_ptr<Choice> ret(new Choice());
    ret->append(re1);
    ret->append(re2);
    return ret;
}

shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    return ret;
}

shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2, shared_ptr<Parser> re3)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    ret->append(re3);
    return ret;
}

shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2, shared_ptr<Parser> re3,
                       shared_ptr<Parser> re4)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    ret->append(re3);
    ret->append(re4);
    return ret;
}

shared_ptr<Parser> seq(shared_ptr<Parser> re1, shared_ptr<Parser> re2, shared_ptr<Parser> re3,
                       shared_ptr<Parser> re4, shared_ptr<Parser> re5)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    ret->append(re3);
    ret->append(re4);
    ret->append(re5);
    return ret;
}

shared_ptr<Parser> str(QString s)
{
    shared_ptr<ByLexeme> ret(new ByLexeme(s));
    return ret;
}

shared_ptr<Parser> no(shared_ptr<Parser> re)
{
    return shared_ptr<Parser>(new Complement(re));
}

shared_ptr<Parser> neither(shared_ptr<Parser> re1, shared_ptr<Parser> re2)
{
    return no(choice(re1, re2));
}

shared_ptr<Parser> anyToken()
{
    return no(eof());
}

shared_ptr<Parser> eof()
{
    return shared_ptr<Parser>(new Eof());
}

shared_ptr<Parser> anyBut(shared_ptr<Parser> re)
{
    return seq(re, anyToken());
}

shared_ptr<Parser> check(shared_ptr<Parser> re)
{
    return shared_ptr<Parser>(new Check(re));
}

shared_ptr<Parser> checkNo(shared_ptr<Parser> re)
{
    return check(no(re));
}

shared_ptr<ParseResult> success(shared_ptr<Ast> result)
{
    return shared_ptr<ParseResult>(new Success(result));
}

shared_ptr<ParseResult> expected(int pos, QString expect, QString found)
{
    return shared_ptr<ExpectedSingle>(new ExpectedSingle(pos, expect, found));
}

shared_ptr<ExpectedOneOf> expectedOneOf(int pos, QString found)
{
    return shared_ptr<ExpectedOneOf>(new ExpectedOneOf(pos, found));
}

shared_ptr<IllegalHere> illegalHere(int pos, QString illegal, QString found)
{
    return shared_ptr<IllegalHere>(new IllegalHere(pos, illegal, found));
}

}
