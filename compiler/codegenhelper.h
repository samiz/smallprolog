/**************************************************************************
** The Kalimat programming language
**   Copyright (C) 2010 Mohamed Samy Ali - samy2004@gmail.com
**   This project is released under the Apache License version 2.0
**   as described in the included license.txt file
**************************************************************************/

#ifndef PL_CODEGENHELPER_H
#define PL_CODEGENHELPER_H

#include <QString>
#include <QStringList>
#include <QMap>

namespace Prolog
{
class CodeGenerator
{
    QStringList Output;
    int uniqueLabelCount;
    int uniqueStringConstantCount;
    int uniqueVariableCount;
    int uniqueLambdaCount;

public:
    QMap<QString, QString> StringConstants;

    CodeGenerator();
    void Init();
    QString getOutput();
public:
    int nextLineNum() { return Output.count(); }
public:
    void gen(QString str);
    void gen(QString str, int i);
    void gen(QString str, double d);
    void gen(QString str, QString arg);
    void gen(QString str, QString arg, QString arg2);
    void gen(QString str, QString arg, int i);
    void genFmt(QString fmtStr);
    void genFmt(QString fmtStr, QString str);
    void genFmt(QString fmtStr, QString str0, QString str1);
    void genWithMetaData(int metaData, QString str);


    QString uniqueLabel();
    QString uniqueVariable();
    QString uniqueLambda();
    QString makeStringConstant(QString str);
};
}
#endif // CODEGENHELPER_H
