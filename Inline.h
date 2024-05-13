#ifndef INLINE_H
#define INLINE_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <unordered_map>

using namespace llvm;

namespace Inline {
    bool shouldInline(const Function *F);
    void copyFunctionBody(Function *F, CallInst *CallInstr, std::unordered_map<Value *, Value *>& FunctionArgumentsMap);
    bool inlineFunction(CallInst *CallInstr);
}

#endif