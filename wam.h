#ifndef WAM_H
#define WAM_H

#include <QString>
#include <QStack>
#include <QMap>
#include "sexpression.h"
#include "terms.h"

enum OpCode
{
    PushV,
    PushLocal,
    PopLocal,
    NewVar,
    NewObj,
    Unify,
    Call,
    TryMeElse,
    Proceed
};

struct ChoicePoint
{
    int trailIndex;
    int frameIndex;
    QString continuation; // a label
};

struct Instruction
{
    OpCode opcode;
    shared_ptr<Term::Term> arg;
    Instruction(OpCode opcode, shared_ptr<Term::Term> arg) : opcode(opcode), arg(arg) { }
    Instruction(OpCode opcode) : opcode(opcode) { }
    Instruction(){ }
    QString toString();
};

struct Method
{
    QVector<Instruction> Instructions;
    QMap<QString, int> labels;
    QString name;
    QString toString();
};

struct Frame
{
    shared_ptr<Method> method;
    QMap<QString, shared_ptr<Term::Term> > Environment;
    int Ip;
    int parentFrame;

};

struct Binding
{
    QString var;
    shared_ptr<Term::Term> val;
    Binding(QString var, shared_ptr<Term::Term> val)
        :var(var), val(val)
    {
    }

    Binding() { }
};

QString EnvToString(QMap<QString, shared_ptr<Term::Term> > env);

class Wam
{
public:
    QVector<QMap<QString, shared_ptr<Term::Term> > > solutions;
    QStringList errors;
    int newVarCount;
    bool result;
    bool done;
private:
    QMap<QString, int> structArities;
    QStack<Frame> callStack;
    QStack<shared_ptr<Term::Term> > operandStack;
    QStack<Binding> trail;
    QStack<ChoicePoint> choicePoints;
    QMap<QString, shared_ptr<Method> > predicates;
    int IP;
    int currentFrame;
public:
    void Load(QVector<shared_ptr<SExpression> >code);
    void Init();
    void Run(QString main);
private:
    void processInstruction(shared_ptr<SExpression> inst, shared_ptr<Method> method, int &count);
    void backtrack();
    void bind(QString, const shared_ptr<Term::Term> &val);
    bool unifyCompound(shared_ptr<Term::Term> t1, shared_ptr<Term::Term> t2);
    bool unify(shared_ptr<Term::Term> t1, shared_ptr<Term::Term> t2);
    bool lookup(QString, shared_ptr<Term::Term> &ret, int &bindIndex);
    QString dumpTrail();
    bool ground(const shared_ptr<Term::Term> &t, shared_ptr<Term::Term> &ret);
    void fail();
    QMap<QString, shared_ptr<Term::Term> > resolveAll
    (QMap<QString, shared_ptr<Term::Term> >env);
};

#endif // WAM_H
