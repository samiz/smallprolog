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
    for(auto i=program.facts.begin(); i!=program.facts.end();++i)
    {
        compileFact(i.value());
    }
    for(auto i=program.clauses.begin(); i!=program.clauses.end();++i)
    {
        compileClause(i.value());
    }
}

void PrologCompiler::compileFact(shared_ptr<Fact> const &fact)
{
    /*
     How a compiled fact looks like:
     (struct parent 2)
     (fact parent symbol symbol)
     (predicate parent
        (dbquery parent)
        (label q (dbcheck parent))
        (proceed)
     )

     dbquery: uses N arguments on the operand stack to query the database for the given table name
              it then leaves on the stack a 'query' object representing the returned value
              (N of course is the number of columns in the table)

     dbcheck: if the query has zero remaining results, fails and pops N+1 items from
                  the operand stack (the args and the query object)

              if the query has at least one remaining result, does this:
                put a choice point (in case there are future results) using the q label
                attempt to unify the values on the stack with the query result
     */
    g.gen("(fact %1 %2)", fact->name, fact->argTypes.join(" "));
    g.gen("(predicate %1", fact->name);
    g.gen("(dbquery %1)", fact->name);
    g.gen("(label q (dbcheck %1))", fact->name);
    g.gen("(proceed))");
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
                // If it's in the form not(...), we'll negate
                // like so:
                /*
                 not(A) -->
                 (try_me_else lbl1)
                 ....A
                 (fail)
                 (label lbl1)
                */
                else if(tc->functor->toString()=="not")
                {
                    if(tc->args.count() !=1)
                    {
                        errors.append(QString("Expression %1: not must have exactly one arg").arg(tc->toString()));
                    }
                    else
                    {
                        QString lbl = g.uniqueLabel();
                        g.gen("(savecr)");
                        g.gen("(try_me_else %1)", lbl);

                        shared_ptr<Term::Compound> invokation
                                = dynamic_pointer_cast<Term::Compound>(tc->args[0]);
                        if(invokation)
                        {
                            generateInvokation(invokation, vars);
                            g.gen("(cut)");
                            g.gen("(fail)");
                            g.gen(("(label %1)"), lbl);
                        }
                        else
                        {
                            errors.append(QString("Expression %1: arg to 'not' must be an invokation").arg(tc->toString()));
                        }
                    }
                }
                else if(tc->functor->toString() == "assert")
                {
                    generateExpression(tc->args[0], vars);
                    g.gen(("(callex assert)"));
                }
                else
                {
                    // Otherwise let's invoke
                    generateInvokation(tc, vars);
                }
            }
        }

        g.gen("(proceed)");
    }
    g.gen(")");
}

void PrologCompiler::generateInvokation(shared_ptr<Term::Compound> tc, QSet<QString> &vars)
{
    // First push args:
    for(int k=tc->args.count()-1; k>=0; --k)
    {
        shared_ptr<Term::Term> targ = tc->args[k];
        generateExpression(targ, vars);
    }

    // Done pushing args, now call:
    if(program.externalMethods.contains(tc->functor->toString()))
    {
        g.gen("(callex %1)", tc->functor->toString());
    }
    else
    {
        g.gen("(call %1)", tc->functor->toString());
    }
}

void PrologCompiler::generateExpression(shared_ptr<Term::Term> targ, QSet<QString> &vars)
{
    if(targ->tag == Term::TermId)
    {
        if(targ->toString()=="_")
        {
            g.gen("(newv)");
        }
        else
        {
            if(!vars.contains(targ->toString()))
            {
                g.gen("(newv)");
                g.gen("(popl %1)", targ->toString());
                vars.insert(targ->toString());
            }
            g.gen("(pushl %1)", targ->toString());
        }
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
            errors.append(QString("Structure %1 arity use is %2, declared is %3")
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
