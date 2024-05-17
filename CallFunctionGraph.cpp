#include "CallFunctionGraph.h"

CallFunctionGraph::CallFunctionGraph(Function *EntryFunction)
    : m_EntryFunction(EntryFunction) {}

void CallFunctionGraph::createCallGraph() {
    m_CallGraph.clear();
    m_UsedFunctions.clear();

    createCallGraph(m_EntryFunction);
}

void CallFunctionGraph::createCallGraph(Function *F) {
    m_UsedFunctions.insert(F);

    for (BasicBlock &BB : *F) {
        for (Instruction &I : BB) {
            if (CallInst *CallInstr = dyn_cast<CallInst>(&I)) {
                Function *Callee = CallInstr->getCalledFunction();

                m_CallGraph[F].push_back(Callee);
                if (m_UsedFunctions.find(Callee) == m_UsedFunctions.end()) {
                    createCallGraph(Callee);
                }
            }
        }
    }
}

void CallFunctionGraph::findRecursiveCalls(Function *F, std::unordered_set<Function *> &Visited,
                                            std::vector<Function *> &Path,
                                            std::unordered_set<Function *> &InPath) {
    Visited.insert(F);
    Path.push_back(F);
    InPath.insert(F);

    for (Function *Callee : m_CallGraph[F]) {
        if (InPath.find(Callee) != InPath.end()) {
            for (size_t i = Path.size() - 1; Path[i] != Callee; i--) {
                m_RecursiveFunctions.insert(Path[i]);
            }
            m_RecursiveFunctions.insert(Callee);
        }
        else if (Visited.find(Callee) == Visited.end()) {
            findRecursiveCalls(Callee, Visited, Path, InPath);
        }
    }

    Path.pop_back();
    InPath.erase(F);
}

void CallFunctionGraph::findRecursiveCalls() {
    std::vector<Function *> Path;
    std::unordered_set<Function *> Visited, InPath;
    findRecursiveCalls(m_EntryFunction, Visited, Path, InPath);
}

bool CallFunctionGraph::isRecursive(Function *F) {
    return m_RecursiveFunctions.find(F) != m_RecursiveFunctions.end();
}

bool CallFunctionGraph::removeUnusedFunctions() {
    std::vector<Function *> UnusedFunctions;
    for (Function &F : *m_EntryFunction->getParent()) {
        if (m_UsedFunctions.find(&F) == m_UsedFunctions.end()) {
            UnusedFunctions.push_back(&F);
        }
    }

    for (Function *F : UnusedFunctions) {
        F->eraseFromParent();
    }

    return !UnusedFunctions.empty();
}

void CallFunctionGraph::print() {
    for (auto &MapPair : m_CallGraph) {
        errs() << MapPair.first->getName() << ": \n";
        for (Function *F : MapPair.second) {
            errs() << "\t" << F->getName() << "\n";
        }
    }

    errs() << "Recursive functions: \n";
    for (Function *F : m_RecursiveFunctions) {
        errs() << "\t" << F->getName() << "\n";
    }
}
