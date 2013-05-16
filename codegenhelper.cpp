/**************************************************************************
**   The Kalimat programming language
**   Copyright (C) 2010 Mohamed Samy Ali - samy2004@gmail.com
**   This project is released under the Apache License version 2.0
**   as described in the included license.txt file
**************************************************************************/

#include "codegenhelper.h"

CodeGenerator::CodeGenerator()
{
    Init();
}

void CodeGenerator::Init()
{
    Output.clear();
    uniqueLabelCount = 0;
    uniqueStringConstantCount = 0;
    uniqueVariableCount = 0;
    uniqueLambdaCount = 0;
}

QString CodeGenerator::getOutput()
{
    return Output.join("");
}

QString CodeGenerator::makeStringConstant(QString str)
{
    if(!StringConstants.contains(str))
    {
        QString symRef = QString("%%string%1").arg(uniqueStringConstantCount++);
        StringConstants[str] = symRef;
    }
    return StringConstants[str];
}

QString CodeGenerator::uniqueLabel()
{
    return QString("%%label%1").arg(uniqueLabelCount++);
}

QString CodeGenerator::uniqueVariable()
{
    return QString("%%var%1").arg(uniqueVariableCount++);
}

QString CodeGenerator::uniqueLambda()
{
    return QString("%%lambda%1").arg(uniqueLambdaCount++);
}

void CodeGenerator::gen(QString str)
{
    Output.append(str+"\n");
}

void CodeGenerator::gen(QString str, QString arg)
{
    genFmt(str, arg);
}

void CodeGenerator::gen(QString str, QString arg, int i)
{
    gen(QString(str).arg(arg).arg(i));
}


void CodeGenerator::gen(QString str, int i)
{
    gen(QString(str).arg(i));
}

void CodeGenerator::gen(QString str, double d)
{
    gen(QString(str).arg(d));
}

void CodeGenerator::genFmt(QString fmtStr)
{
    gen(fmtStr);
}

void CodeGenerator::genFmt(QString fmtStr, QString str)
{
    gen(fmtStr.arg(str));
}

void CodeGenerator::genFmt(QString fmtStr, QString str0, QString str1)
{
    gen(fmtStr.arg(str0).arg(str1));
}

void CodeGenerator::genWithMetaData(int metaData, QString str)
{
    gen(str+ "@"+ QString("%1").arg(metaData));
}
