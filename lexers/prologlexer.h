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

        // For now we'll consider '=' a symbol character except when
        // on its own, so make 'Eq' come before 'Symbol'

        Eq,
        DomainsKw,
        PredicatesKw,
        ClausesKw,
        AssertKw,
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
        Lt,
        Gt,
        Le,
        Ge,
        Ne,
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
