#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include "Inline.h"
#include "CallFunctionGraph.h"

#include "llvm/IR/LegacyPassManager.h"

using namespace llvm;

namespace {
struct InlinePass : public ModulePass {
    static char ID;
    InlinePass() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {
        Function *MainFunction = M.getFunction("main");
        CallFunctionGraph *CallGraph = new CallFunctionGraph(MainFunction);
        CallGraph->createCallGraph();
        CallGraph->findRecursiveCalls();

        bool IRChanged = false;
        std::vector<CallInst *> CallInstructions;

        do {
            for (CallInst *CallInstr : CallInstructions) {
                if (Inline::inlineFunction(CallInstr)) {
                    IRChanged = true;
                }
            }

            CallInstructions.clear();
            for (BasicBlock &BB : *MainFunction) {
                for (Instruction &I : BB) {
                    if (CallInst *CallInstr = dyn_cast<CallInst>(&I)) {
                        if (Inline::shouldInline(CallInstr->getCalledFunction(), CallGraph)) {
                            CallInstructions.push_back(CallInstr);
                        }
                    }
                }
            }
        } while (!CallInstructions.empty());

        CallGraph->createCallGraph();
        if (CallGraph->removeUnusedFunctions()) {
            IRChanged = true;
        }

        delete CallGraph;

        return IRChanged;
    }
};
}  // end of anonymous namespace

char InlinePass::ID = 0;
static RegisterPass<InlinePass> X("inline-pass", "Inline Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
