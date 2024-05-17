#include "Inline.h"

bool Inline::shouldInline(Function *F, CallFunctionGraph *CallGraph) {
    return !F->isDeclaration() && !CallGraph->isRecursive(F);
}

Instruction *Inline::findFirstNonAlloca(Function *F) {
    for (BasicBlock &BB : *F) {
        for (Instruction &I : BB) {
            if (!isa<AllocaInst>(&I)) {
                return &I;
            }
        }
    }

    return nullptr;
}

void Inline::resolveAllocas(Function *Callee, Instruction *CallInstr,
                            std::unordered_map<Value *, Value *> &ValuesMap) {
    Function *Caller = CallInstr->getParent()->getParent();
    Instruction *FirstNonAlloca = findFirstNonAlloca(Caller);

    for (Instruction &I : *Callee->begin()) {
        if (!isa<AllocaInst>(&I)) {
            break;
        }

        Instruction *ICopy = I.clone();
        ICopy->insertBefore(FirstNonAlloca);
        ValuesMap[&I] = ICopy;
    }
}

void Inline::copyFunctionBody(Function *F, CallInst *CallInstr,
                                std::unordered_map<Value *, Value *> &FunctionArgumentsMap,
                                std::unordered_map<Value *, Value *> &ValuesMap) {
    std::vector<BasicBlock *> CopiedBasicBlocks;

    // create a new basic block NewBB after the original one
    // and append basic blocks before it in a loop
    BasicBlock *OriginalBB = CallInstr->getParent();
    BasicBlock *NewBB = OriginalBB->splitBasicBlock(CallInstr);
    Instruction *NewReturnValue = NewBB->getFirstNonPHI();

    for (BasicBlock &BB : *F) {
        BasicBlock *BBCopy = BasicBlock::Create(OriginalBB->getContext(), "",
                                                OriginalBB->getParent(), NewBB);

        CopiedBasicBlocks.push_back(BBCopy);
        ValuesMap[&BB] = BBCopy;

        for (Instruction &I : BB) {
            // we ignore alloca instructions
            // to avoid unnecessary stack allocations
            if (!isa<AllocaInst>(&I)) {
                Instruction *ICopy = I.clone();
                ICopy->insertInto(BBCopy, BBCopy->end());
                ValuesMap[&I] = ICopy;
            }
        }
    }

    // remapping values
    for (BasicBlock *BBCopy : CopiedBasicBlocks) {
        for (Instruction &I : *BBCopy) {
            for (size_t i = 0; i < I.getNumOperands(); i++) {
                if (ValuesMap.find(I.getOperand(i)) != ValuesMap.end()) {
                    I.setOperand(i, ValuesMap[I.getOperand(i)]);
                }
                else if (FunctionArgumentsMap.find(I.getOperand(i)) != FunctionArgumentsMap.end()) {
                    I.setOperand(i, FunctionArgumentsMap[I.getOperand(i)]);
                }
            }
        }
    }

    // handling the return instruction which is expected to be
    // the last instruction of the last basic block
    if (ReturnInst *ReturnInstr = dyn_cast<ReturnInst>(CopiedBasicBlocks.back()->getTerminator())) {
        NewReturnValue->replaceAllUsesWith(ReturnInstr->getReturnValue());
        ReturnInstr->eraseFromParent();
        BranchInst *BranchInstr = BranchInst::Create(NewBB);
        BranchInstr->insertInto(CopiedBasicBlocks.back(), CopiedBasicBlocks.back()->end());
    }
    else {
        errs() << "function does not terminate with a return instruction\n";
        return;
    }

    OriginalBB->getTerminator()->setSuccessor(0, CopiedBasicBlocks.front());
    NewReturnValue->eraseFromParent();
}

bool Inline::inlineFunction(CallInst *CallInstr) {
    std::unordered_map<Value *, Value *> FunctionArgumentsMap;
    std::unordered_map<Value *, Value *> ValuesMap;

    Function *Callee = CallInstr->getCalledFunction();

    // mapping function arguments
    auto it = Callee->arg_begin();
    for (size_t i = 0; i < CallInstr->getNumOperands() - 1 && it != Callee->arg_end(); i++) {
        FunctionArgumentsMap[&*it] = CallInstr->getOperand(i);
        it++;
    }

    resolveAllocas(Callee, CallInstr, ValuesMap);
    copyFunctionBody(Callee, CallInstr, FunctionArgumentsMap, ValuesMap);

    return true;
}
