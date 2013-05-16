#include "parser.h"

BaseParser::BaseParser(QVector<shared_ptr<Token> > &tokens)
    : tokens(tokens)
{
    this->tokenToString = NULL;
    pos = -1;
    posOfLastError = -1;
    nextToken();
}

void BaseParser::sepBy(QString separator, function<bool()> subParser, QString description)
{
    while(true)
    {
        if(!subParser())
        {
            error("Expected: %1", description);
            break;
        }
        match(separator);
    }
}

void BaseParser::sepSurroundBy(QString opener,
                           QString closer,
                           QString separator,
                           function<bool()> subParser,
                           QString description,
                           function<void()> action)
{
    match(opener);
    while(true)
    {
        if(!subParser())
        {
            error("Expected: %1", description);
            break;
        }
        action();
        match(separator);
    }
    match(closer);
}

void BaseParser::nextToken()
{
    pos++;
    if(!eof())
    {
        lookAhead = tokens[pos];
    }
}

shared_ptr<Token> BaseParser::chomp()
{
    shared_ptr<Token> ret = lookAhead;
    nextToken();
    return ret;
}

void BaseParser::expect(int type)
{
    if(LA(type))
    {
        return;
    }
    else
    {

        error(QString("Expected token of type %1, got %2 [%3]")
              .arg(tokenType(type))
              .arg(tokenType(getCurrentToken()->Type))
              .arg(getCurrentToken()->Lexeme));
    }
}

void BaseParser::match(int type)
{
    if(LA(type))
    {
        nextToken();
    }
    else
    {

        error(QString("Expected token of type %1, got %2 [%3]")
              .arg(tokenType(type))
              .arg(tokenType(getCurrentToken()->Type))
              .arg(getCurrentToken()->Lexeme));
    }
}

void BaseParser::match(QString lexeme)
{
    if(LA(lexeme))
    {
        nextToken();
    }
    else
    {
        error("Expected token: '%1', got %2", lexeme, getCurrentToken()->Lexeme);
    }
}

bool BaseParser::eof()
{
    return pos >= tokens.count();
}

bool BaseParser::LA(int type)
{
    return !eof() && lookAhead->Type == type;
}

bool BaseParser::LA2(int type1, int type2)
{
    return (pos+1 < tokens.count()) && lookAhead->Type == type1 &&
            tokens[pos+1]->Type == type2;
}

bool BaseParser::LA(QString lexeme)
{
    return !eof() && lookAhead->Lexeme == lexeme;
}

shared_ptr<Token> BaseParser::getCurrentToken()
{
    if(eof())
        return tokens[tokens.count()-1];
    else
        return lookAhead;
}

QString BaseParser::tokenType(int t)
{
    if(tokenToString == NULL)
        return QString("%1").arg(t);
    else
        return tokenToString(t);
}

void BaseParser::error(QString s)
{
    if(posOfLastError !=-1 && posOfLastError == pos)
    {
        // Two errors for the same position... quit the parser
        // to avoid infinite loop since we aren't progressing
        throw ParserException();
    }
    errors.append(QString("(%1:%2): %3)").arg(getCurrentToken()->Line)
                  .arg(getCurrentToken()->Column)
                  .arg(s));
    posOfLastError = pos;

}

void BaseParser::error(QString msg, QString arg1)
{
    error(QString(msg).arg(arg1));
}

void BaseParser::error(QString msg, int arg1)
{
    error(QString(msg).arg(arg1));
}

void BaseParser::error(QString msg, QString arg1, QString arg2)
{
    error(QString(msg).arg(arg1).arg(arg2));
}
