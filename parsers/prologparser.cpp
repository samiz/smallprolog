#include "prologparser.h"
#include "prologlexer.h"

namespace Prolog
{

PrologParser::PrologParser(QVector<shared_ptr<Token> > &tokens, Program &program)
    :BaseParser(tokens), program(program)
{
    this->tokenToString = Prolog::tokenToString;
}

void PrologParser::parse()
{
    domainsSection();
    factsSection();
    predicatesSection();
    clausesSection();
    goalSection();
}

void PrologParser::domainsSection()
{
    if(LA("domains"))
    {
        nextToken();
        while(LA(Prolog::Symbol))
        {
            match(Prolog::Symbol);
            match(Prolog::Eq);
            if(LA(Prolog::Symbol))
            {
                domainElement();
                while(LA(Prolog::Semi))
                {
                    match(Prolog::Semi);
                    domainElement();
                }
            }
            match(Prolog::Dot);
        }
    }
}

void PrologParser::domainElement()
{
    expect(Prolog::Symbol);
    shared_ptr<Token> sym = chomp();
    int arity = 0;
    match(Prolog::LParen);
    if(LA(Prolog::Symbol))
    {
        arity++;
        match(Prolog::Symbol);
        while(LA(Prolog::Comma))
        {
            arity++;
            match(Prolog::Comma);
            match(Prolog::Symbol);
        }
    }
    match(Prolog::RParen);
    program.addStruct(sym->Lexeme, arity);
}

void PrologParser::factsSection()
{
    if(LA("facts"))
    {
        //todo:
    }
}

void PrologParser::predicatesSection()
{
    if(LA("predicates"))
    {
        //todo:
    }
}

void PrologParser::clausesSection()
{
    if(LA("clauses"))
    {
        nextToken();
        while(LA(Prolog::Symbol))
        {
            clause();
        }
    }
}

void PrologParser::clause()
{
    shared_ptr<Clause> c = program.newClause();
    shared_ptr<Term::Compound> head = compound();
    c->head = head;
    if(LA(Prolog::Dot))
    {
        nextToken();
        program.addClause(c);
        return;
    }
    else if(LA(Prolog::OnlyIf))
    {
        nextToken();
        shared_ptr<Term::Term> t = term();
        c->body.append(t);
        while(LA(Prolog::Comma))
        {
            nextToken();
            shared_ptr<Term::Term> t = term();
            if(t)
                c->body.append(t);
        }
        match(Prolog::Dot);
        program.addClause(c);
        return;
    }
    else
    {
        errors.append("Expected term or .");
    }

}

shared_ptr<Term::Term> PrologParser::term()
{
    shared_ptr<Term::Term> t1 = simpleTerm();
    if(LA(Prolog::Eq))
    {
        match(Prolog::Eq);
        shared_ptr<Term::Term> t2 = simpleTerm();
        shared_ptr<Term::Compound> ret(new Term::Compound());

        ret->functor = shared_ptr<Term::Symbol>(new Term::Symbol("="));
        ret->args.append(t1);
        ret->args.append(t2);

        return ret;
    }
    return t1;
}

shared_ptr<Term::Term> PrologParser::simpleTerm()
{
    if(LA2(Prolog::Symbol, Prolog::LParen))
    {
        return compound();
    }
    else if(LA(Prolog::Symbol))
    {
        return shared_ptr<Term::Term>(new Term::Symbol(chomp()->Lexeme));
    }
    else if(LA(Prolog::Num))
    {
        return shared_ptr<Term::Term>(new Term::Int(chomp()->Lexeme.toInt()));
    }
    else if(LA(Prolog::Str))
    {
        shared_ptr<Token> t = chomp();
        return make_shared<Term::String>(t->Lexeme.mid(1, t->Lexeme.length()-2));
    }
    else if(LA(Prolog::Variable))
    {
        return shared_ptr<Term::Term>(new Term::Id(chomp()->Lexeme));
    }
    else if(LA(Prolog::LBracket))
    {
        match(Prolog::LBracket);
        shared_ptr<Term::Term> ret;
        shared_ptr<Term::Term> tail;
        QVector<shared_ptr<Term::Term> > elements;
        if(!LA(Prolog::RBracket))
        {
            elements.append(term());
            while(LA(Prolog::Comma))
            {
                match(Prolog::Comma);
                elements.append(term());
            }
            if(LA(Prolog::Bar))
            {
                match(Prolog::Bar);
                tail = term();
            }
            else
            {
                shared_ptr<Term::Compound> t2 = shared_ptr<Term::Compound>(new Term::Compound());
                t2->functor = shared_ptr<Term::Symbol>(new Term::Symbol("nil"));
                tail = t2;
            }
            shared_ptr<Term::Term> *current = &ret;
            for(int i=0; i<elements.count(); ++i)
            {
                shared_ptr<Term::Compound> c2 = shared_ptr<Term::Compound>(new Term::Compound());
                c2->functor = shared_ptr<Term::Symbol>(new Term::Symbol("pair"));
                c2->args.append(elements[i]);
                c2->args.append(shared_ptr<Term::Compound>());
                *current = c2;
                current = &(c2->args[1]);
            }
            *current = tail;
        }
        else
        {
            shared_ptr<Term::Compound> c2 = shared_ptr<Term::Compound>(new Term::Compound());
            c2->functor = shared_ptr<Term::Symbol>(new Term::Symbol("nil"));
            ret = c2;
        }

        match(Prolog::RBracket);
        return ret;
    }
    else
    {
        errors.append("Expected a term");
        return shared_ptr<Term::Term>();
    }
}

bool PrologParser::LA_first_term()
{
    return LA(Prolog::Symbol) || LA(Prolog::Num) || LA(Prolog::Variable)
            || LA(Prolog::Str)
            || LA(Prolog::LBracket);
}

shared_ptr<Term::Compound> PrologParser::compound()
{
    shared_ptr<Term::Symbol> functor(new Term::Symbol(chomp()->Lexeme));
    QVector<shared_ptr<Term::Term> > elements;
    match(Prolog::LParen);
    if(LA_first_term())
    {
        elements.append(term());
        while(LA(Prolog::Comma))
        {
            nextToken();
            elements.append(term());
        }
    }
    match(Prolog::RParen);
    shared_ptr<Term::Compound> ret(new Term::Compound());
    ret->functor = functor;
    ret->args +=elements;
    return ret;
}

void PrologParser::goalSection()
{
    if(LA("goal"))
    {
        //todo:
    }
}



}
