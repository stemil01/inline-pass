#include "CallFunctionGraph.h"

void CallFunctionGraph::createCallGraph(Function *F, std::unordered_set<Function *> &VisitedFunctions) {
    VisitedFunctions.insert(F);

    for (BasicBlock &BB : *F) {
        for (Instruction &I : BB) {
            if (CallInst *CallInstr = dyn_cast<CallInst>(&I)) {
                Function *CalledFunction = CallInstr->getCalledFunction();

                m_CallGraph[F].push_back(CalledFunction);
                if (VisitedFunctions.find(CalledFunction) == VisitedFunctions.end()) {
                    createCallGraph(CalledFunction, VisitedFunctions);
                }
            }
        }
    }
}

CallFunctionGraph::CallFunctionGraph(Function *EntryFunction) {
    std::unordered_set<Function *> VisitedFunctions;
    createCallGraph(EntryFunction, VisitedFunctions);
}

bool CallFunctionGraph::isRecursiveCall(Function *F) {
    
}

void CallFunctionGraph::print() {
    for (auto &MapPair : m_CallGraph) {
        errs() << MapPair.first->getName() << ": \n";
        for (Function *F : MapPair.second) {
            errs() << "\t" << F->getName() << "\n";
        }
    }
}
