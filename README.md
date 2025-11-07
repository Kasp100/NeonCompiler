# NeonCompiler: The Neoncode Compiler and Language Server

This repository hosts **NeonCompiler**, the compiler and language server for **Neoncode**.

Neoncode is a general-purpose, compiled programming language designed for **safe**, **efficient**, and **expressive** programming.

---

## The Neoncode Programming Language

The full language specification is not yet published, but here are some of the core principles and features of the language:

### Default = Safest
Neoncode enforces safety and predictability through secure and immutable behavior unless explicitly relaxed.

- Types without `mut` are **immutable**  
- Variables declared without `var` cannot be **reassigned**  
- Regular classes without `extendable` cannot be extended (i.e., used as supertypes)

### Separation Between Internal and External Data
A type marked `serialisable` indicates that its data may come directly from I/O operations.  
This enforces a clear boundary between trusted internal data and untrusted external input.

### Safe Memory Model
Neoncode eliminates manual memory management by using **reference counting**, allowing for predictable lifetime management and **C interoperability**.

### Programming Paradigms
Neoncode combines **object-oriented** and **functional** programming paradigms to promote code reuse, modularity, and safety.

#### Private Fields
Fields in non-serialisable types are **inaccessible from outside code** (i.e., private).  
Auto-generated getters and setters make data access safe and explicit.

### Strictly Pure Functions
The compiler enforces purity for functions marked `pure`.

#### Pure Function Sets
Pure function sets allow pure functions to be grouped and imported similarly to a type.  
They may contain:
- Pure functions (compiler-enforced)
- Constants (strictly immutable data)

### Interoperability with Other Languages
**C interoperability** enables Neoncode to directly use existing C libraries and codebases.

### Metaprogramming
Neoncode supports compile-time metaprogramming and code generation through "**compile functions**" — functions written in Neoncode that operate on the **AST (Abstract Syntax Tree)**.  
Compile function calls are explicit and use the `auto:` prefix.

### Custom Grammar
Neoncode supports custom **expression grammars** through grammar sets, enabling domain-specific syntax extensions.

---

## Compiler and Language Server

NeonCompiler serves as both the **compiler** and the **language server** for Neoncode.

### Compiler
The compiler is built on **LLVM** for flexible and efficient compilation, supporting multiple compilation targets.

### Language Server
NeonCompiler also acts as a **language server** for IDE integration.

Planned and implemented features include:
- Real-time syntax highlighting  
- Code navigation and structure analysis  
- Diagnostics, warnings, and errors  
- Potentially other advanced features to support efficient development in Neoncode
