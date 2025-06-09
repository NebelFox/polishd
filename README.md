# Polishd

This is a library for evaluating arithmetic expressions using [postfix notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation). The name `polishd` is a wordplay of `polish` and `polished`, implying the used notation.

The [demo](dmeo/) folder contains a simple command-line project showcasing the library. Check it out for a complete usage example in the code.

## Features

* Define a completely custom grammar with a set of operations and constants
* Compile a string expression into a `Function` object
* Use named parameters in expressions
* Get infix and postfix string representations of a compiled `Function`

## Getting Started

These instructions will get you a copy of the project on your local
machine and let you run the **demo** application.

### Prerequisites

* **CMake v3.19+** - found at [https://cmake.org/](https://cmake.org/)

* **C++ Compiler** - i.e. *MSVC*, *GCC*, *Clang*

> ***Note:*** *You also need to be able to provide ***CMake*** a supported
[generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html), i.e. **Ninja**.*

### Clone

```bash
git clone https://github.com/Nebelfox/polishd/
```

### Generate CMake files

Depends on the **CMake** generator you use. For example, for **Ninja**:

```bash
mkdir build
cd build
cmake -G "Ninja" ..
```

### Build

```bash
cmake --build build
```

### Run the Demo

```bash
build/demo/demo.exe
```
