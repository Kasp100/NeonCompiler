# NeonCompiler

This repository contains NeonCompiler, the eventual compiler and language server for Neoncode.

Neoncode is a statically typed, compiled, object-oriented programming language designed around **safe defaults, explicit capabilities, and controlled side effects**.

The full language specification is found .

The Neoncode compiler uses **LLVM** for code generation and provides compiler-inserted **reference-counted memory management**, so memory management does not need to be handled manually by the programmer.

Some of Neoncode's core characteristics include:

* **Static typing** with no type inference
* **Object-oriented programming** with generics
* **Reference semantics** - values behave as references from the programmer's perspective, with `=` used for reassignment
* **Explicit mutation control** through `mut:`, `own`, `borrow`, and `shared`
* **Explicit effect annotations** for mutation and I/O
* **Compiler-managed concurrency**, including mutex and atomic behaviour
* **Result/error values** for error handling
* A strong focus on **C interoperability**

Neoncode does not use nullable types. The term *optional* is reserved for values or features that are genuinely optional rather than being used as a general nullability mechanism.

For the language's design principles and complete semantics, see the [Neoncode language specification](https://github.com/Kasp100/Neoncode-docs).

NeonCompiler is being developed as both the compiler and language server for Neoncode. The language server will provide IDE integration, including real-time syntax highlighting, code structure analysis, diagnostics, and other language-aware features.

## Status

NeonCompiler is currently in very early development. The parser and AST are under development, and many compiler and language-server features are not yet implemented.