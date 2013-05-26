#-------------------------------------------------
#
# Project created by QtCreator 2013-05-06T05:09:57
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=gnu++0x -Wno-unused-parameter
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmallProlog
#TEMPLATE = app
TEMPLATE = lib
CONFIG += staticlib


SOURCES += main.cpp\
        mainwindow.cpp \
    lexers/lexer.cpp \
    lexers/regexp.cpp \
    lexers/buffer.cpp \
    lexers/token.cpp \
    lexers/sexpressionlexer.cpp \
    lexers/lexererror.cpp \
    lexers/strbuffer.cpp \
    lexers/linetracker.cpp \
    data/sexpression.cpp \
    parsers/sexpressionparser.cpp \
    data/program.cpp \
    wam/wam.cpp \
    data/terms.cpp \
    lexers/prologlexer.cpp \
    parsers/parser.cpp \
    parsers/prologparser.cpp \
    data/ast.cpp \
    parsers/parsecombinators.cpp \
    parsers/parseresult.cpp \
    compiler/prologcompiler.cpp \
    compiler/codegenhelper.cpp \
    wam/builtins.cpp \
    wam/stack.cpp \
    wam/binding.cpp \
    dbhelper.cpp \
    prologengine.cpp

HEADERS  += mainwindow.h \
    lexers/lexer.h \
    lexers/regexp.h \
    lexers/buffer.h \
    lexers/token.h \
    lexers/sexpressionlexer.h \
    lexers/lexererror.h \
    lexers/strbuffer.h \
    lexers/linetracker.h \
    data/sexpression.h \
    parsers/sexpressionparser.h \
    data/program.h \
    wam/wam.h \
    data/terms.h \
    lexers/prologlexer.h \
    parsers/parser.h \
    parsers/prologparser.h \
    data/ast.h \
    parsers/parsecombinators.h \
    parsers/parseresult.h \
    compiler/prologcompiler.h \
    compiler/codegenhelper.h \
    wam/builtins.h \
    wam/stack.h \
    wam/operandstack.h \
    wam/binding.h \
    dbhelper.h \
    prologengine.h

FORMS    += mainwindow.ui
QT += sql
OTHER_FILES += \
