# Operators

Some basic operators/symbols you'll encounter along the way:

| Symbol(s) | Example   | Description                                          |
|-----------|-----------|------------------------------------------------------|
|     ^     |  `x ^ 3`  | *exponentiate* (not bitwise xor)                     |
|     *     |  `x * 3`  | multiply two operands                                |
|     /     |  `5 / y`  | divide two operands, C-style                         |
|     %     | `-11 % 4` | get remainder of two operands.                       |
|    //     | `-5 // y` | divide two operands, python-style                    |
|    %%     | `-1 %% 4` | get *modulus* of two operands.                       |
|     +     | `+1 + 2`  | add two operands (or also unary +)                   |
|     -     | `-1 - 5`  | subtract two operands (also unary -, i.e. negation)  |
|     =     |  `x = 5`  | set equal to.  LHS must be settable.                 |
|     ==    |  `x == 5` | check equality; returns true iff LHS equals RHS.     |
|     !=    |  `x != 5` | check not equal; returns false iff LHS equals RHS.   |
|    and    | `x and 3` | returns LHS if falsy, else RHS                       |
|     or    |  `x or 3` | returns LHS if truthy, else RHS                      |
|    xor    | `x xor 3` | returns Null if LHS and RHS are both truthy or falsy otherwise returns the single truthy value |
|     $     |  `x $= 3` | creates a [lambda function](lambda_functions.md)     |
|     <<    |  `3 << 5` | bit-shift left (multiply LHS by 2^RHS)               |
|     >>    |  `3 >> 1` | bit-shift right (divide LHS by 2^RHS)                |
|     &     |  `2 & 4`  | bitwise and                                          |
|     \|    |  `2 \| 4` | bitwise or                                           |
|     ><    |  `x >< y` | returns bitwise xor of two operands                  |
|     <>    |  `<>x`    | bitflip unary operator, flips bits on operand        |

## Note on division and remainder operators

For both C-style (`/` and `%`) and python-style (`//` and `%%`) division/remainders,
we have the following identities for a numerator `n` and denominator `d`:

```
    n =  (n / d) * d +  (n % d)
    n = (n // d) * d + (n %% d)
```

### Integer operands

For integer operands, `/` will round towards zero, whereas `//` will round towards
negative infinity.  When `n` and `d` are both positive (or both negative), the two
operations are equivalent (`n / d == n // d` and `n % d == n %% d`), but the
differences come in when `n` or `d` are negative.  Some examples:

|   n   |   d   |   n / d   |   n % d   |   n // d  |   n %% d  |
|-------|-------|-----------|-----------|-----------|-----------|
|  15   |   7   |     2     |     1     |     2     |     1     |
| -11   |   7   |    -1     |    -4     |    -2     |     3     |
|  -3   |  -7   |     0     |    -3     |     0     |    -3     |
|  10   |  -7   |    -1     |    -3     |    -2     |     4     |

The end result is that `n %% d` is always positive if `d > 0`, which makes it useful
as a modulus for indexing lists.
