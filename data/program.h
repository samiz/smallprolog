#ifndef PROGRAM_H
#define PROGRAM_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>
#include <memory>
#include "terms.h"
using namespace std;
namespace Prolog
{
struct Domain
{
    QString name;
    Domain(QString name) : name(name) { }
};

struct Predicate
{
    QString functor;
    QVector<shared_ptr<Domain> > argDomains;
    Predicate(QString functor) :functor(functor) { }
    Predicate(QString functor,
    QVector<shared_ptr<Domain> > argDomains) :
        functor(functor), argDomains(argDomains){ }
};

struct Clause
{
    shared_ptr<Term::Compound> head;
    QVector<shared_ptr<Term::Term> > body;
    QString toString();
};

struct Fact
{
    QString name;
    QStringList argTypes;
    QStringList argNames;
    Fact(QString name):name(name){ }
};

class Program
{
public:
    QMap<QString, shared_ptr<Domain> > domains;
    QMap<QString, int> structContructors;
    QMap<QString, shared_ptr<Predicate> > predicates;
    QMap<QString, QVector<shared_ptr<Clause> > > clauses;
    QMap<QString, shared_ptr<Fact> > facts;
    QSet<QString> externalMethods;
public:
    shared_ptr<Domain> addDomain(QString name)
    {
        shared_ptr<Domain> d(new Domain(name));
        domains[name] = d;
        return d;
    }

    shared_ptr<Fact> addFact(QString name)
    {
        shared_ptr<Fact> f = make_shared<Fact>(name);
        facts[name] = f;
        return f;
    }

    void addStruct(QString name, int arity)
    {
        structContructors[name] = arity;
    }

    shared_ptr<Predicate> addPredicate(QString name)
    {
        shared_ptr<Predicate> p(new Predicate(name));
        predicates[name] = p;
        return p;
    }

    shared_ptr<Clause> newClause()
    {
        shared_ptr<Clause> c(new Clause());
        return c;
    }

    void addClause(shared_ptr<Clause> c)
    {
        clauses[c->head->functor->toString()].append(c);
    }
};
}
#endif // PROGRAM_H
