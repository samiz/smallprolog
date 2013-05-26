/**************************************************************************
**   The Kalimat programming language
**   Copyright 2010 Mohamed Samy Ali - samy2004@gmail.com
**   This project is released under the Apache License version 2.0
**   as described in the included license.txt file
**************************************************************************/

#ifndef PL_LINETRACKER_H
#define PL_LINETRACKER_H

#include <QString>
#include <QMap>
namespace Lex
{
class LineTracker
{
    QMap<int, int> lineStartPositions;
    int maxLine;
public:
    LineTracker() { maxLine = -1;}
    void reset() { lineStartPositions.clear(); maxLine = -1; }
    // Assumes marking is always linear from start to finish
    void markLine(int lineNo, int startPos);
    void removeLast() { lineStartPositions.remove(lineStartPositions.count()-1);}
    void lineColumnOfPos(int pos, int &line, int &col);
    int lineFromPos(int pos);
    int posFromLineColumn(int line, int column);
    void linesFromTo(int pos1, int pos2, int &lineFrom, int &lineTo);
    int lineCount() { return lineStartPositions.count();}
    void line(int i, int &start, int &len);
};
}
#endif // LINETRACKER_H
