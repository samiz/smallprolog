#ifndef WAM_H
#define WAM_H

#include <QString>
#include <QStack>
#include <QMap>
#include <functional>
#include "../dbhelper.h"
#include "./operandstack.h"

using namespace std;
class SExpression;
namespace Wam
{
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
    DbQuery,
    DbCheck,
    TryMeElse,
    Proceed,
    Fail,
    SaveCR,
    Cut
};

struct ChoicePoint
{
    int trailIndex;
    int frameIndex;
    int cutReg;
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
    DBHelper dbHelper;
    bool result;
    bool done;
public:
    QMap<QString, int> structArities;
    QMap<QString, int> factArities;
    QMap<QString, QVector<Term::Tag> > factArgTypes;
    QMap<QString, shared_ptr<Prolog::Fact> > facts;
    QStack<Frame> callStack;
    //OperandStack operandStack;
    QStack<shared_ptr<Term::Term> > operandStack;
    TrailStack trail;
    QStack<ChoicePoint> choicePoints;
    QMap<QString, shared_ptr<Method> > predicates;
    QMap<QString, function<void(Wam &)> > externalMethods;
    inline shared_ptr<Term::Var> newVar() { return Term::makeVar(newVarCount++); }
    int IP;
    int CutReg;
    int currentFrame;
public:
    void Load(const QVector<shared_ptr<SExpression> > &code);
    void Init();
    void OpenDb();
    void CloseDb();
    void Finished();
    void Run(QString main, QMap<QString, shared_ptr<Term::Term> > bindings=QMap<QString, shared_ptr<Term::Term> >());
    void RegisterExternal(QString name, function<void(Wam &)> f);
    void query(QString tableName);
    void insert(QString tableName, const QVector<QVariant> &args) { dbHelper.insert(tableName, args); }

    void beginTransaction() { dbHelper.beginTransaction();}
    void endTransaction() { dbHelper.endTransaction(); }

    shared_ptr<Term::Term> resultToTerm(int i, QSqlQuery &q, Term::Tag type);
    void error(QString s);
public:
    Term::Tag termTypeOf(QString name);
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
}
#endif // WAM_H
