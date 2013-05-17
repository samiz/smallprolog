#ifndef WAM_H
#define WAM_H

#include <QString>
#include <QStack>
#include <QMap>
#include <functional>
#include "sexpression.h"
#include "terms.h"

using namespace std;
enum OpCode
{
    PushV,
    PushLocal,
    PopLocal,
    NewVar,
    NewObj,
    Unify,
    Call,
    CallEx,
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

class Wam;
void nothingFunction(Wam &);

class Wam
{

    int newVarCount;
public:
    QStringList errors;
    QVector<QMap<QString, shared_ptr<Term::Term> > > solutions;
    bool result;
    bool done;
public:
    QMap<QString, int> structArities;
    QStack<Frame> callStack;
    QStack<shared_ptr<Term::Term> > operandStack;
    QStack<Binding> trail;
    QStack<ChoicePoint> choicePoints;
    QMap<QString, shared_ptr<Method> > predicates;
    QMap<QString, function<void(Wam &)> > externalMethods;
    int IP;
    int currentFrame;
public:
    void Load(QVector<shared_ptr<SExpression> >code);
    void Init();
    void Run(QString main);
    void RegisterExternal(QString name, function<void(Wam &)> f);
    void error(QString s);
public:
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
