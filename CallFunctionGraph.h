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

    bool isRecursiveCall(Function *F);
    void print();

private:
    std::unordered_map<Function *, std::vector<Function *>> m_CallGraph;

    void createCallGraph(Function *F, std::unordered_set<Function *> &VisitedFunctions);
};

#endif