#ifndef PROLOGPARSER_H
#define PROLOGPARSER_H

#include "parser.h"
#include "program.h"

namespace Prolog
{
class PrologParser : public BaseParser
{
public:
    Program &program;
public:
    PrologParser(QVector<shared_ptr<Token> > &tokens, Program &program);
    void parse();
    void domainsSection();
    void domainElement();
    void factsSection();
    void predicatesSection();
    void clausesSection();
    void goalSection();
    //--------
    void clause();
    shared_ptr<Term::Term> term();
    shared_ptr<Term::Term> simpleTerm();
    shared_ptr<Term::Compound> compound();

    bool LA_first_term();
};
}
#endif // PROLOGPARSER_H
