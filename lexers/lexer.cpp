#include "buffer.h"
#include "regexp.h"
#include "lexer.h"

namespace Lex
{
shared_ptr<Token> Lexer::accept(TokenType type)
{
    int l, c, p;
    l = buffer.GetLine();
    c = buffer.GetColumn();
    p = buffer.GetPos();
    QString lexeme = buffer.accept();

    shared_ptr<Token> ret = make_shared<Token>(lexeme, type, p, l, c);
    return ret;
}

void Lexer::tokenize()
{
    bool found;
    acceptedTokens.clear();
    errors.clear();
    while(true)
    {
        if(buffer.eof())
            break;

        found = false;
        shared_ptr<LexerError> lastErr;
        for(QMap<int, shared_ptr<RegExp> >::const_iterator i=rules.begin();
            i!=rules.end(); ++i)
        {
            shared_ptr<RegExp> exp = i.value();
            auto result = exp->match(buffer);
            if(result->success())
            {
                 shared_ptr<Token> t = accept(i.key());
                 if(!skipTokens.contains(i.key()))
                 {
                    acceptedTokens.append(t);
                 }
                 found = true;
                 break;
            }
            else if(!lastErr || result->lastPos > lastErr->lastPos)
                lastErr = result;
        }
        if(!found)
        {

            errors.append(QString("Near '%1', %2")
                          .arg(buffer.snapShotPos(lastErr->lastPos, '^'))
                          .arg(lastErr->toString()));
            shared_ptr<ExpectedSingle> es = dynamic_pointer_cast<ExpectedSingle>(lastErr);
            if(es)
            {
                if(es->lastPos > buffer.GetPos())
                {
                    buffer.seek(es->lastPos);
                    continue;
                }
                else
                {
                    break;
                }
            }

            shared_ptr<ExpectedOneOf> eo = dynamic_pointer_cast<ExpectedOneOf>(lastErr);
            if(eo)
            {
                if(eo->lastPos > buffer.GetPos())
                {
                    buffer.seek(eo->lastPos);
                    continue;
                }
                else
                {
                    break;
                }

            }
            shared_ptr<IllegalHere> ih = dynamic_pointer_cast<IllegalHere>(lastErr);
            if(ih)
            {
                int newPos = ih->lastPos + ih->found.length();
                if(newPos > buffer.GetPos())
                {
                    buffer.seek(newPos);
                    continue;
                }
                else
                {
                    break;
                }
            }
            break; // can't recover
        }
    }
}
}
