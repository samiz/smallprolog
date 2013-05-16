#include "prologlexer.h"

namespace Prolog
{
PrologLexer::PrologLexer()
{

    lexer.rules[Prolog::Spacing] = loop1(charOf(" \t\n\r"));
    lexer.rules[Prolog::LParen] = str("(");
    lexer.rules[Prolog::RParen] = str(")");
    lexer.rules[Prolog::LBracket] = str("[");
    lexer.rules[Prolog::RBracket] = str("]");
    lexer.rules[Prolog::OnlyIf] = str(":-");
    lexer.rules[Prolog::Eq] = str("=");
    lexer.rules[Prolog::Dot] = str(".");
    lexer.rules[Prolog::Comma] = str(",");
    lexer.rules[Prolog::Semi] = str(";");
    lexer.rules[Prolog::Bar] = str("|");
    lexer.rules[Prolog::DomainsKw] = str("domains");
    lexer.rules[Prolog::PredicatesKw] = str("predicates");
    lexer.rules[Prolog::ClausesKw] = str("clauses");

    lexer.rules[Prolog::Str] = seq(str("\""),
                                 loop(anyBut(choice(str("\""), str("\n")))),
                                 str("\""));

    shared_ptr<RegExp> digit = charIs([](QChar c){return c.isDigit();},"<digit>");
    shared_ptr<RegExp> letter = charIs([](QChar c){return c.isLetter();}, "<letter>");
    shared_ptr<RegExp> smallLetter = charIs([](QChar c){return c.isLower();}, "<lowercase>");
    shared_ptr<RegExp> capitalLetter = charIs([](QChar c){return c.isUpper();}, "<uppercase>");
    shared_ptr<RegExp> alpha = charIs([](QChar c){return c.isLetterOrNumber();},"<alphanumeric>");
    shared_ptr<RegExp> symbol = charOf("_");
    shared_ptr<RegExp> digits = seq(loop1(digit), checkNo(letter));
    //shared_ptr<RegExp> letterOrSymbol = choice(letter, symbol);
    shared_ptr<RegExp> alphaOrSymbol = choice(alpha, symbol);

    lexer.rules[Prolog::Num] = seq(digits,
                                 optional(seq(str("."), digits)));

    lexer.rules[Prolog::Symbol] = seq(smallLetter, loop(choice(digit, alphaOrSymbol)));
    lexer.rules[Prolog::Variable] = seq(choice(capitalLetter, str("_")),
                                        loop(choice(digit, alphaOrSymbol)));

}

    QString tokenToString(int type)
    {
        switch(type)
        {
        case Symbol: return "Symbol";
        case Variable: return "Variable";
        case Str: return "Str";
        case Num: return "Num";
        case Spacing: return "Spacing";
        case LParen: return "LParen";
        case RParen: return "RParen";
        case OnlyIf: return "OnlyIf";
        case LBracket: return "LBracket";
        case RBracket: return "RBracket";
        case Eq: return "Eq";
        case Dot: return "Dot";
        case Comma: return "Comma";
        case Semi: return "Semi";
        case Bar: return "Bar";
        case DomainsKw: return "domains";
        case PredicatesKw: return "predicates";
        case ClausesKw: return "clauses";

        }
        return "<unknown>";
    }
}

