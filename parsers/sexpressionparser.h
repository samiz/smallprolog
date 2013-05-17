#ifndef SEXPRESSIONPARSER_H
#define SEXPRESSIONPARSER_H

#include "parser.h"
#include "../data/sexpression.h"

class SExpressionParser : public BaseParser
{
public:
    SExpressionParser(QVector<shared_ptr<Token> > &tokens);
    QVector<shared_ptr<SExpression> > parse();
    shared_ptr<SExpression> sexp();
};

#endif // SEXPRESSIONPARSER_H
