# test/

This file is for testing the classes we have created.

## Usage

To run, for example, `test/node/node.cpp`, which tests the `Node` class, run `pio test -f node`.

## Guidelines

- Each test file should test **one** class. This could be a custom class in `lib/` or a library that we are using.
- Like `main.cpp`, **tests should be declarative, not imperative**.
- `Serial` commands used in the test files **must** be on baud 115200!

## Things to avoid

`test/` must not devolve into a dumping grounds for once-useful code snippets. Each file in `test/` must be used to assist in the development/usage of a class.
