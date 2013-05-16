/**************************************************************************
**   The Kalimat programming language
**   Copyright 2010 Mohamed Samy Ali - samy2004@gmail.com
**   This project is released under the Apache License version 2.0
**   as described in the included license.txt file
**************************************************************************/

#include "linetracker.h"
#include <QStringList>

void LineTracker::markLine(int lineNo, int startPos)
{
    lineStartPositions[lineNo] = startPos;
    if(lineNo > maxLine)
        maxLine = lineNo;
}

int LineTracker::lineFromPos(int pos)
{
   int line, col;
   lineColumnOfPos(pos, line, col);
   return line;
}

void LineTracker::lineColumnOfPos(int pos, int &line, int &col)
{
    // todo: replace linear search with binary search
    int i=0;
    int iPos;
    while(true)
    {
        iPos=lineStartPositions[i];
        if(i>= maxLine || lineStartPositions[i+1]> pos)
            break;
        i++;
    }

    line = i;
    col = pos - iPos;
}

int LineTracker::posFromLineColumn(int line, int column)
{
    return lineStartPositions[line] + column;
}

void LineTracker::linesFromTo(int pos1, int pos2, int &lineFrom, int &lineTo)
{
    int col;
    lineColumnOfPos(pos1, lineFrom, col);
    lineColumnOfPos(pos2, lineTo, col);
}

void LineTracker::line(int i, int &start, int &len)
{
    start = lineStartPositions[i];
    if(i == maxLine)
        len = 0;
    else
        len = lineStartPositions[i+1] - start;
}
