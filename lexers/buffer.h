/**************************************************************************
** The Kalimat programming language
**   Copyright (C) 2010 Mohamed Samy Ali - samy2004@gmail.com
**   This project is released under the Apache License version 2.0
**   as described in the included license.txt file
**************************************************************************/

#ifndef PL_BUFFER_H
#define PL_BUFFER_H

#include <QtCore>
#include <functional>

using namespace std;
namespace Lex
{
template<class BufferType>
struct BufferState
{
    BufferType *buffer;
    int p1,p2;
};

template<class Collection,
         class Element,
         Element (*sourceAt)(Collection &, int),
         class State,
         class BufferType>

class Buffer
{
protected:
    Collection source;

    // p2 is the "lookahead" pointer, p1 is the "reset" pointer.
    // Both are indexes in the 'source' variable.
    int p1, p2;

public:
    Buffer() { }

    virtual void init(Collection source)
    {
        this->source = source;
        p1 = 0;
        p2 = 0;
    }

    // Have we read all characters?
    bool eof()
    {
        return eof(p2);
    }

    bool eof(int pos)
    {
        return pos>= source.count();
    }

    // Return true if the next QChar is c, does not advance the read index
    bool readAhead(Element c)
    {
        return readAhead(c,0);
    }

    bool readAhead(Element c, int offset)
    {
        if(eof(p2+offset))
            return false;
        if(source.at(p2+offset)==c)
            return true;
        return false;
    }

    // Return true if the next set of QChars to read is s.
    bool readAhead(Collection s)
    {
        for(int i=0; i<s.length();i++)
        {
            if(!readAhead(s.at(i), i))
            {
                return false;
            }
        }
        return true;
    }

    // Called with the assumption that at least one character can be read
    Element readAhead()
    {
        Element c = source.at(p2);
        return c;
    }

    // return true if we are at the first Character of a line
    bool isStartOfLine();

    // Reads a QChar, advances p2. (all readAhead functions don't advance p2)
    virtual Element read()
    {
        Element c = source.at(p2++);
        return c;
    }

    void readMany(Collection string)
    {
        // Todo:
        // This is unoptimized but at least doesn't mess up the startOfLine
        // which is handled in char read() so we use it here:
        for(int i=0; i<string.length(); i++)
            read();
    }

    // Seeks forwards only!
    void seek(int nextPos)
    {
        while(p1 < nextPos &&!eof())
        {
            read();
            p1 = p2;
        }
        return;
    }

    Collection snapShotPos(int pos, Element caret)
    {
        Collection snap;
        int from = pos - 10;
        if(from < 0)
            from = 0;
        for(int i=from; i<pos; i++)
        {
            snap.append(sourceAt(source, i));
        }

        snap.append(caret);
        int to = pos + 10;
        if(to >= source.length()-1)
            to = source.length();

        for(int i=pos; i<to; i++)
        {
            snap.append(sourceAt(source, i));
        }

        return snap;
    }

    // Returns the QString between p1 and p2, inclusive. and sets p1 = p2
    Collection accept()
    {
        Collection ret = acceptable();
        p1 = p2;
        return ret;
    }

    // Returns the QString that would've been returned by accept(), but doesn't change buffer state
    Collection acceptable()
    {
        Collection ret;
        for(int i=p1; i<p2; i++)
        {
            ret.append(sourceAt(source, i));
        }
        return ret;
    }


    // Cancels all read operations from the last start state, i.e sets p2 = p1
    void reset()
    {
        p2 = p1;
    }

    virtual void unread()
    {
        p2--;
    }

    // decrements p2 by numChars
    void retract(int numChars)
    {
        for(int i=0; i<numChars; i++)
        {
            unread();
        }
    }

    virtual void saveState(State &state)
    {
        state.buffer = (BufferType *) this;
        state.p1 = p1;
        state.p2 = p2;
    }

    virtual void restoreState(State &state)
    {
        p1 = state.p1;
        p2 = state.p2;
    }

};

template<class T>
T elementAt(QVector<T> &v, int i)
{
    return v.at(i);
}

template<class T>
class SimpleVectorBuffer : public Buffer<
    QVector<T>,
    T,
    elementAt,
    BufferState<SimpleVectorBuffer<T> >,
        SimpleVectorBuffer<T>
        >
{
    typedef BufferState<SimpleVectorBuffer<T> > State;
};
}
#endif // BUFFER_H
