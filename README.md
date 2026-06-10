# Toy C Compiler #


A toy C compiler, compiling to x86 Assembly.


## Features ##


- [x] Declarations
- [x] Assignments
- [x] If Statements
- [x] While Loops
- [x] Function Calls

## Future Work ##


### Types ###

The current implementation is restricted to `int` only, eliminating the need for type checking.
Future work will look to address this, by introducing `char`, `float`, and other primitive datatypes found in C.

### Register Allocation ###

For simplicity, all variables are stored on the stack. This allowed for quick iterations to produce working assembly. Replacing this methodology with Linear Scan Register Allocation, or ideally Graph Colouring, will improve code performance.

### Optimisations ###

Zero code-optimisations are implemented. The assembly emitted is far-from optimal.

### Preprocessing ###

No macros (`#define`) are supported yet.

### Arithmetic ###

The mathematical capability of the compiler is limited to just addition (`+`). 

### Conditionals ###

The following conditionals are recognised by the Lexer and Parser: `<`, `>`, `==`. However, only `==` emits working assembly.

## Using the compiler ##

1. Create a simple c program `input.c` 


```
int add_two(int a, int b)
{
  int z = a + b;
}

int main()
{
  add_two(1, 2);
  int x = 5;
  while (x == 5)
  {
    x = x + 1;
  }
}
```

2. Compile the program using `make`

3. View the output in `test.asm`
