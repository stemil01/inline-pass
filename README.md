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

## Examples and results

TODO