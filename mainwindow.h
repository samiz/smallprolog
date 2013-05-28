#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "./lexers/sexpressionlexer.h"
#include "./data/sexpression.h"
#include "./data/program.h"
#include "./wam/wam.h"

namespace Ui {
class PrologConsole;
}

class PrologConsole : public QMainWindow
{
    Q_OBJECT
    
private:
    SExpressionLexer SExp;
    Wam::Wam *wam;
public:
    explicit PrologConsole(QWidget *parent = 0, Wam::Wam *existingWam = NULL);
    ~PrologConsole();
    
    bool parsePrologCode(Prolog::Program &proggy);
    void parseWam(QString source, QVector<shared_ptr<SExpression> >&sexps, bool verbose=true);
    void runWam(QVector<shared_ptr<SExpression> > &sexps);
    void prepareWam();
private slots:
    void on_actionRun_triggered();

    void on_actionParse_prolog_triggered();

    void on_actionCompile_Prolog_triggered();

    void on_actionParse_WAM_triggered();

    void on_actionRun_Prolog_triggered();

private:
    Ui::PrologConsole *ui;
};

#endif // MAINWINDOW_H
