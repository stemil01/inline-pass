# LLVM Inline Pass

This is an LLVM Pass for basic [inlining](https://en.wikipedia.org/wiki/Inline_expansion).
It detects non-recursive functions that are defined within the same module and replaces the function call with the body of the function.

## Running the pass

This pass uses the legacy Pass Manager, and detailed instructions how to write such passes can be found on [the official website](https://llvm.org/docs/WritingAnLLVMPass.html).
In short, to run the pass, the following is required:

1. Clone the [LLVM project](https://github.com/llvm/llvm-project)
2. Clone this project into the directory `llvm/lib/Transforms` and add the following line to the `CMakeLists.txt` file that is inside of it

> `add_subdirectory(InlinePass)`

3. Position to the project's root directory and build it (you can do this manually, or run this [script](https://www.prevodioci.matf.bg.ac.rs/kk/2023/vezbe/make_llvm.sh))
4. Find an LLVM IR file `test.ll` or build it (with *Clang* for example), and run the pass with the following command (assuming you are in the project's root directory):

> `./build/bin/opt -load build/lib/LLVMInlinePass.so -bugpoint-enable-legacy-pm -inline-pass -S test.ll -o result.ll`

Modified IR should be in the `result.ll` file.

## Project's structure

The pass has two parts: the inlining module and the call graph class.

- The inlining module's primary purpose is to copy the function body of the called function and to position it at the place of the call instruction. 
The associated nececcary remapping of instructions is handled here.
- Call Graph class is a class representing a graph of functions in which caller is connected a callee.
It is used to determine whether cycle exists in the module (for instance if function f calls g, g calls h, and h calls f) and ignores such functions for inlining.
Moreover, it keeps track of unused functions (functions that cannot be reached from the `main` function) and removes it from the module.

## Examples and results

TODO

## Potential Improvements

TODO