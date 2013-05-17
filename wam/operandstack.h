#ifndef OPERANDSTACK_H
#define OPERANDSTACK_H

#include "stack.h"
#include "../data/terms.h"
#include "binding.h"
#include <memory>
using namespace std;

const int fourK = 4096;
const int ValueSize = 16;
const int trailCount = 1024;
// OperandStackChunkSize _must_ be a power of two!
// or modulo(...) won't work!!
const int OperandStackChunkSize = fourK / ValueSize;

typedef Stack<shared_ptr<Term::Term>, OperandStackChunkSize> OperandStack;
typedef Stack<Binding, trailCount> TrailStack;
#endif // OPERANDSTACK_H
