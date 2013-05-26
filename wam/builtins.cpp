#include "builtins.h"

namespace Prolog
{

inline shared_ptr<Term::Term> PopTerm(Wam::Wam &wam, const QString &, int)
{
    return wam.operandStack.pop();
}

inline bool PopGround(Wam::Wam &vm, const QString &fname, int argn, shared_ptr<Term::Term> &ret)
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

inline bool PopCompound(Wam::Wam &vm, const QString &fname, int argn, shared_ptr<Term::Compound> &ret)
{
    shared_ptr<Term::Term> a = vm.operandStack.pop();
    shared_ptr<Term::Compound> ac = dynamic_pointer_cast<Term::Compound>(a);
    if(!ac)
    {
        vm.error(QString("%1: argument %2 not a compund term").arg(fname).arg(argn));
        return false;
    }
    ret = ac;
    return true;
}

inline bool PopInt(Wam::Wam &vm, const QString &fname, int argn, int &ret)
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

inline bool PopStr(Wam::Wam &vm, const QString &fname, int argn, QString &ret)
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

void sqrt(Wam::Wam &vm)
{
    int ii;
    if(!PopInt(vm, "sqrt", 1, ii))
        return;

    shared_ptr<Term::Term> b = PopTerm(vm, "sqrt", 2);

    ii = ::sqrt(ii);
    shared_ptr<Term::Int> result = make_shared<Term::Int>(ii);
    vm.unify(result, b);
}

void plus(Wam::Wam &vm)
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

void minus(Wam::Wam &vm)
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

void mul(Wam::Wam &vm)
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

void div(Wam::Wam &vm)
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

void concat(Wam::Wam &vm)
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

void lt(Wam::Wam &vm)
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

void gt(Wam::Wam &vm)
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

void le(Wam::Wam &vm)
{
    shared_ptr<Term::Term> a,b;
    if(!(PopGround(vm,"<=",1, a)
         && PopGround(vm,"<=", 2, b)))
    {
        return;
    }
    if(!(a->lt(b) || a->equals(b)))
    {
        vm.fail();
    }
}

void ge(Wam::Wam &vm)
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

void ne(Wam::Wam &vm)
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

void assert(Wam::Wam &vm)
{
    shared_ptr<Term::Term> a;
    if(!PopGround(vm,"assert",1, a))
    {
        return;
    }
    if(a->tag != Term::TermCompund)
    {
        vm.error(QString("assert: expected compound value, given %1").arg(a->toString()));
        return;
    }
    vm.dbHelper.assert(dynamic_pointer_cast<Term::Compound>(a));
}

void delete_(Wam::Wam &vm)
{
    shared_ptr<Term::Compound> a;
    if(!PopCompound(vm,"delete",1, a))
    {
        return;
    }
    if(a->tag != Term::TermCompund)
    {
        vm.error(QString("assert: expected compound value, given %1").arg(a->toString()));
        return;
    }
    vm.dbHelper.delete_(a, vm);
}

}
