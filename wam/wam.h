#ifndef WAM_H
#define WAM_H

#include <QString>
#include <QStack>
#include <QMap>
#include <functional>
#include "../data/sexpression.h"
#include "operandstack.h"

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
    OperandStack operandStack;
    TrailStack trail;
    QStack<ChoicePoint> choicePoints;
    QMap<QString, shared_ptr<Method> > predicates;
    QMap<QString, function<void(Wam &)> > externalMethods;
    int IP;
    int currentFrame;
public:
    void Load(const QVector<shared_ptr<SExpression> > &code);
    void Init();
    void Run(QString main);
    void RegisterExternal(QString name, function<void(Wam &)> f);
    void error(QString s);
public:
    void processInstruction(shared_ptr<SExpression> inst, const shared_ptr<Method> &method, int &count);
    void backtrack();
    void bind(uint, const shared_ptr<Term::Term> &val);
    bool unifyCompound(shared_ptr<Term::Term> t1, shared_ptr<Term::Term> t2);
    bool unify(shared_ptr<Term::Term> t1, shared_ptr<Term::Term> t2);
    bool lookup(uint, shared_ptr<Term::Term> &ret);
    QString dumpTrail();
    bool ground(const shared_ptr<Term::Term> &t, shared_ptr<Term::Term> &ret);
    void fail();
    QMap<QString, shared_ptr<Term::Term> > resolveAll
    (QMap<QString, shared_ptr<Term::Term> >env);
};

#endif // WAM_H
