# Polishd

This is a library for evaluating arithmetic expressions using [postfix notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation). The name `polishd` is a wordplay of *polish* and *polished*, implying the used notation.

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

## Usage

### Define a Grammar

```c++
polishd::Grammar grammar;

grammar.add_constant("g", 9.8);

grammar.add_prefix_operator("-", [](double x) { return x; });
grammar.add_prefix_operator("sin", std::sin);

grammar.add_binary_operator("+", [](double a, double b) { return a + b; }, 1);

grammar.add_postfix_operator("?", [](double x) { return x != 0; });
```

### Evaluate an expression

```c++
double result = polishd::compile(grammar, "-(sin g?)").evaluate();

polishd::Args args; // just an alias to std::unordered_map<std::string, double>
args["x"] = 4.2;
result = polishd::compile(grammar, "x + g").evaluate(args);
```

### Save and reuse a compiled expression

```c++
polishd::Function f = polishd::compile(grammar, "-(x + x)");
double xs[4] {0.1, 5.4, -2, 453.522};
double results[4];
for(size_t i=0; i<4; ++i)
    results[i] = f(xs[i]);
```

### Get the infix and postfix representations

```c++
std::string infix = f.infix(); // 1 + 1
std::string postfix = f.postfix(); // 1 1 +
```

> **Note**: *The infix() returns the original string that was parsed, so `polishd::compile(grammar, "1   +1").infix()` would give `"1  +1"` instead of `"1 + 1"` or `"1+1"`.*

### Access (read-only) the Grammar entries

```c++
const double a = 4.2, b = 2.5;
for (const auto& [name, value]: grammar.constants())
    printf("const `%s` = %f\n", name.c_str(), value);

for (const auto& [name, op]: grammar.prefix())
    printf("prefix operator `%s`. `%s %f` = %f\n", name.c_str(), name.c_str(), a, op(a));

for(const auto& [name, op]: grammar.binary())
    printf("binary operator `%s` with precedence %d. `%f %s %f` = %f\n", name.c_str(), op.precedence, a, name.c_str(), b, op.binary(a, b));

for (const auto& [name, op]: grammar.postfix())
    printf("postfix operator `%s`. `%f%s` = %f\n", name.c_str(), a, name.c_str(), op(a));
```

## Semantics and Caveats

Each argument is treated as a potential usage of a constant.
During the compilation, the list of constants of the `Compiler`'s `Grammar` is checked
for the argument name, and if such a constant is present,
the argument is treated as referencing the constant
and is "hardcoded" in the compiled function to evaluate to the constant value.

Currently, the names and signatures of entries within a `Grammar` are isolated by kind
and are not cross-checked anyhow for duplicates,
so you can have a constant `e` and a prefix operator `e` at the same time.

The expected behavior in such a case is based on the parsing rules.

## Parsing Rules

### Definitions

* **Number** - any floating point number that fits into `double` type. Consists of `-.0123456789` characters.
* **Argument** - a non-empty string without whitespaces.
* **Prefix Operator** - an unary function that operates on the value to the right of it.
* **Binary Operator** - a binary function that operates on the two values adjacent to it.
  * **Binary Operator Precedence** - `0` meaning the lowest and `255` meaning the highest. Adjacent binary operators are executed in the descending order of their *precedence*. For example, given operator `+` with *precedence* `0` and operator `*` with *precedence* `1`, the expression `2+3*4` is equivalent to `2+(3*4)`.
* **Postfix Operator** - an unary function that operates on the value to the left of it.
* **Opening Parenthesis** `(` - marks the start of a sub-expression,
  i.e. a part of an expression that is evaluated in isolation from outer expression
  and is then treated as a single number by it. Sub-expressions could be nested.
* **Closing Parenthesis** `)` - marks the end of the most-recently started sub-expression.
* **Operand** - either a **Number**, an **Argument**, a **Prefix Operator** or an **Opening Parenthesis**.
* **Operator** - either a **Binary Operator**, a **Postfix Operator** or a **Closing Parenthesis**.

### Rules

* An expression must start with an **Operand**.
* A **Number**, an **Argument**, a **Closing Parenthesis** and a **Postfix Operator** must be followed by an **Operator**.
* A **Prefix Operator**, a **Binary Operator** and an **Opening Parenthesis** must be followed by an **Operand**.
* The **Operand** variants are attempted to be parsed in the following order:
   1. **Number**
   2. **Prefix Operator**
   3. **Opening Parenthesis**
   4. **Argument**
* The **Operator** variants are attempted to be parsed in the following order:
  1. **Binary Operator**
  2. **Postfix Operator**
  3. **Closing Parenthesis**

> **Note**: *Hence, given a prefix operator with name `sin` and an argument with name `sin`, any `sin` in an expression would be treated as the **Prefix Operator**, as they are parsed prior to **Argument**s.*
