# C-instrumentator
A C instrumentation library

# Building

## Ubuntu

Install dependencies: `sudo apt-get install clang llvm clang-tidy  git linux-libc-dev libclang-dev libclang-cpp-dev cmake build-essential`
Configure: `mkdir build && cd build && cmake .. -DBUILD_TESTING=1 -DClang_DIR=/usr/lib/cmake/clang-14 -DLLVM_DIR=/usr/lib/llvm-14/lib/cmake/llvm`
Build: `make -j4`

## macOS

Install dependencies: `brew install llvm@16`
Configure: `mkdir build && cd build && cmake .. -DBUILD_TESTING=1 -DLLVM_DIR=/opt/homebrew/opt/llvm@14 -DClang_DIR=/opt/homebrew/opt/llvm@14`
Build: `make -j4`

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

# GoalInjector

The objective of this instrumentation is to extend and reimplement some ideas from
[FuSeBMC](https://github.com/kaled-alshmrany/FuSeBMC) in a way that is
easier to integrate with ESBMC tools.

## Example

```c
int main() {
  while(1)
    while(1) ;
  return 0;
})";
```

is transformed into

```c
int main() {__ESBMC_assert(0, "0");
  while(1)
    {__ESBMC_assert(0, "1");while(1) {__ESBMC_assert(0, "2");;}}
  return 0;
})";
```
