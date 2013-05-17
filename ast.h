#ifndef AST_H
#define AST_H

#include <QStringList>
#include <QVector>
#include <QMap>
#include <memory>
using namespace std;

#include "./lexers/token.h"

struct Ast
{
    int type;
    Ast(int type=-1) : type(type) { }
    virtual shared_ptr<Token> first()=0;
    virtual shared_ptr<Token> last()=0;
    virtual QString toString()=0;
};

struct TokenAst: public Ast
{
    shared_ptr<Token> Value;
    TokenAst(shared_ptr<Token> Value, int type=-1):Ast(type), Value(Value) { }
    virtual shared_ptr<Token> first();
    virtual shared_ptr<Token> last();
    QString toString() { return Value->toString(); }
};

struct SeqAst : public Ast
{
    SeqAst(int type=-1): Ast(type) { }
    shared_ptr<Ast> at(QString s) { return subs.at(labels[s]);}
    shared_ptr<Ast> operator[](QString s) { return this->at(s);}
    void add(shared_ptr<Ast> sub) { subs.append(sub);}
    void add(shared_ptr<Ast> sub, QString label)
    {
        subs.append(sub);
        labels[label] = subs.count()-1;
        indexlabels[subs.count()-1] = label;
    }

    shared_ptr<Token> first();
    shared_ptr<Token> last();
    QString toString();
private:
    QVector<shared_ptr<Ast> > subs;
    QMap<QString, int> labels;
    QMap<int, QString> indexlabels;
};

struct VectorAst: public Ast
{
    VectorAst(int type=-1) : Ast(type) { }
    shared_ptr<Ast> at(int i) { return subs[i]; }
    void add(shared_ptr<Ast> a) { subs.append(a); }
    int count() { return subs.count(); }
    shared_ptr<Token> first();
    shared_ptr<Token> last();
    QString toString();
private:
    QVector<shared_ptr<Ast> > subs;
};

#endif // AST_H
