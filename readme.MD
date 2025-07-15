
# cGit – Minimal Git Core Implementation in C

## Overview

**cGit** is a low-level project that aims to reimplement the core functionalities of Git in the C programming language. The goal is to explore how Git internally stores and manages content using objects like blobs, trees, and commits, with an emphasis on understanding Git's object model and plumbing commands.

This project should simulate how Git creates and reads compressed objects, generates tree structures from files, and builds commit chains—all through a minimal, educational interface. It relies on SHA-1 hashing and zlib compression to mirror Git’s storage format.

The program is structured to support commands such as repository initialization, object hashing, file inspection, and tree construction, with an architecture divided into commands, core logic, and utility layers.

## Build Instructions

### Prerequisites

- CMake ≥ 3.10  
- zlib  
- OpenSSL (for SHA-1 support)

### Build

```bash
./build
```

The resulting binary will be located at: `out/Release/cGit`

## Usage

```bash
./build --run <command> [args]
```

Example commands may include:

- `init` – initialize a `.mygit` repository
- `hash-object <file>` – create a blob object from a file
- `cat-file <hash>` – view the contents of a blob object
- `write-tree` – build a tree object from the current working directory
- `ls-tree <tree-hash>` – display the contents of a tree object

## Project Structure

```
cGit/
├── src/
│   ├── main.c             # CLI entry point
│   ├── commands/          # Implementation of Git-like commands
│   └── core/              # Core logic for object creation and serialization
├── include/               # Headers for all modules
├── out/                   # Compiled output binaries
├── build                  # Build script
├── CMakeLists.txt         # CMake build configuration
```

## Future Work

- Implement commit and reference support
- Add checkout-like functionality
- Refine object parsing and error handling
- Add comprehensive test coverage with Criterion
- Ensure compatibility with real Git tooling
