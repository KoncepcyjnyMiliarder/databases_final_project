# Final project for the databases university class

The project is an implementation of a facade over database, which task is to properly handle requests given as JSON objects, store and fetch information during run.

The full specification (in polish language) is given [here](https://github.com/KoncepcyjnyMiliarder/databases_final_project/blob/master/lecturers_specification.md).

Program compiles and runs both on Windows and Linux.

## Requirements
+ c++11 compiler
+ installed PostgreSQL database
+ pgcrypto extension

## How to use?
For convenience, create `build` directory, run `cmake`, then `make`. As specified in the task, first run the program with --init argument.

![Build](https://raw.githubusercontent.com/KoncepcyjnyMiliarder/databases_final_project/master/build.png)

From now on, you can use the executable to give orders to the system.

![Run](https://raw.githubusercontent.com/KoncepcyjnyMiliarder/databases_final_project/master/run.png)
