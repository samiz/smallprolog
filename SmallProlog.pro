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
    lexers/lexer.cpp \
    lexers/regexp.cpp \
    lexers/buffer.cpp \
    lexers/token.cpp \
    lexers/sexpressionlexer.cpp \
    lexers/lexererror.cpp \
    lexers/strbuffer.cpp \
    lexers/linetracker.cpp \
    sexpression.cpp \
    sexpressionparser.cpp \
    program.cpp \
    wam.cpp \
    terms.cpp \
    lexers/prologlexer.cpp \
    parsers/parser.cpp \
    parsers/prologparser.cpp \
    ast.cpp \
    parsers/parsecombinators.cpp \
    parsers/parseresult.cpp \
    prologcompiler.cpp \
    codegenhelper.cpp \
    builtins.cpp

HEADERS  += mainwindow.h \
    lexers/lexer.h \
    lexers/regexp.h \
    lexers/buffer.h \
    lexers/token.h \
    lexers/sexpressionlexer.h \
    lexers/lexererror.h \
    lexers/strbuffer.h \
    lexers/linetracker.h \
    sexpression.h \
    sexpressionparser.h \
    program.h \
    wam.h \
    terms.h \
    lexers/prologlexer.h \
    parsers/parser.h \
    parsers/prologparser.h \
    ast.h \
    parsers/parsecombinators.h \
    parsers/parseresult.h \
    prologcompiler.h \
    codegenhelper.h \
    builtins.h

FORMS    += mainwindow.ui

OTHER_FILES += \
