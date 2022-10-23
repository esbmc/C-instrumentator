# C-instrumentator
A C instrumentation library

The objective of this tool is to extend and reimplement some ideas from
[FuSeBMC](https://github.com/kaled-alshmrany/FuSeBMC) in a way that is
easier to integrate with ESBMC tools.

# Project Structure

 The project is structured as follows:

 - `.github`: Actions scripts 
 - `include`: Header files
 - `src`: Library files
 - `unit`: Unit tests

# Dependencies

I want to keep the dependencies at a minimum. So far, the required dependencies are:

- LLVM/Clang > 11
- fmt

Please, avoid adding dependencies. The idea is for this to be used as a library.

# How it works

It basically adds GOAL labels into something