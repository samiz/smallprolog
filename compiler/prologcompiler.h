#ifndef PROLOGCOMPILER_H
#define PROLOGCOMPILER_H

#include <QStringList>
#include "../data/program.h"
#include "./codegenhelper.h"

namespace Prolog
{
class PrologCompiler
{
    Program &program;
    CodeGenerator g;
public:
    QStringList errors;
public:
    PrologCompiler(Program &program): program(program) { }
    void compile();
    void compileFact(shared_ptr<Fact> const &fact);
    void compileClause(QVector<shared_ptr<Clause> > &clauseBodies);
    void generateExpression(shared_ptr<Term::Term> targ, QSet<QString> &vars);
    void bindClauseHead(shared_ptr<Term::Compound> &head, QSet<QString> &vars);
    QString getOutput() { return g.getOutput(); }
};
}
#endif // PROLOGCOMPILER_H
