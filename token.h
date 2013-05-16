#ifndef TOKEN_H
#define TOKEN_H

#include <QString>

typedef int TokenType;

struct Token
{
    QString Lexeme;
    int Type;
    int Pos, Line, Column;

    Token(QString Lexeme, TokenType Type, int Pos, int Line, int Column);
    Token(Token const &other);
    bool operator ==(Token &t2);
    bool operator !=(Token &t2);
    Token &operator = (const Token &other);
    bool Is(const QString &lexeme);
    bool Is(TokenType type);

    QString toString() { return QString("Token:'%1',[line=%2,col=%3,pos=%4]").arg(Lexeme)
                   .arg(Line).arg(Column).arg(Pos); }



};

#endif // TOKEN_H
