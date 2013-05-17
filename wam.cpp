#include "wam.h"
#include <QDebug>

void Wam::Load(QVector<shared_ptr<SExpression> > code)
{
    for(auto i=code.begin(); i!=code.end(); ++i)
    {
        shared_ptr<SExpression> expr = *i;
        shared_ptr<List> rest;
        QString predName, structName;
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
            qDebug() << "Loaded: " << method->toString();
        }
        else if(expr->match("struct", structName, arity, rest))
        {
            structArities[structName] = arity;
        }
        else
        {
            errors.append(QString("Invalid format: %1").arg(expr->toString()));
        }
    }
}

void Wam::processInstruction(shared_ptr<SExpression> inst, shared_ptr<Method> method, int &count)
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
            processInstruction(tail, method, count);
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
    else if(inst->match("pushv", tail) && tail->matchLstStr(sval))
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
    else if(inst->match("try_me_else", sval))
    {
        method->Instructions.append(Instruction(TryMeElse, Term::makeSymbol(sval)));
    }
    else if(inst->matchLst("proceed"))
    {
        method->Instructions.append(Instruction(Proceed));
    }
    else
    {
        errors.append(QString("Invalid instruction format: %1").arg(inst->toString()));
    }
}

void Wam::Init()
{
    callStack.clear();
    trail.clear();
    choicePoints.clear();
    solutions.clear();
    operandStack.clear();
}

void Wam::Run(QString main)
{
    newVarCount = 0;
    IP = 0;
    currentFrame = 0;
    Frame f;
    f.Ip = -1; // unused
    f.parentFrame = -1;
    f.method = predicates[main];
    callStack.push(f);

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
            qDebug() << "Pushing " << frame.Environment[i.arg->toString()]->toString() << "\n";
            break;
        case PopLocal:
            frame.Environment[i.arg->toString()] = operandStack.pop();
            break;
        case NewVar:
            operandStack.push(Term::makeVar(QString("v%1").arg(newVarCount++)));
            break;
        case NewObj:
            if(structArities.contains(i.arg->toString()))
            {
                shared_ptr<Term::Compound> s(new Term::Compound());
                s->functor = dynamic_pointer_cast<Term::Atom>(i.arg);
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
            qDebug() << "Calling: " << i.arg->toString()
                 << " from " << callStack[currentFrame].method->name;
            f2.Environment = QMap<QString, shared_ptr<Term::Term> >();
            callStack.push(f2);
            IP = 0;
            currentFrame = callStack.count()-1;
            break;
        case TryMeElse:
            cp = ChoicePoint();
            cp.trailIndex = trail.count();
            cp.frameIndex = currentFrame;
            cp.continuation = i.arg->toString();
            choicePoints.push(cp);
            break;
        case Proceed:
            if(currentFrame==0)
            {
                result = true;
                qDebug() << dumpTrail();
                errors.append(EnvToString(resolveAll(callStack[0].Environment)));
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
        }
    }
}

void Wam::backtrack()
{
    ChoicePoint cp = choicePoints.pop();
    currentFrame = cp.frameIndex;
    while(trail.count() > cp.trailIndex)
        trail.pop();
    qDebug() << "Backtrack to: " << cp.continuation;
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
        int i;
        shared_ptr<Term::Term> r;
        bool b = lookup(t->toString(), r, i);
        if(!b)
            return false;
        if(r->tag==Term::TermVar)
            return false;
        return ground(r,ret);
    }
    if(t->tag == Term::TermCompund)
    {
        shared_ptr<Term::Compound> t1 = dynamic_pointer_cast<Term::Compound>(t);
        shared_ptr<Term::Compound> t2 = shared_ptr<Term::Compound>(new Term::Compound());
        t2->functor = t1->functor;
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

void Wam::bind(QString var, shared_ptr<Term::Term> const &val)
{
    qDebug() << "Unify: binding " << var << " and "
         << val->toString() << endl;
    shared_ptr<Term::Term> boundVar;
    int i;
    if(lookup(var, boundVar, i))
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
    int t1Bind, t2Bind;

    if(t1->tag == Term::TermVar)
    {
        lookup(t1->toString(), t1, t1Bind);
    }

    if(t2->tag == Term::TermVar)
    {
        lookup(t2->toString(), t2, t2Bind);
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
        trail.push(Binding(t1->toString(), t2));
        return true;
    }

    if(!t1Var && t2Var)
    {
        trail.push(Binding(t2->toString(), t1));
        return true;
    }

    // both free vars
    qDebug() << "Trail..." << dumpTrail();
    QString t1s = t1->toString();
    QString t2s = t2->toString();
    if(t1s < t2s)
    {
        swap(t1s, t2s);
    }
    trail.push(Binding(t1s, t2));
    trail.push(Binding(t2s, t2));

    return true;
}

bool Wam::lookup(QString v,
                 shared_ptr<Term::Term> &ret,
                 int &bindIndex)
{
    bindIndex = -1;
    for(int i=trail.count()-1; i>=0; --i)
    {
        Binding &b = trail[i];
        if(b.var == v)
        {
            bindIndex = i;
            if(b.val->tag == Term::TermVar)
            {
                shared_ptr<Term::Var> v2 = dynamic_pointer_cast<Term::Var>(b.val);
                if(v2->toString() != v)
                    return lookup(v2->name, ret, bindIndex);
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
    for(int i=trail.count()-1; i>=0; i--)
    {
        ret.append(QString("%1 -> %2").arg(trail[i].var).arg(trail[i].val->toString()));
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
                qDebug() << "Free variable " << t->toString() << " on goal succcess";
                continue;
            }
            else
            {
                if(t2)
                    ret[i.key()] = t2;
                else
                {
                    qDebug() << "Bug! binding is null";
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
    case TryMeElse: return "TryMeElse";
    case Proceed: return "Proceed";
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
