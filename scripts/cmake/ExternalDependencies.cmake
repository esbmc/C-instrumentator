include(CPM)
CPMAddPackage("gh:fmtlib/fmt#7.1.3")

# BUG: Removing catch will result in link problems with LLVM. I have no clue for the reason why
CPMAddPackage("gh:catchorg/Catch2@3.6.0")

include(FindLLVM)
