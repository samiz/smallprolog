#-------------------------------------------------
#
# Project created by QtCreator 2013-05-06T05:09:57
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=gnu++0x -Wno-unused-parameter
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmallProlog
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lexer.cpp \
    regexp.cpp \
    buffer.cpp \
    token.cpp \
    sexpressionlexer.cpp \
    lexererror.cpp \
    strbuffer.cpp \
    linetracker.cpp \
    sexpression.cpp \
    sexpressionparser.cpp \
    program.cpp \
    wam.cpp \
    terms.cpp \
    prologlexer.cpp \
    parser.cpp \
    prologparser.cpp \
    ast.cpp \
    parsecombinators.cpp \
    parseresult.cpp \
    prologcompiler.cpp \
    codegenhelper.cpp

HEADERS  += mainwindow.h \
    lexer.h \
    regexp.h \
    buffer.h \
    token.h \
    sexpressionlexer.h \
    lexererror.h \
    strbuffer.h \
    linetracker.h \
    sexpression.h \
    sexpressionparser.h \
    program.h \
    wam.h \
    terms.h \
    prologlexer.h \
    parser.h \
    prologparser.h \
    ast.h \
    parsecombinators.h \
    parseresult.h \
    prologcompiler.h \
    codegenhelper.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    syntax.txt
