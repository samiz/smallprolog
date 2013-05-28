#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "./parsers/sexpressionparser.h"
#include "./lexers/prologlexer.h"
#include "./parsers/prologparser.h"
#include "./compiler/prologcompiler.h"
#include "./wam/builtins.h"
#include "prologengine.h"

#include <sys/time.h>

PrologConsole::PrologConsole(QWidget *parent, Wam::Wam *existingWam) :
    QMainWindow(parent),
    ui(new Ui::PrologConsole)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->splitter);
    this->setWindowTitle(QString::fromStdWString(L"小さい Prolog"));

    if(existingWam)
    {
        this->wam = existingWam;
        ui->txtCode->append("clauses\n\nmain():-\n\n.\n");
    }
    else
    {
        prepareWam();
        this->setWindowState(Qt::WindowMaximized);
    }
}

PrologConsole::~PrologConsole()
{
    delete ui;
}

QString humanize(QString s)
{
    return s.replace("\n", "\\n").replace("\t","\\t").replace("\r", "\\r");
}

void PrologConsole::prepareWam()
{
    wam = new Wam::Wam();
    wam->RegisterExternal("sqrt", Prolog::sqrt);
    wam->RegisterExternal("+", Prolog::plus);
    wam->RegisterExternal("-", Prolog::minus);
    wam->RegisterExternal("/", Prolog::div);
    wam->RegisterExternal("*", Prolog::mul);
    wam->RegisterExternal("++", Prolog::concat);

    wam->RegisterExternal(">", Prolog::gt);
    wam->RegisterExternal("<", Prolog::lt);
    wam->RegisterExternal(">=", Prolog::ge);
    wam->RegisterExternal("<=", Prolog::le);
    wam->RegisterExternal("<>", Prolog::ne);

    wam->RegisterExternal("assert", Prolog::assert);
    wam->RegisterExternal("delete", Prolog::delete_);
    wam->RegisterExternal("write", Prolog::write);
    wam->RegisterExternal("dumpTrail", Prolog::dumpTrail);

}

void PrologConsole::on_actionRun_triggered()
{
   QString source = ui->txtCode->toPlainText();
   QVector<shared_ptr<SExpression> >sexps;
   parseWam(source, sexps);
   runWam(sexps);
}

// In microseconds
namespace Timing
{
long get_time()
{
    /*
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    */
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
}

void PrologConsole::runWam(QVector<shared_ptr<SExpression> > &sexps)
{
    wam->Load(sexps);
    wam->Init();
    long a,b;
    a = Timing::get_time();
    wam->Run("main");
    b = Timing::get_time();

    if(wam->errors.count()> 0)
        ui->txtMessages->append("______________________");

    for(int i=0; i<wam->errors.count(); ++i)
    {
        ui->txtMessages->append(wam->errors[i]);
    }

    for(int i=0; i<wam->solutions.count(); ++i)
    {
        ui->txtMessages->append(EnvToString(wam->solutions[i]));
    }
    ui->txtMessages->append(QString("Interpreter executed in %1 ms").arg((b-a)/1000));


}

bool PrologConsole::parsePrologCode(Prolog::Program &proggy)
{
    bool success = true;
    QString source = ui->txtCode->toPlainText();
    Prolog::PrologLexer lexer;
    lexer.lexer.init(source);
    lexer.lexer.skipTokens.insert(Prolog::Spacing);
    lexer.lexer.tokenize();
    ui->txtMessages->clear();
    for(int i=0; i<lexer.lexer.acceptedTokens.count(); ++i)
    {
        shared_ptr<Token> tok = lexer.lexer.acceptedTokens[i];
        ui->txtMessages->append(tok->toString());
        ui->txtMessages->append("\n");
    }
    if(lexer.lexer.errors.count() > 0)
    {
        ui->txtMessages->append("______________________");
        success = false;
    }
    for(int i=0; i<lexer.lexer.errors.count(); ++i)
    {
        ui->txtMessages->append(humanize(lexer.lexer.errors[i]));
    }

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
    proggy.externalMethods.insert("write");
    proggy.externalMethods.insert("dumpTrail");


    proggy.addStruct("pair", 2);
    proggy.addStruct("nil", 0);
    Prolog::PrologParser parser(lexer.lexer.acceptedTokens, proggy);
    try
    {
        parser.parse();
    }
    catch(ParserException)
    {

    }
    if(parser.errors.count() > 0)
    {
        ui->txtMessages->append("______________________");
        success = false;
    }
    for(int i=0; i<parser.errors.count(); ++i)
    {
        ui->txtMessages->append(humanize(parser.errors[i]));
    }

    for(auto i=proggy.clauses.begin(); i!= proggy.clauses.end(); ++i)
    {
        for(auto j=i.value().begin(); j!=i.value().end(); ++j)
        {
            ui->txtMessages->append((*j)->toString());
        }
    }
    return success;
}

void PrologConsole::on_actionParse_prolog_triggered()
{
    Prolog::Program proggy;
    parsePrologCode(proggy);
}

void PrologConsole::on_actionCompile_Prolog_triggered()
{
    Prolog::Program proggy;
    parsePrologCode(proggy);

    Prolog::PrologCompiler comp(proggy);
    comp.compile();
    ui->txtMessages->append(comp.errors.join("\n"));
    ui->txtMessages->append(comp.getOutput());
}

void PrologConsole::parseWam(QString source, QVector<shared_ptr<SExpression> >&sexps, bool verbose)
{
    SExp.lexer.init(source);
    SExp.lexer.skipTokens.insert(SExp::Spacing);
    SExp.lexer.tokenize();
    ui->txtMessages->clear();
    if(verbose)
    {
        for(int i=0; i<SExp.lexer.acceptedTokens.count(); ++i)
        {
            shared_ptr<Token> tok = SExp.lexer.acceptedTokens[i];
            ui->txtMessages->append(tok->toString());
            ui->txtMessages->append("\n");
        }
    }
    if(SExp.lexer.errors.count() > 0)
        ui->txtMessages->append("______________________");

    for(int i=0; i<SExp.lexer.errors.count(); ++i)
    {
        ui->txtMessages->append(humanize(SExp.lexer.errors[i]));
    }

    SExpressionParser parser(SExp.lexer.acceptedTokens);
    sexps = parser.parse();
    if(parser.errors.count() >0)
        ui->txtMessages->append("______________________");

    for(int i=0; i<parser.errors.count(); ++i)
    {
        ui->txtMessages->append(humanize(parser.errors[i]));
    }

    if(verbose)
    {
        ui->txtMessages->append("______________________");

        for(int i=0; i<sexps.count(); ++i)
        {
            shared_ptr<SExpression> sexp = sexps[i];
            ui->txtMessages->append(sexp->toString());
        }
    }
}

void PrologConsole::on_actionParse_WAM_triggered()
{
    QVector<shared_ptr<SExpression> > sexps;
    QString source = ui->txtCode->toPlainText();
    parseWam(source, sexps);
}

void PrologConsole::on_actionRun_Prolog_triggered()
{
    bool success;
    Prolog::Program proggy;
    success = parsePrologCode(proggy);
    if(!success)
        return;
    Prolog::PrologCompiler comp(proggy);

    comp.compile();
    ui->txtMessages->append(comp.errors.join("\n"));
    QString wam = comp.getOutput();
    QVector<shared_ptr<SExpression> > sexps;
    parseWam(wam, sexps, false);
    runWam(sexps);
}
