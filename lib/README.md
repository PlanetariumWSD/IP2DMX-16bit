# lib/

This folder contains the classes that are used by `main.cpp`. These classes do most of the code base's work.

## Guidelines

- Generally speaking, these classes should be concise and easily testable.
- These classes should contain the imperative code of the code base. If imperative code finds its way into `main.cpp` and `test/`, it will increase the chance of duplicate code appearing, making program maintenance exceptionally challenging.
