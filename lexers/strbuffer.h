#ifndef STRBUFFER_H
#define STRBUFFER_H

#include "./buffer.h"
#include "./linetracker.h"

namespace Lex
{
template<class BufferType>
struct StringBufferState : public BufferState<BufferType>
{
    int CurrentLine, CurrentLineStartPos;
};

QChar strAt(QString &s, int i);

class StrBuffer : public Lex::Buffer<QString, QChar, strAt, StringBufferState<StrBuffer>,
        StrBuffer>
{
    int CurrentLine, CurrentLineStartPos;
    Lex::LineTracker tracker;
public:
    typedef StringBufferState<StrBuffer> State;

    void init(QString source)
    {
        Buffer::init(source);
        CurrentLine = 0;
        CurrentLineStartPos = 0;
    }

    QChar read()
    {
        QChar c = Buffer::read();

        if(c == '\n')
        {
            tracker.markLine(CurrentLine, CurrentLineStartPos);
            CurrentLineStartPos = p2;
            CurrentLine++;
        }
        if(eof())
        {
            // Add an extra line start to the tracker
            // So that the length of the real last line
            // is the difference in position between it
            // and the dummy last line
            tracker.markLine(CurrentLine, CurrentLineStartPos);
            CurrentLineStartPos = p2;
            CurrentLine++;
        }
        return c;
    }

    void unread()
    {
        QChar c = source.at(p2-1);
        if(c == '\n')
        {
            // When we read forwards again we'll
            // re-add the line info
            CurrentLine--;
            int d;
            tracker.line(CurrentLine, CurrentLineStartPos, d);
        }

        Buffer::unread();
    }

    void saveState(StringBufferState<StrBuffer> &state)
    {
        Buffer::saveState(state);
        state.CurrentLineStartPos = CurrentLineStartPos;
        state.CurrentLine = CurrentLine;
    }

    void restoreState(StringBufferState<StrBuffer> &state)
    {
        Buffer::restoreState(state);
        CurrentLineStartPos = state.CurrentLineStartPos;
        CurrentLine = state.CurrentLine;
    }

    int GetLine()
    {
        return tracker.lineFromPos(p1);
    }

    int GetColumn()
    {
        int l, c;
        tracker.lineColumnOfPos(p1, l, c);
        return c;
    }

    int GetPos()
    {
        return p1;
    }

    void updateStartOfLine(int pos)
    {
        /*
        if(eof())
        {
            LineStart = false;
            return;
        }

        QChar c = source.at(pos);
        if(c=='\n')
            LineStart = true;
        else
            LineStart = false;
            */
    }
};
}
#endif // STRBUFFER_H
