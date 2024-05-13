add_llvm_library( LLVMInlinePass MODULE
  InlinePass.cpp
  Inline.cpp

  PLUGIN_TOOL
  opt
  )
