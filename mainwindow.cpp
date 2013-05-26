#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "./parsers/sexpressionparser.h"
#include "./lexers/prologlexer.h"
#include "./parsers/prologparser.h"
#include "./compiler/prologcompiler.h"
#include "./wam/wam.h"
#include "./wam/builtins.h"
#include "prologengine.h"

#include <sys/time.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->splitter);
    this->setWindowState(Qt::WindowMaximized);
    this->setWindowTitle(QString::fromStdWString(L"小さい Prolog"));
    PrologEngine prolog;
    prolog.load("facts\nprocDef(string)\n");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString humanize(QString s)
{
    return s.replace("\n", "\\n").replace("\t","\\t").replace("\r", "\\r");
}

void MainWindow::on_actionRun_triggered()
{
   QString source = ui->txtCode->toPlainText();
   QVector<shared_ptr<SExpression> >sexps;
   parseWam(source, sexps);
   runWam(sexps);
}

// In microseconds
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

void MainWindow::runWam(QVector<shared_ptr<SExpression> > &sexps)
{
    Wam::Wam wam;
    wam.Load(sexps);
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

    wam.Init();
    long a,b;
    a = get_time();
    wam.Run("main");
    b = get_time();

    if(wam.errors.count()> 0)
        ui->txtMessages->append("______________________");

    for(int i=0; i<wam.errors.count(); ++i)
    {
        ui->txtMessages->append(wam.errors[i]);
    }
    ui->txtMessages->append(QString("Interpreter executed in %1 ms").arg((b-a)/1000));
}

bool MainWindow::parsePrologCode(Prolog::Program &proggy)
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

void MainWindow::on_actionParse_prolog_triggered()
{
    Prolog::Program proggy;
    parsePrologCode(proggy);
}

void MainWindow::on_actionCompile_Prolog_triggered()
{
    Prolog::Program proggy;
    parsePrologCode(proggy);

    Prolog::PrologCompiler comp(proggy);
    comp.compile();
    ui->txtMessages->append(comp.errors.join("\n"));
    ui->txtMessages->append(comp.getOutput());
}

void MainWindow::parseWam(QString source, QVector<shared_ptr<SExpression> >&sexps, bool verbose)
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

void MainWindow::on_actionParse_WAM_triggered()
{
    QVector<shared_ptr<SExpression> > sexps;
    QString source = ui->txtCode->toPlainText();
    parseWam(source, sexps);
}

void MainWindow::on_actionRun_Prolog_triggered()
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
