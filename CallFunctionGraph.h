#ifndef CALL_FUNCTION_GRAPH_H
#define CALL_FUNCTION_GRAPH_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace llvm;

class CallFunctionGraph {
public:
    CallFunctionGraph(Function *EntryFunction);

    void findRecursiveCalls();
    bool isRecursive(Function *F);

    // This function is used for testing purposes only
    void print();

private:
    std::unordered_map<Function *, std::vector<Function *>> m_CallGraph;
    Function *m_EntryFunction;
    std::unordered_set<Function *> m_RecursiveFunctions;

    void createCallGraph(Function *F, std::unordered_set<Function *> &VisitedFunctions);
    void findRecursiveCalls(Function *F, std::unordered_set<Function *> &Visited,
                            std::vector<Function *> &Path,std::unordered_set<Function *> &InPath);
};

#endif