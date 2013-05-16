#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include "token.h"
#include "buffer.h"

shared_ptr<Token> tokenAt(QVector<shared_ptr<Token> >&v, int i);

class TokenBuffer : public Buffer<
    QVector<shared_ptr<Token> >,
    shared_ptr<Token>,
    tokenAt,
    BufferState<TokenBuffer>,
        TokenBuffer
        >
{
public:
     typedef BufferState<TokenBuffer> State;
    int GetPos()
    {
        return p1;
    }
};

class ParserException
{

};

class BaseParser
{
public:
    QStringList errors;
protected:
     QVector<shared_ptr<Token> > &tokens;
     int pos;
     int posOfLastError;
     shared_ptr<Token> lookAhead;
     QString (*tokenToString)(int);
public:
     BaseParser(QVector<shared_ptr<Token> > &tokens);
protected:
     void sepBy(QString separator, function<bool()> subParser, QString descriptopn);

     void sepSurroundBy(QString opener,
                        QString closer,
                        QString separator,
                        function<bool()> subParser,
                        QString description,
                        function<void()> action);
protected:
    void nextToken();
    shared_ptr<Token> chomp();
    void expect(int type);
    void match(int type);
    void match(QString lexeme);
    bool eof();
    bool LA(int type);
    bool LA(QString lexeme);
    bool LA2(int type1, int type2);
    shared_ptr<Token> getCurrentToken();
    void error(QString);
    void error(QString msg, QString arg1);
    void error(QString, int);
    void error(QString msg, QString arg1, QString arg2);
    QString tokenType(int);
};

#endif // PARSER_H
