#ifndef BUILTINS_H
#define BUILTINS_H

#include "wam.h"
namespace Prolog
{
void sqrt(Wam &);
void plus(Wam &vm);
void minus(Wam &vm);
void mul(Wam &vm);
void div(Wam &vm);
void concat(Wam &vm);

void lt(Wam &vm);
void gt(Wam &vm);
void le(Wam &vm);
void ge(Wam &vm);
void ne(Wam &vm);

}
#endif // BUILTINS_H
