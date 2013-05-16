#include "prologcompiler.h"
#include <QSet>
namespace Prolog
{
void PrologCompiler::compile()
{
    for(auto i=program.structContructors.begin(); i!=program.structContructors.end(); ++i)
    {
        g.gen("(struct %1 %2)", i.key(), i.value());
    }
    for(auto i=program.clauses.begin(); i!=program.clauses.end();++i)
    {
        compileClause(i.value());
    }
}

void PrologCompiler::compileClause(QVector<shared_ptr<Clause> > &clauseBodies)
{
    /*
     (predicate a
        (popl @a0)
        (popl @a1)

        (try_me_or_else lbl1)
        .....
        (label lbl1)
        (try_me_or_else lbl2)
        .....
        (label lbl2)
        ....
     */
    g.gen("(predicate %1 \n", clauseBodies[0]->head->functor->toString());
    QString nextLabel = g.uniqueLabel();
    for(int i=0; i<clauseBodies[0]->head->args.count(); ++i)
    {
        g.gen("(popl _A%1)", i);
    }

    for(int i=0; i<clauseBodies.count(); ++i)
    {
        QSet<QString> vars;
        const shared_ptr<Clause> &c = clauseBodies.at(i);
        if(i>0)
        {
            g.gen("(label %1)", nextLabel);
            nextLabel = g.uniqueLabel();
        }
        if(i+1 < clauseBodies.count())
        {
            // Not the last...
            g.gen("(try_me_else %1)", nextLabel);
        }

        bindClauseHead(c->head, vars);

        for(int j=0; j<c->body.count(); ++j)
        {
            shared_ptr<Term::Term> &t = c->body[j];
            if(t->tag == Term::TermCompund)
            {

                shared_ptr<Term::Compound> tc = dynamic_pointer_cast<Term::Compound>(t);

                // If it's in the form ...=... we'll unify
                if(tc->functor->toString()=="=")
                {
                    generateExpression(tc->args[0], vars);
                    generateExpression(tc->args[1], vars);
                    g.gen("(unify)");

                }
                else
                {
                    // Otherwise let's invoke
                    for(int k=tc->args.count()-1; k>=0; --k)
                    {
                        shared_ptr<Term::Term> targ = tc->args[k];
                        generateExpression(targ, vars);
                    }
                    // done pushing args
                    g.gen("(call %1)", tc->functor->toString());
                }
            }
        }

        g.gen("(proceed)");
    }
    g.gen(")");
}

void PrologCompiler::generateExpression(shared_ptr<Term::Term> targ, QSet<QString> &vars)
{
    if(targ->tag == Term::TermId)
    {
        if(!vars.contains(targ->toString()))
        {
            g.gen("(newv)");
            g.gen("(popl %1)", targ->toString());
            vars.insert(targ->toString());
        }
        g.gen("(pushl %1)", targ->toString());
    }
    else if(atomic(targ->tag))
    {
        g.gen("(pushv %1)", targ->toString());
    }
    else if(targ->tag == Term::TermCompund)
    {
        shared_ptr<Term::Compound> cc = dynamic_pointer_cast<Term::Compound>(targ);
        if(!program.structContructors.contains(cc->functor->toString()))
        {
            errors.append(QString("Structure not defined: %1").arg(cc->functor->toString()));
            return;
        }
        int declaredArity = program.structContructors[cc->functor->toString()];
        if(!(declaredArity==cc->args.count()))
        {
            errors.append(QString("Structure arity use is %1, declared is %2")
                          .arg(cc->functor->toString())
                          .arg(cc->args.count())
                          .arg(declaredArity));
            return;
        }
        for(int i=cc->args.count()-1; i>=0; --i)
        {
            generateExpression(cc->args[i], vars);
        }
        g.gen("(new %1)", cc->functor->toString());
    }
}

void PrologCompiler::bindClauseHead(shared_ptr<Term::Compound> &head, QSet<QString> &vars)
{

    for(int i=0; i<head->args.count(); ++i)
    {
        g.gen("(pushl _A%1)", i);
        shared_ptr<Term::Term> &arg = head->args[i];
        generateExpression(arg, vars);
        g.gen("(unify)");
    }
}
}
