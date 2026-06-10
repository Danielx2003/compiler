### Toy C Compiler ###

---

A toy C compiler, compiling to x86 Assembly.


## Features ##

---

* Declarations [-]
* Assignments [-]
* If Statements [-]
* While Loops [-]
* Function Calls [-]

## Future Work ##

---

# Types #

The current implementation is restricted to `int` only, eliminating the need for type checking.
Future work will look to address this, by introducing `char`, `float`, and other primitive datatypes found in C.

# Register Allocation #

For simplicity, all variables are stored on the stack. This allowed for quick iterations to produce working assembly. Replacing this methodology with Linear Scan Register Allocation, or ideally Graph Colouring, will improve code performance.

# Optimisations #

Zero code-optimisations are implemented. The assembly emitted is far-from optimal.

# Preprocessing #

No macros (`#define`) are supported yet.

# Functions # 

Despite the misleading return type `int add_two(int a, intb)`, functions are not equipped with the ability to return values yet, as the `return` keyword does not exist. 

## Using the compiler ##

---

1. Create a simple c program `input.c` 


```int add_two(int a, int b)
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

2. Compile the program

```make```

3. View the output in `test.asm`
