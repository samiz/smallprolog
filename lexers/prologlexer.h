#ifndef PROLOGLEXER_H
#define PROLOGLEXER_H

#include "lexer.h"
namespace Prolog
{
    enum TokenType
    {
        // The lexer uses the types
        // to prioritize so make sure
        // to have keyword types come before the type 'Symbol'

        DomainsKw,
        PredicatesKw,
        ClausesKw,
        Symbol,
        Variable,
        Str,
        Num,
        Spacing,
        LParen,
        RParen,
        OnlyIf,
        LBracket,
        RBracket,
        Eq,
        Dot,
        Comma,
        Semi,
        Bar

    };

QString tokenToString(int type);
class PrologLexer
{
public:
    Lexer lexer;
public:
    PrologLexer();

};
}

#endif // PROLOGLEXER_H
