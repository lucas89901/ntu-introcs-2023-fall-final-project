# NTU Introduction to Computer 2023 Fall Final Project - Assembler & VM Translator

Author: B12902110 呂承諺 (100% contribution)

For the full commit history of the programs in this project, see [my main nand2tetris repository](https://github.com/gpwaob92679/nand2tetris).

## Abstract

In this final project, I implemented projects 6 through 8 of the textbook, namely the Hack assembler and VM-to-Hack translator. I chose C++ as my programming language and opted for an object-oriented design approach, not fully complying with the suggestions from the textbook.

I learned several software development tools through the process of doing this project. [CMake](https://cmake.org/), a cross-platform build system generator, is used to build this project, as it allows code to be compiled on various platforms with different compilers, and also simplifies testing and binary distribution. I chose [Abseil](https://github.com/abseil/abseil-cpp) as my support library, utilizing it's logging library, flags library and string utilities. For unit testing, [GoogleTest](https://github.com/google/googletest) is the framework of choice.

Possible future improvements include:
- Automated testing of the assembler
- Produce better error and warning messages from the assembler and the VM translator
- Optimize generated VM code (for example, when `index` is 0 or 1 for `push` and `pop` commands)

## Assembler

assembler - An assembler for the Hack platform.

### Usage

```
assembler SOURCE
```

*`SOURCE`*: Source assembly file to be assembled.

### Description

The assembler is a rather simple program. It translates Hack assembly code into Hack machine code with a two-pass approach, processing labels in the first pass and translating code in the second pass.

### Build and test

#### Requirements

- CMake 3.20 or later
- A C++ compiler supporting at least C++17

#### Build

To build the assembler, run the following commands:

```sh
cd assembler
cmake -B build
cmake --build build --target install
```

The compiled binary should appear under the `assembler` directory.

#### Test

At the time of writing the assembler, I didn't think of writing unit tests and automated tests, so I tested each test program from the textbook manually.

To test that the assembler is working correctly, compare the machine code output of our assembler to that of the textbook's assembler.

## VM translator

vmtranslator - A VM code to Hack assembly code translator.

### Usage

```
vmtranslator [-v] [-d] SOURCE
```

- *`SOURCE`*: Source VM program to be translated.
- `-v`: Verbose output. Print translated assembly code to console.
- `-d`: Debug mode. Write VM source lines as comments in assembly output.

If *`SOURCE`* is a single VM file (for example, `MyProgram.vm`), the translator will output the assembly file in the same directory as *`SOURCE`* (for example, `Program.asm`). If *`SOURCE`* is a directory (for example, `MyProgram`), the translator will gather and translate each VM file in the directory and output the assembly file in the directory (for example, `MyProgram/MyProgram.asm`).

### Description

The VM translator is the primary focus of the final project. It is comprised of four modules—`command`, `addressing`, `parser`, and the main program.

The `commands` module contains classes for each of the 17 VM commands of the Hack platform. The `ToAssembly()` method returns assembly code of the command. Thanks to an object-oriented design, it also offers abstract base classes for extensibility. If developers want to extend the VM command set, they could inherit from base classes such as `BinaryArithmeticCommand` and `UnaryArithmeticCommand` and override the virtual function `ToAssembly()`.

The `addressing` module contains classes for each of the 8 memory segments of the Hack platform. The `AddressingAssembly()` methods returns assembly code that stores the address of the value to be accessed in registers specified in its argument `destination`. Developers could also introduce custom memory segments by inheriting from an appropriate abstract base class and overriding `AddressingAssembly()`.

The `parser` module parses an VM file and provides a friendly interface for accessing the commands.

The main program drives the entire translation using the other 3 modules. It has a verbose mode, which also prints the translated assembly to the console, and a debug mode, which write VM source lines as comments in assembly output, both of which can be enabled via command-line flags.

### Build and test

#### Requirements

- CMake 3.20 or later
- A C++ compiler supporting at least C++17
- A Java runtime environment that can run nand2tetris's CPU emulator (used in a portion of tests)

#### Build

To build the VM translator, run the following commands:

```sh
cd vmtranslator
cmake -B build
cmake --build build --target install
```

The compiled binary should appear under the `vmtranslator` directory.

#### Test

This project contains unit tests for the `addressing` and `commands` modules, as well as automated tests of test programs provided from the textbook.

To run the tests after building, run the `ctest` command under the `build` directory. The output is similar to the following:

```
Test project /tmp2/b12902110/ntu-introcs-2023-fall-final-project/vmtranslator/build
      Start  1: DestinationTest.DestinationString
 1/62 Test  #1: DestinationTest.DestinationString ............................   Passed    0.01 sec
      Start  2: AddressingTest.ArgumentAddress
 2/62 Test  #2: AddressingTest.ArgumentAddress ...............................   Passed    0.01 sec
      Start  3: AddressingTest.LocalAddress
 3/62 Test  #3: AddressingTest.LocalAddress ..................................   Passed    0.01 sec
...
      Start 59: Translation: test_programs/FunctionCalls/FibonacciElement/
59/62 Test #59: Translation: test_programs/FunctionCalls/FibonacciElement/ ...   Passed    0.01 sec
      Start 60: Comparison: test_programs/FunctionCalls/FibonacciElement/
60/62 Test #60: Comparison: test_programs/FunctionCalls/FibonacciElement/ ....   Passed    0.21 sec
      Start 61: Translation: test_programs/FunctionCalls/StaticsTest/
61/62 Test #61: Translation: test_programs/FunctionCalls/StaticsTest/ ........   Passed    0.01 sec
      Start 62: Comparison: test_programs/FunctionCalls/StaticsTest/
62/62 Test #62: Comparison: test_programs/FunctionCalls/StaticsTest/ .........   Passed    0.21 sec

100% tests passed, 0 tests failed out of 62

Total Test time (real) =   2.57 sec
```
