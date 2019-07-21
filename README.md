# Final project for the databases university class

The project is an implementation of a facade over database, which task is to properly handle requests given as JSON objects, store and fetch information during run.

The full specification (in polish language) is given [here](https://github.com/KoncepcyjnyMiliarder/databases_final_project/blob/master/lecturers_specification.md).

Program is written in object oriented, modern style C++, compiles and runs both on Windows and Linux.

## Requirements
+ c++11 compiler
+ installed PostgreSQL database
+ pgcrypto extension

## How to use?
For convenience, create `build` directory, run `cmake`, then `make`. As specified in the task, first run the program with --init argument.

![Build](https://raw.githubusercontent.com/KoncepcyjnyMiliarder/databases_final_project/master/build.png)

From now on, you can use the executable to give orders to the system.

![Run](https://raw.githubusercontent.com/KoncepcyjnyMiliarder/databases_final_project/master/run.png)

## Implementation details

### Code

Abstract factory pattern is at the very core of the application. The two modes of run (`init` and standard) are completely separated by being implemented in two unrelated factories. The product is a instance of class responsible for handling a particular request. Every request handler is encapsulated in one class. The main loop of the program operates on abstract commands, which makes it very easy to add next handlers without modifying existing parts of code.

### Underlying database structure

E-R diagram for PostgreSQL database:

![DBStructure](https://raw.githubusercontent.com/KoncepcyjnyMiliarder/databases_final_project/master/documentation/er-diagram.png)
