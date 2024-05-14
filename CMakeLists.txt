add_llvm_library( LLVMInlinePass MODULE
  InlinePass.cpp
  Inline.cpp
  CallFunctionGraph.cpp

  PLUGIN_TOOL
  opt
  )
