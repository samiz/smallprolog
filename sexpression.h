#ifndef SEXPRESSION_H
#define SEXPRESSION_H

#include <memory>
#include <QVector>
#include <QStringList>

using namespace std;

class List;
struct SExpression
{
    virtual bool match(int i) { return false; }
    virtual bool match(QString s) { return false; } // symbols
    virtual bool matchStr(QString s) { return false; } // strings

    virtual bool matchGet(int &i) { return false; }
    virtual bool matchGet(QString &s) { return false; } // symbols
    virtual bool matchStrGet(QString &s) { return false; } // strings

    virtual bool match(QString s, int &i) { return false; }
    virtual bool match(QString s, QString &s2) { return false; }
    virtual bool match(QString s, shared_ptr<List> &tail) { return false; }
    virtual bool match(int i, shared_ptr<List> &tail) { return false; }

    virtual bool match(QString s, int &i, shared_ptr<List> &tail) { return false; }
    virtual bool match(QString s, QString &s2, shared_ptr<List> &tail) { return false; }
    virtual bool match(QString s, QString &s2, int &i, shared_ptr<List> &tail) { return false; }
    virtual bool matchLst(QString s) { return false; }
    virtual bool matchLstStr(QString s) { return false; }

    virtual QString toString()=0;
};

struct Int : public SExpression
{
    int Value;
    Int(int Value) : Value(Value) { }
    bool match(int i) { return i == Value; }
    bool matchGet(int &i) { i= Value; return true; }
    QString toString()
    {
        return QString("%1").arg(Value);
    }
};

struct Symbol: public SExpression
{
    QString Value;
    Symbol(QString Value) : Value(Value) { }
    bool match(QString s) { return s == Value; }
    bool matchGet(QString &s) { s= Value; return true; }
    QString toString()
    {
        return Value;
    }
};

struct Str: public SExpression
{
    QString Value;
    Str(QString Value) : Value(Value) { }
    bool matchStr(QString s) { return s == Value; }
    bool matchStrGet(QString &s) { s= Value; return true; }
    QString toString()
    {
        return QString("\"%1\"").arg(Value);
    }
};

struct List : public SExpression
{
    virtual void forEach(function<void(shared_ptr<SExpression>)> todo)=0;
    virtual shared_ptr<List> reverse()=0;
};

struct Nil : public List
{
    void forEach(function<void(shared_ptr<SExpression>)> todo)
    {

    }
    shared_ptr<List> reverse() { return shared_ptr<List>(new Nil()); }
    QString toString()
    {
        return "()";
    }
};

struct Pair : public List
{
    shared_ptr<SExpression> head;
    shared_ptr<List> tail;
    Pair(shared_ptr<SExpression> head, shared_ptr<List> tail)
        : head(head), tail(tail)
    {

    }
    Pair() { }
    shared_ptr<List> reverse()
    {
        shared_ptr<List> ret(new Pair(head, shared_ptr<List>(new Nil)));
        shared_ptr<List> tt = tail;
        while(true)
        {
            shared_ptr<Pair> t = dynamic_pointer_cast<Pair>(tt);
            if(!t)
                break;
            ret = shared_ptr<List>(new Pair(t->head, ret));
            tt = t->tail;
        }

        return ret;
    }
    void forEach(function<void(shared_ptr<SExpression>)> todo)
    {
        todo(head);
        if(tail)
        {
            tail->forEach(todo);
        }
    }

    virtual bool match(QString s, int &i)
    {
        if(head->match(s))
        {
            shared_ptr<Pair> t = dynamic_pointer_cast<Pair>(tail);
            if(t)
            {
                return t->head->matchGet(i);
            }
        }
        return false;
    }
    virtual bool match(QString s, QString &s2)
    {
        if(head->match(s))
        {
            shared_ptr<Pair> t = dynamic_pointer_cast<Pair>(tail);
            if(t)
            {
                return t->head->matchGet(s2);
            }
        }
        return false;
    }

    virtual bool match(QString s, shared_ptr<List> &tail)
    {
        if(head->match(s))
        {
            tail = this->tail;
            return true;
        }
        return false;
    }

    virtual bool match(int i, shared_ptr<List> &tail)
    {
        if(head->match(i))
        {
            tail = this->tail;
            return true;
        }
        return false;
    }

    virtual bool match(QString s, int &i, shared_ptr<List> &tail)
    {
        if(head->match(s))
        {
            shared_ptr<Pair> t1 = dynamic_pointer_cast<Pair>(this->tail);
            if(t1 && t1->head->matchGet(i))
            {
                tail = t1->tail;
                return true;
            }
        }
        return false;
    }

    virtual bool match(QString s, QString &s2, shared_ptr<List> &tail)
    {
        if(head->match(s))
        {
            shared_ptr<Pair> t1 = dynamic_pointer_cast<Pair>(this->tail);
            if(t1 && t1->head->matchGet(s2))
            {
                tail = t1->tail;
                return true;
            }
        }
        return false;
    }

    virtual bool match(QString s, QString &s2, int &i, shared_ptr<List> &tail)
    {
        if(head->match(s))
        {
            shared_ptr<Pair> t1 = dynamic_pointer_cast<Pair>(this->tail);
            if(t1 && t1->head->matchGet(s2))
            {
                shared_ptr<Pair> t2 = dynamic_pointer_cast<Pair>(t1->tail);
                if(t2 && t2->head->matchGet(i))
                {
                    tail = t2->tail;
                    return true;
                }
            }
        }
        return false;
    }

    virtual bool matchLst(QString s)
    {
        if(head->match(s))
        {
            shared_ptr<Nil> t1 = dynamic_pointer_cast<Nil>(this->tail);
            if(t1)
            {
                return true;
            }
        }
        return false;
    }

    virtual bool matchLstStr(QString s)
    {
        if(head->matchStr(s))
        {
            shared_ptr<Nil> t1 = dynamic_pointer_cast<Nil>(this->tail);
            if(t1)
            {
                return true;
            }
        }
        return false;
    }

    QString toString()
    {
        QStringList lst;
        this->forEach([&lst](shared_ptr<SExpression> ex)
        {
            lst.append(ex->toString());
        });

        return QString("(%1)").arg(lst.join(" "));
    }



};

template<class T> QVector<T> map(shared_ptr<List> list, function<T(shared_ptr<SExpression>)> todo)
{
    QVector<T> ret;
    list->forEach([&ret,&todo](shared_ptr<SExpression> sx) {
        ret.append(todo(sx));
    });
    return ret;
}

#endif // SEXPRESSION_H
