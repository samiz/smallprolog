#include "program.h"

namespace Prolog
{
QString Clause::toString()
{
    QStringList lst;
    for(int i=0; i<body.count(); ++i)
        lst.append(body[i]->toString());
    return QString("%1:-%2.").arg(head->toString()).arg(lst.join(", "));
}

}
