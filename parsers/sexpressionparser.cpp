#include "sexpressionparser.h"
#include "sexpressionlexer.h"

SExpressionParser::SExpressionParser(QVector<shared_ptr<Token> > &tokens)
    : BaseParser(tokens)
{

}

QVector<shared_ptr<SExpression> > SExpressionParser::parse()
{
    QVector<shared_ptr<SExpression> > ret;
    while(!eof())
    {
        shared_ptr<SExpression> se = sexp();
        if(se) // if not an error
            ret.append(se);
    }
    return ret;
}

shared_ptr<SExpression> SExpressionParser::sexp()
{
    shared_ptr<SExpression> ret;
    if(LA(SExp::Num))
    {
        ret = shared_ptr<SExpression>(new Int(lookAhead->Lexeme.toInt()));
        nextToken();
    }
    else if(LA(SExp::Str))
    {
        ret = shared_ptr<SExpression>(new ::Str(lookAhead->Lexeme.mid(1, lookAhead->Lexeme.length()-2)));
        nextToken();
    }
    else if(LA(SExp::Symbol))
    {
        ret = shared_ptr<SExpression>(new Symbol(lookAhead->Lexeme));
        nextToken();
    }
    else if(LA(SExp::LParen))
    {
        shared_ptr<Token> start = lookAhead;
        nextToken();
        shared_ptr<List> lst(new Nil());
        while(true)
        {
            if(LA(SExp::RParen))
            {
                ret = lst->reverse();
                nextToken();
                break;
            }
            else if(!eof())
            {
                lst = shared_ptr<List>(new Pair(sexp(), lst));
            }
            else
            {
                errors.append(QString("List started at line %1 must end with ')'").arg(start->Line));
                break;
            }
        }
    }


    return ret;
}

