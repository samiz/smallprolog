#ifndef PL_LEXER_H
#define PL_LEXER_H

#include "./token.h"
#include "./strbuffer.h"
#include "./regexp.h"

using namespace std;
namespace Lex
{
class Lexer
{
    StrBuffer buffer;
public:
    QVector<shared_ptr<Token> > acceptedTokens;
    QStringList errors;
    QMap<TokenType, shared_ptr<RegExp> > rules;
    QSet<TokenType> skipTokens;
public:
    void init(QString s) { buffer.init(s); }
    shared_ptr<Token> accept(TokenType type);
    void tokenize();
};
}
#endif // LEXER_H
