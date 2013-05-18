#include "prologparser.h"
#include "../lexers/prologlexer.h"

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
        nextToken();
        while(LA(Prolog::Symbol))
        {
            shared_ptr<Token> factName = chomp();
            shared_ptr<Fact> f = program.addFact(factName->Lexeme);
            match(Prolog::LParen);
            if(!LA(Prolog::RParen))
            {
                expect(Prolog::Symbol);
                shared_ptr<Token> aType = chomp();
                f->argTypes.append(aType->Lexeme);
                while(LA(Prolog::Comma))
                {
                    match(Prolog::Comma);
                    expect(Prolog::Symbol);
                    aType = chomp();
                    f->argTypes.append(aType->Lexeme);
                }
            }
            match(Prolog::RParen);
            program.addStruct(factName->Lexeme, f->argTypes.count());
        }
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
        shared_ptr<Term::Compound> ret=Term::makeCompound("=");
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
        return Term::makeSymbol(chomp()->Lexeme);
    }
    else if(LA(Prolog::Num))
    {
        return Term::makeInt(chomp()->Lexeme.toInt());
    }
    else if(LA(Prolog::Str))
    {
        shared_ptr<Token> t = chomp();
        return Term::makeString(t->Lexeme.mid(1, t->Lexeme.length()-2));
    }
    else if(LA(Prolog::Variable))
    {
        return Term::makeId(chomp()->Lexeme);
    }
    else if(LA(Prolog::AssertKw))
    {
        nextToken();
        shared_ptr<Term::Compound> t = Term::makeCompound("assert");
        match(Prolog::LParen);
        t->args.append(term());
        match(Prolog::RParen);
        return t;
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
                shared_ptr<Term::Compound> t2 = Term::makeCompound("nil");
                tail = t2;
            }
            shared_ptr<Term::Term> *current = &ret;
            for(int i=0; i<elements.count(); ++i)
            {
                shared_ptr<Term::Compound> c2 = Term::makeCompound("pair");
                c2->args.append(elements[i]);
                c2->args.append(shared_ptr<Term::Compound>());
                *current = c2;
                current = &(c2->args[1]);
            }
            *current = tail;
        }
        else
        {
            shared_ptr<Term::Compound> c2 = Term::makeCompound("nil");
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
    shared_ptr<Term::Symbol> functor = Term::makeSymbol(chomp()->Lexeme);
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
    shared_ptr<Term::Compound> ret=Term::makeCompound(functor->toString());
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
