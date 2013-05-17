#include "ast.h"


shared_ptr<Token> TokenAst::first()
{
    return Value;
}

shared_ptr<Token> TokenAst::last()
{
    return Value;
}

shared_ptr<Token> SeqAst::first()
{
    return subs[0]->first();
}

shared_ptr<Token> SeqAst::last()
{
    return subs[subs.count()-1]->last();
}

QString SeqAst::toString()
{
    QStringList lst;
    for(int i=0; i<subs.count(); ++i)
    {
        if(indexlabels.contains(i))
            lst.append(QString("%1=%2").arg(indexlabels[i]).arg(subs[i]->toString()));
        else
            lst.append(subs[i]->toString());
    }
    return QString("%1{%2}").arg(type).arg(lst.join(", "));
}

shared_ptr<Token> VectorAst::first()
{
    return subs[0]->first();
}

shared_ptr<Token> VectorAst::last()
{
    return subs[subs.count()-1]->last();
}

QString VectorAst::toString()
{
    QStringList lst;
    for(int i=0; i<subs.count(); ++i)
    {
        lst.append(subs[i]->toString());
    }
    return QString("%1{%2}").arg(type).arg(lst.join(", "));
}
