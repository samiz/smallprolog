#include "sexpressionlexer.h"

SExpressionLexer::SExpressionLexer()
{
    lexer.rules[SExp::Spacing] = loop1(charOf(" \t\n\r"));
    lexer.rules[SExp::LParen] = str("(");
    lexer.rules[SExp::RParen] = str(")");
    lexer.rules[SExp::Quote] = str("\'");
    lexer.rules[SExp::Str] = seq(str("\""),
                                 loop(anyBut(choice(str("\""), str("\n")))),
                                 str("\""));

    shared_ptr<RegExp> digit = charIs([](QChar c){return c.isDigit();},"<digit>");
    shared_ptr<RegExp> letter = charIs([](QChar c){return c.isLetter();}, "<letter>");
    shared_ptr<RegExp> alpha = charIs([](QChar c){return c.isLetterOrNumber();},"<alphanumeric>");
    shared_ptr<RegExp> symbol = charOf("+-/*=!?%_<>");
    shared_ptr<RegExp> digits = seq(loop1(digit), checkNo(letter));
    shared_ptr<RegExp> letterOrSymbol = choice(letter, symbol);
    shared_ptr<RegExp> alphaOrSymbol = choice(alpha, symbol);
    lexer.rules[SExp::Num] = seq(digits,
                                 optional(seq(str("."), digits)));

    lexer.rules[SExp::Symbol] = seq(letterOrSymbol, loop(choice(digit, alphaOrSymbol)));
}
