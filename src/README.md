# src/

This source folder should always have one file: `main.cpp`. It is the file that bootstraps the project.

## Usage

To run `main.cpp`, run the command `pio run`.

## Guidelines

- `main.cpp` should include every class within `lib/`. If a class in `lib/` is not included in `main.cpp`, it shouldn't exist.
- Generally speaking, **`main.cpp`'s code should be more declarative than imperative**—the imperative code should be within the classes in `lib/`.

## Things to avoid

- `main.cpp` should never be modified to fit the role of a test file. Class troubleshooting should be done within the `test/`.
