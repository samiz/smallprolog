#include "prologengine.h"
#include "parsers/prologparser.h"
#include "parsers/sexpressionparser.h"

#include "wam/builtins.h"

PrologEngine::PrologEngine()
{
    wam.RegisterExternal("sqrt", Prolog::sqrt);
    wam.RegisterExternal("+", Prolog::plus);
    wam.RegisterExternal("-", Prolog::minus);
    wam.RegisterExternal("/", Prolog::div);
    wam.RegisterExternal("*", Prolog::mul);
    wam.RegisterExternal("++", Prolog::concat);

    wam.RegisterExternal(">", Prolog::gt);
    wam.RegisterExternal("<", Prolog::lt);
    wam.RegisterExternal(">=", Prolog::ge);
    wam.RegisterExternal("<=", Prolog::le);
    wam.RegisterExternal("<>", Prolog::ne);

    wam.RegisterExternal("assert", Prolog::assert);
    wam.RegisterExternal("delete", Prolog::delete_);
}

void PrologEngine::initProgram(Prolog::Program &proggy)
{
    proggy.externalMethods.insert("sqrt");
    proggy.externalMethods.insert("+");
    proggy.externalMethods.insert("-");
    proggy.externalMethods.insert("/");
    proggy.externalMethods.insert("*");
    proggy.externalMethods.insert("++");
    proggy.externalMethods.insert("<");
    proggy.externalMethods.insert(">");
    proggy.externalMethods.insert("<=");
    proggy.externalMethods.insert(">=");
    proggy.externalMethods.insert("<>");
    proggy.externalMethods.insert("assert");
    proggy.externalMethods.insert("delete");


    proggy.addStruct("pair", 2);
    proggy.addStruct("nil", 0);
}

void PrologEngine::prepareDb()
{
    wam.OpenDb();
}

bool PrologEngine::load(QString code)
{
    lexer.lexer.init(code);
    lexer.lexer.skipTokens.insert(Prolog::Spacing);
    lexer.lexer.tokenize();

    errors << lexer.lexer.errors;
    if(!errors.empty())
        return false;

    Prolog::Program program;
    initProgram(program);
    Prolog::PrologParser parser(lexer.lexer.acceptedTokens, program);
    parser.parse();

    errors << parser.errors;
    if(!errors.empty())
        return false;

    Prolog::PrologCompiler compiler(program);
    compiler.compile();

    errors << compiler.errors;
    if(!errors.empty())
        return false;

    SExp.lexer.init(compiler.getOutput());
    SExp.lexer.skipTokens.insert(SExp::Spacing);
    SExp.lexer.tokenize();

    errors << SExp.lexer.errors;
    if(!errors.empty())
        return false;

    SExpressionParser sparser(SExp.lexer.acceptedTokens);
    QVector<shared_ptr<SExpression> >sexps = sparser.parse();

    errors << sparser.errors;
    if(!errors.empty())
        return false;

    wam.Load(sexps);

    errors << wam.errors;
    if(!errors.empty())
        return false;

    return true;
}

QMap<QString,QVariant> convertMap(QMap<QString, shared_ptr<Term::Term> > m)
{
    QMap<QString,QVariant> ret;
    for(auto i=m.begin(); i!=m.end();++i)
    {
        ret[i.key()] = i.value()->toVariant();
    }
    return ret;
}

void PrologEngine::call(QString proc,
                        std::function<void (QMap<QString, QVariant>)> callBack)
{
    QVector<QVariant> v;
    call(proc, v, callBack);
}

void PrologEngine::call(QString proc,
                        QVariant arg0,
                        std::function<void (QMap<QString, QVariant>)> callBack)
{
    QVector<QVariant> v;
    v.append(arg0);
    call(proc, v, callBack);
}

void PrologEngine::call(QString proc, QVariant arg0, QVariant arg1, std::function<void (QMap<QString, QVariant>)> callBack)
{
    QVector<QVariant> v;
    v.append(arg0);
    v.append(arg1);
    call(proc, v, callBack);
}

void PrologEngine::call(QString proc,
                        QVariant arg0,
                        QVariant arg1,
                        QVariant arg2,
                        std::function<void (QMap<QString, QVariant>)> callBack)
{
    QVector<QVariant> v;
    v.append(arg0);
    v.append(arg1);
    v.append(arg2);
    call(proc, v, callBack);
}

void PrologEngine::call(QString proc,
                        QVariant arg0,
                        QVariant arg1,
                        QVariant arg2,
                        QVariant arg3,
                        std::function<void (QMap<QString, QVariant>)> callBack)
{
    QVector<QVariant> v;
    v.append(arg0);
    v.append(arg1);
    v.append(arg2);
    v.append(arg3);
    call(proc, v, callBack);
}

void PrologEngine::call(QString proc)
{
    QVector<QVariant> v;
    call(proc, v, [](QMap<QString,QVariant> m){});
}

void PrologEngine::call(QString proc, QVariant arg0)
{
    QVector<QVariant> v;
    v.append(arg0);
    call(proc, v, [](QMap<QString,QVariant> m){});
}

void PrologEngine::call(QString proc, QVariant arg0, QVariant arg1)
{
    QVector<QVariant> v;
    v.append(arg0);
    v.append(arg1);
    call(proc, v, [](QMap<QString,QVariant> m){});
}

void PrologEngine::call(QString proc, QVariant arg0, QVariant arg1, QVariant arg2)
{
    QVector<QVariant> v;
    v.append(arg0);
    v.append(arg1);
    v.append(arg2);

    call(proc, v, [](QMap<QString,QVariant> m){});
}

void PrologEngine::call(QString proc, QVariant arg0, QVariant arg1, QVariant arg2, QVariant arg3)
{
    QVector<QVariant> v;
    v.append(arg0);
    v.append(arg1);
    v.append(arg2);
    v.append(arg3);

    call(proc, v, [](QMap<QString,QVariant> m){});
}

void PrologEngine::call(QString proc, const QVector<QVariant> &args)
{
    call(proc, args, [](QMap<QString,QVariant> m){});
}

shared_ptr<Term::Term> termFromVariant(QVariant v)
{
    if(v.type() == QVariant::Int)
        return Term::makeInt(v.toInt());
    else
        return Term::makeString(v.toString());
}

void PrologEngine::call(QString proc,
                        const QVector<QVariant> &args,
                        std::function<void (QMap<QString, QVariant>)> callBack)
{
    wam.Init();
    QMap<QString, shared_ptr<Term::Term> > bindings;
    for(int i=args.count()-1; i>=0; --i)
    {
        if(args[i].type() == QVariant::String && args[i].toString().startsWith("_"))
        {
            QString var = args[i].toString().mid(1);
            shared_ptr<Term::Var> v = wam.newVar();
            wam.operandStack.push(v);
            bindings[var] = v;
        }
        else
        {
            wam.operandStack.push(termFromVariant(args[i]));
        }
    }
    wam.Run(proc, bindings);
    errors << wam.errors;
    if(!errors.empty())
        return;
    for(auto i=wam.solutions.begin(); i!=wam.solutions.end();++i)
    {
        QMap<QString,QVariant> m = convertMap(*i);
        callBack(m);
    }
}
