#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sexpressionlexer.h"
#include "sexpression.h"
#include "program.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    SExpressionLexer SExp;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    bool parsePrologCode(Prolog::Program &proggy);
    void parseWam(QString source, QVector<shared_ptr<SExpression> >&sexps, bool verbose=true);
    void runWam(QVector<shared_ptr<SExpression> > &sexps);
private slots:
    void on_actionRun_triggered();

    void on_actionParse_prolog_triggered();

    void on_actionCompile_Prolog_triggered();

    void on_actionParse_WAM_triggered();

    void on_actionRun_Prolog_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
