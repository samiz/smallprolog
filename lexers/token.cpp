#include "token.h"

namespace Lex
{
Token::Token(QString Lexeme, TokenType Type, int Pos, int Line, int Column)
    : Lexeme(Lexeme), Type(Type),Pos(Pos), Line(Line), Column(Column)
{
}

Token::Token(const Token &other)
    : Lexeme(other.Lexeme), Type(other.Type),Pos(other.Pos), Line(other.Line), Column(other.Column)
{
}

Token &Token::operator = (const Token &other)
{
    Lexeme = other.Lexeme;
    Type = other.Type;
    Pos = other.Pos;
    Line = other.Line;
    Column = other.Column;
    return *this;
}

bool Token::operator ==(Token &t2)
{
    return Type==t2.Type && Lexeme == t2.Lexeme && Line == t2.Line && Column == t2.Column && Pos == t2.Pos;
}
bool Token::operator !=(Token &t2)
{
    return Type!=t2.Type || Lexeme != t2.Lexeme  || Line != t2.Line || Column != t2.Column || Pos != t2.Pos;
}
bool Token::Is(const QString &lexeme)
{
    return Lexeme == lexeme;
}

bool Token::Is(TokenType type)
{
    return Type == type;
}

}
