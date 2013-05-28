#include "wam.h"
#include "../data/sexpression.h"
#include <QDebug>
namespace Wam
{
void Wam::Load(QVector<shared_ptr<SExpression> > const &code)
{
    for(auto i=code.begin(); i!=code.end(); ++i)
    {
        const shared_ptr<SExpression> &expr = *i;
        shared_ptr<List> rest;
        QString predName, factName, structName;
        int arity;
        if(expr->match("predicate", predName, rest))
        {
            shared_ptr<Method> method(new Method());
            method->name = predName;
            int count =0;
            rest->forEach([this, &method, &count](shared_ptr<SExpression> inst){
                processInstruction(inst, method, count);
                count++;
            });
            predicates[predName] = method;
            //errors.append(QString("Loaded %1").arg(method->toString()));
#if defined(QT_DEBUG) && defined(PL_VERBOSE_DEBUGGING)
            qDebug() << "Loaded: " << method->toString();
#endif
        }
        else if(expr->match("struct", structName, arity, rest))
        {
            structArities[structName] = arity;
        }
        else if(expr->match("fact", factName, rest))
        {
            shared_ptr<Prolog::Fact> fact = make_shared<Prolog::Fact>(factName);
            arity = 0;

            factArgTypes[factName] = QVector<Term::Tag>();
            rest->forEach([this, &factName, &fact,&arity](shared_ptr<SExpression> tt){
                this->factArgTypes[factName].append(termTypeOf(tt->toString()));
                fact->argTypes.append(tt->toString());
                arity++;
            });
            factArities[factName] = arity;
            facts[factName] = fact;
        }
        else
        {
            errors.append(QString("Invalid format: %1").arg(expr->toString()));
        }
    }
}

Term::Tag Wam::termTypeOf(QString name)
{
    if(name=="symbol")
        return Term::TermSymbol;
    else if(name=="int")
        return Term::TermInt;
    else if(name=="string")
        return Term::TermStr;
    else
    {
        error(QString("termTypeOf: unhandled term type %1").arg(name));
        return Term::TermInt;
    }
}

void Wam::processInstruction(shared_ptr<SExpression> inst, shared_ptr<Method> const &method, int &count)
{
    int ival;
    QString sval;
    shared_ptr<List> tail;
    if(inst->match("label", sval, tail))
    {
        method->labels[sval] = count;
        shared_ptr<Pair> p = dynamic_pointer_cast<Pair>(tail);
        // if the tail is empty, it's just a label, otherwise process it
        // without increasing 'count'
        if(p)
        {
            processInstruction(p->head, method, count);
        }
        else
        {
            // an empty label doesn't "count" as an instruction
            count--;
        }
    }
    else if(inst->match("pushv", ival))
    {
        method->Instructions.append(Instruction(PushV, Term::makeInt(ival)));
    }
    else if(inst->match("pushv", sval))
    {
        method->Instructions.append(Instruction(PushV, Term::makeSymbol(sval)));
    }
    else if(inst->match("pushv", tail) && tail->matchLstStrGet(sval))
    {
        method->Instructions.append(Instruction(PushV, Term::makeString(sval)));
    }
    else if(inst->match("pushl", sval))
    {
        method->Instructions.append(Instruction(PushLocal, Term::makeSymbol(sval)));
    }
    else if(inst->match("popl", sval))
    {
        method->Instructions.append(Instruction(PopLocal, Term::makeSymbol(sval)));
    }
    else if(inst->matchLst("newv"))
    {
        method->Instructions.append(Instruction(NewVar));
    }
    else if(inst->match("new", sval))
    {
        method->Instructions.append(Instruction(NewObj, Term::makeSymbol(sval)));
    }
    else if(inst->matchLst("unify"))
    {
        method->Instructions.append(Instruction(Unify));
    }
    else if(inst->match("call", sval))
    {
        method->Instructions.append(Instruction(Call, Term::makeSymbol(sval)));
    }
    else if(inst->match("callex", sval))
    {
        method->Instructions.append(Instruction(CallEx, Term::makeSymbol(sval)));
    }
    else if(inst->match("dbquery", sval))
    {
        method->Instructions.append(Instruction(DbQuery, Term::makeSymbol(sval)));
    }
    else if(inst->match("dbcheck", sval))
    {
        method->Instructions.append(Instruction(DbCheck, Term::makeSymbol(sval)));
    }
    else if(inst->match("try_me_else", sval))
    {
        method->Instructions.append(Instruction(TryMeElse, Term::makeSymbol(sval)));
    }
    else if(inst->matchLst("proceed"))
    {
        method->Instructions.append(Instruction(Proceed));
    }
    else if(inst->matchLst("fail"))
    {
        method->Instructions.append(Instruction(Fail));
    }
    else if(inst->matchLst("savecr"))
    {
        method->Instructions.append(Instruction(SaveCR));
    }
    else if(inst->matchLst("cut"))
    {
        method->Instructions.append(Instruction(Cut));
    }
    else
    {
        errors.append(QString("Invalid instruction format: %1").arg(inst->toString()));
    }
}

void nothingFunction(Wam &)
{

}

void Wam::RegisterExternal(QString name, function<void(Wam &)> f)
{
    externalMethods[name] = f;
}

void Wam::query(QString tableName)
{
    if(!factArities.contains(tableName))
    {
        error(QString("Querying unknown fact: %1").arg(tableName));
        return;
    }
    int arity = factArities[tableName];
    QStringList wheres;
    int n = operandStack.count()-1;
    QVector<QVariant> args;
    for(int i=0; i<arity; ++i)
    {
        shared_ptr<Term::Term> arg = operandStack[n-i];
        shared_ptr<Term::Term> groundArg;
        if(ground(arg, groundArg))
        {
            wheres.append(QString("c%1=?").arg(i));
            args.append(dbHelper.termToQVariant(groundArg));
        }
    }
    QString whereClause=(wheres.count()!=0)?QString(" WHERE %1").arg(wheres.join(" AND ")):"";
    QString query = QString("SELECT * FROM %1%2")
            .arg(tableName)
            .arg(whereClause);
    QSqlQuery q;

    if(!dbHelper.find(query, q, args))
    {
        error(QString("Failed to execute query: %1").arg(query));
        return;
    }
    operandStack.push(Term::makeQuery(q));
}

shared_ptr<Term::Term> Wam::resultToTerm(int i, QSqlQuery &q, Term::Tag type)
{
    if(type==Term::TermInt)
    {
        return Term::makeInt(q.value(i).toInt());
    }
    else if(type==Term::TermStr)
    {
        return Term::makeString(q.value(i).toString());
    }
    else if(type==Term::TermSymbol)
    {
        return Term::makeSymbol(q.value(i).toString());
    }
    else
    {
        error(QString("resultToTerm: unhandled term type %1").arg(type));
        return shared_ptr<Term::Term>();
    }
}


void Wam::error(QString s)
{
    errors.append(s);
    done = true;
}


void Wam::OpenDb()
{
    dbHelper.open();
    dbHelper.createTables(facts);
}

void Wam::CloseDb()
{
    dbHelper.close();
}

void Wam::Init()
{
    callStack.clear();
    trail.clear();
    choicePoints.clear();
    solutions.clear();
    operandStack.clear();
    errors.clear();
    newVarCount = 0;
}

void Wam::Finished()
{
}

void Wam::Run(QString main, QMap<QString, shared_ptr<Term::Term> > bindings)
{
    IP = 0;
    currentFrame = 0;
    Frame f;
    f.Ip = -1; // unused
    f.parentFrame = -1;

    f.method = predicates[main];
    if(!f.method)
    {
        errors.append(QString("Wam::Run() cannot find method %1").arg(main));
#ifdef QT_DEBUG
        qDebug() << "Wam::Run() cannot find main method: " << main;
#endif
        return;
    }
    callStack.push(f);

    for(auto i=bindings.begin(); i!=bindings.end();++i)
    {
        callStack[0].Environment[i.key()] = i.value();
    }

    result = false;
    done = false;
    shared_ptr<Term::Term> t1,t2;
    ChoicePoint cp;
    Frame f2;
    while(!done)
    {
        Frame &frame = callStack[currentFrame];
        Instruction &i= frame.method->Instructions[IP++];
        switch(i.opcode)
        {
        case PushV:
            operandStack.push(i.arg);
            break;
        case PushLocal:
            operandStack.push(frame.Environment[i.arg->toString()]);
#if defined(QT_DEBUG) && defined(PL_VERBOSE_DEBUGGING)
            qDebug() << "Pushing " << frame.Environment[i.arg->toString()]->toString() << "\n";
#endif
            break;
        case PopLocal:
            frame.Environment[i.arg->toString()] = operandStack.pop();
            break;
        case NewVar:
            operandStack.push(newVar());
            break;
        case NewObj:
            if(structArities.contains(i.arg->toString()))
            {
                shared_ptr<Term::Compound> s(Term::makeCompound(i.arg->toString()));
                int arity = structArities[i.arg->toString()];
                for(int i=0; i<arity; i++)
                {
                    s->args.append(operandStack.pop());
                }
                operandStack.push(s);
            }
            else
            {
                errors.append(QString("new: Struct %1 not found").arg(i.arg->toString()));
            }
            break;
        case Unify:
            t1 = operandStack.pop();
            t2 = operandStack.pop();
            result = unify(t1, t2);
            if(!result)
            {
                fail();
            }
            break;
        case Call:
            f2 = Frame();
            f2.Ip = IP;
            f2.method = predicates[i.arg->toString()];
            f2.parentFrame = currentFrame;
#if defined(QT_DEBUG) && defined(PL_VERBOSE_DEBUGGING)
            qDebug() << "Calling: " << i.arg->toString()
                 << " from " << callStack[currentFrame].method->name;
#endif
            f2.Environment = QMap<QString, shared_ptr<Term::Term> >();
            callStack.push(f2);
            IP = 0;
            currentFrame = callStack.count()-1;
            break;
        case CallEx:
        {
            function<void(Wam &)> f =
                    externalMethods.value(i.arg->toString(), nothingFunction);
            f(*this);
        }
            break;
        case DbQuery:
            query(i.arg->toString());
            break;
        case DbCheck:
        {
            shared_ptr<Term::DbQuery> tq = dynamic_pointer_cast<Term::DbQuery>(operandStack.top());
            QString table = i.arg->toString();
            int nfields = factArities[table];

            if(!tq)
            {
                error("Expected query object on stack");
            }
            QSqlQuery &q =tq->q;
            if(!q.next())
            {
                operandStack.pop(); // no more query object
                for(int i=0; i<nfields;++i) // no more args
                    operandStack.pop();
                fail();
            }
            else
            {
                // First push a choice point to ourselves
                cp = ChoicePoint();
                cp.trailIndex = trail.count();
                cp.frameIndex = currentFrame;
                cp.cutReg = CutReg;
                cp.continuation = "q";
                choicePoints.push(cp);

                // Now let's try our query
                int n = operandStack.count() -1;
                n--; // skip query object
                for(int i=0; i<nfields; ++i)
                {
                    shared_ptr<Term::Term> t = operandStack[n-i];
                    shared_ptr<Term::Term> t2 = resultToTerm(i, q, factArgTypes[table][i]);
                    if(!t2)
                    {
                        goto DbCheckError;
                    }
                    unify(t, t2);
                }
            }
        }
DbCheckError:
            break;
        case TryMeElse:
            cp = ChoicePoint();
            cp.trailIndex = trail.count();
            cp.frameIndex = currentFrame;
            cp.continuation = i.arg->toString();
            cp.cutReg = CutReg;
            choicePoints.push(cp);
            break;
        case Proceed:
            if(currentFrame==0)
            {
                result = true;
#if defined(QT_DEBUG) && defined(PL_VERBOSE_DEBUGGING)
                qDebug() << dumpTrail();
#endif
                //errors.append(EnvToString(resolveAll(callStack[0].Environment)));
                solutions.append(resolveAll(callStack[0].Environment));
                if(!choicePoints.empty())
                    backtrack();
                else
                {
                    done = true;
                }
            }
            else
            {
                IP = callStack[currentFrame].Ip;
                currentFrame = callStack[currentFrame].parentFrame;
            }
            break;
        case Fail:
            fail();
            break;
        case SaveCR:
            CutReg = choicePoints.count();
            break;
        case Cut:
            while(choicePoints.count() > CutReg)
            {
                choicePoints.pop();
            }
            if(choicePoints.count() >0)
            {
                const ChoicePoint &cp = choicePoints.top();
                while(trail.count() > cp.trailIndex)
                    trail.pop();
            }
            break;
        }
    }
    Finished();
}

void Wam::backtrack()
{
    ChoicePoint cp = choicePoints.pop();
    currentFrame = cp.frameIndex;
    CutReg = cp.cutReg;
    while(trail.count() > cp.trailIndex)
        trail.pop();
#if defined(QT_DEBUG) && defined(PL_VERBOSE_DEBUGGING)
    qDebug() << "Backtrack to: " << cp.continuation;
#endif
    IP = callStack[currentFrame].method->labels[cp.continuation];
}

void Wam::fail()
{
    if(!choicePoints.empty())
        backtrack();
    else
        done = true;
}

bool Wam::ground(shared_ptr<Term::Term> const &t, shared_ptr<Term::Term> &ret)
{
    if(t->ground())
    {
        ret = t;
        return true;
    }
    else if(t->tag == Term::TermVar)
    {
        shared_ptr<Term::Term> r;
        bool b = lookup(t->var(), r);
        if(!b)
            return false;
        if(r->tag==Term::TermVar)
            return false;
        return ground(r,ret);
    }
    if(t->tag == Term::TermCompund)
    {
        shared_ptr<Term::Compound> t1 = dynamic_pointer_cast<Term::Compound>(t);
        shared_ptr<Term::Compound> t2 = Term::makeCompound(t1->functor->toString());
        shared_ptr<Term::Term> arg;
        for(int i=0; i<t1->args.count();++i)
        {
            if(!ground(t1->args[i], arg))
                return false;
            t2->args.append(arg);
        }

        ret = t2;
        return true;
    }
}

void Wam::bind(uint var, shared_ptr<Term::Term> const &val)
{
#if defined(QT_DEBUG) && defined(PL_VERBOSE_DEBUGGING)
    qDebug() << "Unify: binding " << var << " and "
         << val->toString() << endl;
#endif
    shared_ptr<Term::Term> boundVar;
    if(lookup(var, boundVar))
    {
        shared_ptr<Term::Var> bv = dynamic_pointer_cast<Term::Var>(boundVar);
        trail.push(Binding(bv->name, val));
        return;
    }
    trail.push(Binding(var, val));
    return;
}

bool Wam::unifyCompound(shared_ptr<Term::Term> t1, shared_ptr<Term::Term> t2)
{
    if(t2->tag !=Term::TermCompund)
        return false;
    shared_ptr<Term::Compound> c1 = dynamic_pointer_cast<Term::Compound>(t1);
    shared_ptr<Term::Compound> c2 = dynamic_pointer_cast<Term::Compound>(t2);
    if(!c1->functor->equals(c2->functor))
    {
        return false;
    }
    for(int i=0; i<c1->args.count(); ++i)
    {
        if(!unify(c1->args[i], c2->args[i]))
            return false;
    }
    return true;
}

bool Wam::unify(shared_ptr<Term::Term> t1, shared_ptr<Term::Term> t2)
{
    if(t1->tag == Term::TermVar)
    {
        lookup(t1->var(), t1);
    }

    if(t2->tag == Term::TermVar)
    {
        lookup(t2->var(), t2);
    }

    bool t1Ground = t1->ground();
    bool t2Ground = t2->ground();

    //const bool t1Free = !t1Ground, t2Free = !t2Ground;
    const bool t1Var= t1->tag==Term::TermVar,
            t2Var= t2->tag==Term::TermVar;

    if(t1Ground && t2Ground)
    {
        return t1->equals(t2);
    }

    if(t1->tag == Term::TermCompund && !t2Var)
    {
        return unifyCompound(t1, t2);
    }

    if(t2->tag == Term::TermCompund && !t1Var)
    {
        return unifyCompound(t2, t1);
    }

    if(t1Var && !t2Var)
    {
        trail.push(Binding(t1->var(), t2));
        return true;
    }

    if(!t1Var && t2Var)
    {
        trail.push(Binding(t2->var(), t1));
        return true;
    }

    // both free vars
#if defined(QT_DEBUG) && defined(PL_VERBOSE_DEBUGGING)
    qDebug() << "Trail..." << dumpTrail();
#endif
    uint t1s = t1->var();
    uint t2s = t2->var();
    if(t1s < t2s)
    {
        swap(t1s, t2s);
    }
    trail.push(Binding(t1s, t2));
    trail.push(Binding(t2s, t2));

    return true;
}

bool Wam::lookup(uint v,
                 shared_ptr<Term::Term> &ret)
{
    for(auto i=trail.begin(); i!=trail.end(); ++i)
    {
        const Binding &b = *i;
        if(b.var == v)
        {
            const shared_ptr<Term::Term> &v2 = b.val;
            if(v2->tag == Term::TermVar)
            {
                if(v2->var() != v)
                    return lookup(v2->var(), ret);
                else
                {
                    ret = v2;
                    return true;
                }
            }
            else
            {
                ret = b.val;
                return true;
            }
        }
    }
    return false;
}

QString Wam::dumpTrail()
{
    QStringList ret;
    for(auto i=trail.begin(); i!=trail.end(); ++i)
    {
        ret.append(QString("v%1 -> %2").arg((*i).var).arg((*i).val->toString()));
    }
    return QString("Trail: [%1]-").arg(ret.join(", "));
}

QMap<QString, shared_ptr<Term::Term> > Wam::resolveAll
(QMap<QString, shared_ptr<Term::Term> >env)
{
    QMap<QString, shared_ptr<Term::Term> > ret;
    for(auto i=env.begin(); i!=env.end(); ++i)
    {
        shared_ptr<Term::Term> t = i.value();
        shared_ptr<Term::Term> t2;
        if(t->tag == Term::TermVar)
        {
            shared_ptr<Term::Var> vt = dynamic_pointer_cast<Term::Var>(t);
            if(!ground(vt, t2))
            {
#ifdef QT_DEBUG
                qDebug() << "Free variable " << t->toString() << " on goal succcess";
#endif
                continue;
            }
            else
            {
                if(t2)
                    ret[i.key()] = t2;
                else
                {
#ifdef QT_DEBUG
                    qDebug() << "Bug! binding is null";
#endif
                }
            }
        }
    }

    return ret;
}

QString OpcodeToString(OpCode op)
{
    switch(op)
    {
    case PushV: return "PushV";
    case PushLocal: return "PushLocal";
    case PopLocal: return "PopLocal";
    case NewVar: return "NewVar";
    case NewObj: return "NewObj";
    case Unify: return "Unify";
    case Call: return "Call";
    case CallEx: return "CallEx";
    case DbQuery: return "DbQuery";
    case DbCheck: return "DbCheck";
    case TryMeElse: return "TryMeElse";
    case Proceed: return "Proceed";
    case Fail: return "Fail";
    case SaveCR: return "SaveCR";
    case Cut: return "Cut";
    }
    return "<unknown>";
}

QString Instruction::toString()
{
    return QString("[%1 %2]").arg(OpcodeToString(this->opcode)).arg(arg?arg->toString():"");
}

QString Method::toString()
{
    QStringList lst;
    QMap<int, QString> invLabels;
    for(auto i=labels.begin(); i!=labels.end();++i)
    {
        invLabels[i.value()] = i.key();
    }
    for(int i=0; i<Instructions.count(); ++i)
    {
        if(invLabels.contains(i))
        {
            lst.append(invLabels[i]+": ");

        }
        lst.append(Instructions[i].toString());
    }

    return QString("Predicate {%1}").arg(lst.join("; "));
}

QString EnvToString(QMap<QString, shared_ptr<Term::Term> > env)
{
    QStringList lst;
    for(auto i=env.begin(); i!=env.end();++i)
        lst.append(QString("%1=%2").arg(i.key()).arg(i.value()->toString()));

    return QString("[%1]").arg(lst.join(", "));
}
}
