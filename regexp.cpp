#include "regexp.h"
#include "lexer.h"

namespace Lex
{

QString toStr(QChar c)
{
    return QString("%1").arg(c);
}

shared_ptr<LexerError> Str::match(StrBuffer &buffer)
{
    StrBuffer::State s;
    buffer.saveState(s);
    if(str == "")
        return success();
    if(buffer.eof())
    {
        return expected(buffer.GetPos(), str, "EOF");
    }
    if(buffer.readAhead(str))
    {
        buffer.readMany(str);
        return success();
    }
    buffer.restoreState(s);
    return expected(buffer.GetPos(), str, buffer.readAhead());
}

shared_ptr<LexerError> Char::match(StrBuffer &buffer)
{
    if(buffer.eof())
    {
        return expected(buffer.GetPos(), description, "EOF");
    }

    StrBuffer::State s;
    buffer.saveState(s);
    QChar c = buffer.read();
    if(test(c))
    {
        return success();
    }
    buffer.restoreState(s);
    return expected(buffer.GetPos(), description, buffer.readAhead());
}

shared_ptr<LexerError> CharOf::match(StrBuffer &buffer)
{
    if(buffer.eof())
    {
        auto error = expectedOneOf(buffer.GetPos(), "EOF");
        for(int i=0;i<str.length();++i)
            error->what.append(str.mid(i,1));
        return error;
    }
    StrBuffer::State s;
    buffer.saveState(s);
    QChar c = buffer.read();
    if(str.contains(c))
    {
        return success();
    }
    buffer.restoreState(s);
    auto error = expectedOneOf(buffer.GetPos(), buffer.readAhead());
    for(int i=0;i<str.length();++i)
        error->what.append(str.mid(i,1));
    return error;
}

shared_ptr<LexerError> Seq::match(StrBuffer &buffer)
{
    StrBuffer::State s;
    buffer.saveState(s);
    for(int i=0; i<res.count(); i++)
    {
        shared_ptr<RegExp> p = res[i];

        shared_ptr<LexerError> result = p->match(buffer);
        if(!result->success())
        {
            buffer.restoreState(s);
            return result;
        }
    }
    return success();
}

shared_ptr<LexerError> Choice::match(StrBuffer &buffer)
{
    QStringList errs;
    for(int i=0; i<res.count(); i++)
    {
        shared_ptr<RegExp> p = res[i];
        StrBuffer::State s;
        buffer.saveState(s);

        shared_ptr<LexerError> result = p->match(buffer);
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

shared_ptr<LexerError> Repetition::match(StrBuffer &buffer)
{
    while(true)
    {
        StrBuffer::State s;
        buffer.saveState(s);

        auto result = re->match(buffer);
        if(!result->success())
        {
            buffer.restoreState(s);
            break;
        }
    }
    return success();
}

shared_ptr<LexerError> Complement::match(StrBuffer &buffer)
{
    StrBuffer::State s;
    buffer.saveState(s);
    auto result = re->match(buffer);
    if(result->success())
    {
        buffer.restoreState(s);
        QString found = buffer.eof()? "EOF" : toStr(buffer.readAhead());
        return illegalHere(buffer.GetPos(), re->toString(), found);
    }
    return success();
}

shared_ptr<LexerError> Check::match(StrBuffer &buffer)
{
    StrBuffer::State s;
    buffer.saveState(s);
    auto result = re->match(buffer);
    buffer.restoreState(s);
    return result;
}

QStringList cutUp(const QString&str)
{
    QStringList ret;
    for(int i=0; i<str.length(); ++i)
        ret.append(str.mid(i, 1));
    return ret;
}

QStringList mapToString(QVector<shared_ptr<RegExp> > &v)
{
    QStringList ret;
    for(int i=0; i<v.count(); ++i)
        ret.append(v[i]->toString());
    return ret;
}

shared_ptr<RegExp> loop(shared_ptr<RegExp> re)
{
    return shared_ptr<RegExp>(new Repetition(re));
}

shared_ptr<RegExp> loop1(shared_ptr<RegExp> re)
{
    return seq(re, loop(re));
}


shared_ptr<RegExp> optional(shared_ptr<RegExp> re)
{
    return choice(re, str(""));
}

shared_ptr<RegExp> choice(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2)
{
    shared_ptr<Choice> ret(new Choice());
    ret->append(re1);
    ret->append(re2);
    return ret;
}

shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    return ret;
}

shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2, shared_ptr<RegExp> re3)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    ret->append(re3);
    return ret;
}

shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2, shared_ptr<RegExp> re3,
                       shared_ptr<RegExp> re4)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    ret->append(re3);
    ret->append(re4);
    return ret;
}

shared_ptr<RegExp> seq(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2, shared_ptr<RegExp> re3,
                       shared_ptr<RegExp> re4, shared_ptr<RegExp> re5)
{
    shared_ptr<Seq> ret(new Seq());
    ret->append(re1);
    ret->append(re2);
    ret->append(re3);
    ret->append(re4);
    ret->append(re5);
    return ret;
}

shared_ptr<RegExp> str(QString s)
{
    shared_ptr<Str> ret(new Str(s));
    return ret;
}

shared_ptr<RegExp> no(shared_ptr<RegExp> re)
{
    return shared_ptr<RegExp>(new Complement(re));
}

shared_ptr<RegExp> neither(shared_ptr<RegExp> re1, shared_ptr<RegExp> re2)
{
    return no(choice(re1, re2));
}

shared_ptr<RegExp> anyBut(shared_ptr<RegExp> re)
{
    return seq(no(re), anyChar());
}

shared_ptr<RegExp> check(shared_ptr<RegExp> re)
{
    return shared_ptr<RegExp>(new Check(re));
}

shared_ptr<RegExp> checkNo(shared_ptr<RegExp> re)
{
    return check(no(re));
}


shared_ptr<RegExp> charIs(function<bool(QChar)> f, QString description)
{
    shared_ptr<Char> ret(new Char(f, description));
    return ret;
}

shared_ptr<RegExp> charOf(QString s)
{
    return shared_ptr<RegExp>(new CharOf(s));
}


shared_ptr<RegExp> anyChar()
{
    shared_ptr<Char> ret(new Char([](QChar) {return true;}, "<any char>"));
    return ret;
}

shared_ptr<LexerError> success()
{
    return shared_ptr<LexerError>(new Success());
}

shared_ptr<LexerError> expected(int pos, QString expected, QString found)
{
    return shared_ptr<LexerError>(new ExpectedSingle(pos, expected, found));
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
