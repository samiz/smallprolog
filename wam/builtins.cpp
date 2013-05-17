#include "builtins.h"

namespace Prolog
{

inline shared_ptr<Term::Term> PopTerm(Wam &wam, const QString &, int)
{
    return wam.operandStack.pop();
}

inline bool PopGround(Wam &vm, const QString &fname, int argn, shared_ptr<Term::Term> &ret)
{
    shared_ptr<Term::Term> a = vm.operandStack.pop();
    shared_ptr<Term::Term> ag;
    if(!vm.ground(a, ag))
    {
        vm.error(QString("%1: argument %2 not ground term").arg(fname).arg(argn));
        return false;
    }
    ret = ag;
    return true;
}

inline bool PopInt(Wam &vm, const QString &fname, int argn, int &ret)
{
    shared_ptr<Term::Term> a = vm.operandStack.pop();
    shared_ptr<Term::Term> ag;
    if(!vm.ground(a, ag))
    {
        vm.error(QString("%1: argument %2 not ground term").arg(fname).arg(argn));
        return false;
    }
    if(!(ag->tag == Term::TermInt))
    {
        vm.error(QString("%1: argument %2 not a number").arg(fname).arg(argn));
        return false;
    }
    shared_ptr<Term::Int> i = dynamic_pointer_cast<Term::Int>(ag);
    ret = i->value;
    return true;
}

inline bool PopStr(Wam &vm, const QString &fname, int argn, QString &ret)
{
    shared_ptr<Term::Term> a = vm.operandStack.pop();
    shared_ptr<Term::Term> ag;
    if(!vm.ground(a, ag))
    {
        vm.error(QString("%1: argument %2 not ground term").arg(fname).arg(argn));
        return false;
    }
    if(!(ag->tag == Term::TermStr))
    {
        vm.error(QString("%1: argument %2 not a string").arg(fname).arg(argn));
        return false;
    }
    shared_ptr<Term::String> i = dynamic_pointer_cast<Term::String>(ag);
    ret = i->value;
    return true;
}

void sqrt(Wam &vm)
{
    int ii;
    if(!PopInt(vm, "sqrt", 1, ii))
        return;

    shared_ptr<Term::Term> b = PopTerm(vm, "sqrt", 2);

    ii = ::sqrt(ii);
    shared_ptr<Term::Int> result = make_shared<Term::Int>(ii);
    vm.unify(result, b);
}

void plus(Wam &vm)
{
    int a,b;
    if(!(PopInt(vm, "+", 1, a)
        && PopInt(vm, "+", 2,b)))
    {
        return;
    }

    shared_ptr<Term::Term> c = PopTerm(vm,"+",3);

    a+=b;
    shared_ptr<Term::Int> result = make_shared<Term::Int>(a);
    vm.unify(result, c);
}

void minus(Wam &vm)
{
    int a,b;
    if(!(PopInt(vm, "-", 1, a)
        && PopInt(vm, "-", 2,b)))
    {
        return;
    }
    shared_ptr<Term::Term> c = PopTerm(vm,"-",3);

    a-=b;
    shared_ptr<Term::Int> result = make_shared<Term::Int>(a);
    vm.unify(result, c);
}

void mul(Wam &vm)
{
    int a,b;
    if(!(PopInt(vm, "*", 1, a)
        && PopInt(vm, "*", 2,b)))
    {
        return;
    }
    shared_ptr<Term::Term> c = PopTerm(vm,"*",3);

    a*=b;
    shared_ptr<Term::Int> result = make_shared<Term::Int>(a);
    vm.unify(result, c);
}

void div(Wam &vm)
{
    int a,b;
    if(!(PopInt(vm, "/", 1, a)
        && PopInt(vm, "/", 2,b)))
    {
        return;
    }
    shared_ptr<Term::Term> c = PopTerm(vm,"/",3);

    a/=b;
    shared_ptr<Term::Int> result = make_shared<Term::Int>(a);
    vm.unify(result, c);
}

void concat(Wam &vm)
{
    QString a,b;
    if(!(PopStr(vm, "++", 1, a)
        && PopStr(vm, "++", 2,b)))
    {
        return;
    }
    shared_ptr<Term::Term> c = PopTerm(vm,"++",3);

    QString r= a+b;
    shared_ptr<Term::String> result = make_shared<Term::String>(r);
    vm.unify(result, c);
}

void lt(Wam &vm)
{
    shared_ptr<Term::Term> a,b;
    if(!(PopGround(vm,"<",1, a)
         && PopGround(vm,"<", 2, b)))
    {
        return;
    }
    if(!(a->lt(b)))
        vm.fail();
}

void gt(Wam &vm)
{
    shared_ptr<Term::Term> a,b;
    if(!(PopGround(vm,">",1, a)
         && PopGround(vm,">", 2, b)))
    {
        return;
    }
    if(!atomic(a->tag) || a->tag!=b->tag || a->lt(b) || a->equals(b))
    {
        vm.fail();
    }
}

void le(Wam &vm)
{
    shared_ptr<Term::Term> a,b;
    if(!(PopGround(vm,"<=",1, a)
         && PopGround(vm,"<=", 2, b)))
    {
        return;
    }
    if(!a->lt(b) || !a->equals(b))
    {
        vm.fail();
    }
}

void ge(Wam &vm)
{
    shared_ptr<Term::Term> a,b;
    if(!(PopGround(vm,">=",1, a)
         && PopGround(vm,">=", 2, b)))
    {
        return;
    }
    if(!atomic(a->tag) || a->tag !=b->tag || a->lt(b))
    {
        vm.fail();
    }
}

void ne(Wam &vm)
{
    shared_ptr<Term::Term> a,b;
    if(!(PopGround(vm,"<>",1, a)
         && PopGround(vm,"<>", 2, b)))
    {
        return;
    }
    if(!a->equals(b))
    {
        vm.fail();
    }
}



}
