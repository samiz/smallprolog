#ifndef SEXPRESSIONLEXER_H
#define SEXPRESSIONLEXER_H

#include "./lexer.h"
namespace SExp
{
    enum TokenType
    {
        Spacing,
        LParen,
        RParen,
        Quote,
        Str,
        Num,
        Symbol
    };
}

class SExpressionLexer
{
public:
    Lex::Lexer lexer;
public:
    SExpressionLexer();

};

#endif // SEXPRESSIONLEXER_H
