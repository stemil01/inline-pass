#include "Inline.h"

bool Inline::shouldInline(Function *F, CallFunctionGraph *CallGraph) {
    return !F->isDeclaration() && !CallGraph->isRecursive(F);
}

void Inline::copyFunctionBody(Function *F, CallInst *CallInstr,
                                std::unordered_map<Value *, Value *>& FunctionArgumentsMap) {
    std::unordered_map<Value *, Value *> ValuesMap;
    std::vector<BasicBlock *> CopiedBasicBlocks;

    // create new basic block NewBB after the original one
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
            Instruction *ICopy = I.clone();
            ICopy->insertInto(BBCopy, BBCopy->end());

            ValuesMap[&I] = ICopy;
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

    Function *CalledFunction = CallInstr->getCalledFunction();

    // mapping function arguments
    auto it = CalledFunction->arg_begin();
    for (size_t i = 0; i < CallInstr->getNumOperands() - 1 && it != CalledFunction->arg_end(); i++) {
        FunctionArgumentsMap[&*it] = CallInstr->getOperand(i);
        it++;
    }

    copyFunctionBody(CalledFunction, CallInstr, FunctionArgumentsMap);

    return true;
}
