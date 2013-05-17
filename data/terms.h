#ifndef TERMS_H
#define TERMS_H

#include <memory>
#include <QStringList>
#include <QVector>
#include <QDebug>
using namespace std;

namespace Term
{
enum Tag
{
    TermInt,
    TermSymbol,
    TermStr,
    TermId,
    TermVar,
    TermCompund
};



inline bool atomic(Tag tag)
{
    return tag == TermInt || tag == TermSymbol || tag == TermStr;
}

struct Term
{
    Tag tag;
    Term(Tag tag): tag(tag) { }
    virtual QString toString()=0;
    virtual bool equals(shared_ptr<Term>) { return false; }
    virtual bool lt(const shared_ptr<Term> &)
    {
        return false;
    }
    virtual bool ground() { return false; }
    virtual uint var() { return -1;}
};

struct Atom : public Term
{
    Atom(Tag tag) : Term(tag){ }
    virtual bool ground() { return true; }
};

struct Int : public Atom
{
    int value;
    Int(int value) : Atom(TermInt), value(value) { }
    QString toString() { return QString("%1").arg(value); }
    bool equals(shared_ptr<Term> t2)
    {
        shared_ptr<Int> t22 = dynamic_pointer_cast<Int>(t2);
        if(t22)
        {
            return t22->value == this->value;
        }
        return false;
    }
    bool lt(const shared_ptr<Term> &t2)
    {
        shared_ptr<Int> t22 = dynamic_pointer_cast<Int>(t2);
        if(t22)
        {
            return this->value < t22->value;
        }
        return false;
    }
};

struct Symbol : public Atom
{
    QString value;
    Symbol(QString value) : Atom(TermSymbol), value(value) { }
    QString toString() { return value; }
    bool equals(shared_ptr<Term> t2)
    {
        shared_ptr<Symbol> t22 = dynamic_pointer_cast<Symbol>(t2);
        if(t22)
        {
            return t22->value == this->value;
        }
        return false;
    }
    bool lt(const shared_ptr<Term> &t2)
    {
        shared_ptr<Symbol> t22 = dynamic_pointer_cast<Symbol>(t2);
        if(t22)
        {
            return this->value < t22->value;
        }
        return false;
    }
};

struct String : public Atom
{
    QString value;
    String(QString value) : Atom(TermStr), value(value) { }
    QString toString() { return QString("\"%1\"").arg(value); }
    bool equals(shared_ptr<Term> t2)
    {
        shared_ptr<String> t22 = dynamic_pointer_cast<String>(t2);
        if(t22)
        {
            return t22->value == this->value;
        }
        return false;
    }
    bool lt(const shared_ptr<Term> &t2)
    {
        shared_ptr<String> t22 = dynamic_pointer_cast<String>(t2);
        if(t22)
        {
            return this->value < t22->value;
        }
        return false;
    }
};

struct Id : public Term
{
    QString name;
    Id(QString name) : Term(TermId), name(name) { }
    QString toString() { return name; }
};

struct Var : public Term
{
    uint name;
    QString toString() { return QString("v%1").arg(name); }
    Var(uint name) : Term(TermVar), name(name) { }
    uint var() { return name;}
};

/*
struct Cell : public Term
{
    shared_ptr<Term> inner;
    bool free;
    Cell() :
        Term(TermCell),
        free(true)
    { }
    void set(shared_ptr<Term> inner)
    {
        shared_ptr<Term> &ii = this->inner;
        while(ii->tag == TermCell)
        {
            shared_ptr<Cell> c = dynamic_pointer_cast<Cell>(ii);
            ii = c->inner;
        }
        ii = inner;
        free = false;
    }

    QString toString() { return QString("Cell(%1)").arg(inner->toString()); }
};
*/

struct Compound : public Term
{
    shared_ptr<Atom> functor;
    QVector<shared_ptr<Term> > args;
    Compound() : Term(TermCompund) { }
    Compound(shared_ptr<Atom> functor) :
        Term(TermCompund),
        functor(functor)
    {

    }
    QString toString()
    {
        if(functor->toString() == "nil")
        {
            return "[]";
        }
        else if(functor->toString() == "pair")
        {
            return formatList();
        }
        QStringList lst;
        for(int i=0; i<args.count(); ++i)
        {
            lst.append(args[i]->toString());
        }
        return QString("%1(%2)").arg(functor->toString()).arg(lst.join(", "));
    }
    QString formatList()
    {
        QStringList ret;
        ret.append(args[0]->toString());

        shared_ptr<Compound> c = dynamic_pointer_cast<Compound>(args[1]);
        while(c && c->functor->toString() == "pair")
        {
            ret.append(c->args[0]->toString());
            c = dynamic_pointer_cast<Compound>(c->args[1]);
        }
        return QString("[%1]").arg(ret.join(", "));
    }

    bool ground()
    {
        for(auto i=args.begin(); i!=args.end();++i)
        {
            if(!(*i)->ground())
                return false;
        }
        return true;
    }

    bool equals(shared_ptr<Term> t2)
    {
        shared_ptr<Compound> c2 = dynamic_pointer_cast<Compound>(t2);
        if(c2)
        {
            if(this->functor->equals(c2->functor))
            {
                for(int i=0; i<args.count(); ++i)
                {
                    if(!(args[i]->equals(c2->args[i])))
                        return false;
                }
                return true;
            }
        }
        return false;
    }
};

inline shared_ptr<Int> makeInt(int i) { return make_shared<Int>(i); }
inline shared_ptr<Symbol> makeSymbol(QString i) { return make_shared<Symbol>(i); }
inline shared_ptr<Var> makeVar(int i) { return make_shared<Var>(i); }
inline shared_ptr<Id> makeId(QString i) { return make_shared<Id>(i); }
inline shared_ptr<String> makeString(QString i) { return make_shared<String>(i); }
inline shared_ptr<Compound> makeCompound(QString i)
{
   return make_shared<Compound>(makeSymbol(i));
}

}
#endif // TERMS_H
