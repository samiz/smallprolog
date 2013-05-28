#ifndef BUILTINS_H
#define BUILTINS_H

#include "./wam.h"
using namespace Wam;
namespace Prolog
{
void sqrt(Wam::Wam &);
void plus(Wam::Wam &vm);
void minus(Wam::Wam &vm);
void mul(Wam::Wam &vm);
void div(Wam::Wam &vm);
void concat(Wam::Wam &vm);

void lt(Wam::Wam &vm);
void gt(Wam::Wam &vm);
void le(Wam::Wam &vm);
void ge(Wam::Wam &vm);
void ne(Wam::Wam &vm);

void assert(Wam::Wam &vm);
void delete_(Wam::Wam &vm);
void write(Wam::Wam &vm);
void dumpTrail(Wam::Wam &vm);

}
#endif // BUILTINS_H
