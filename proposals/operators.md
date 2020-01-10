# Operators

Some basic operators/symbols you'll encounter along the way:

| Symbol(s) | Example   | Description                                          |
|-----------|-----------|------------------------------------------------------|
|     ^     |  `x ^ 3`  | *exponentiate* (not bitwise xor)                     |
|     *     |  `x * 3`  | multiply two operands                                |
|     /     |  `5 / y`  | divide two operands                                  |
|     %     | `-11 % 4` | get *modulus* of two operands.  NOT remainder.       |
|     \     |  `11 \ 4` | get remainder of two operands after division         |
|     +     | `+1 + 2`  | add two operands (or also unary +)                   |
|     -     | `-1 - 5`  | subtract two operands (also unary -, i.e. negation)  |
|     =     |  `x = 5`  | set equal to.  LHS must be settable.                 |
|     ==    |  `x == 5` | check equality; returns true iff LHS equals RHS.     |
|     !=    |  `x != 5` | check not equal; returns false iff LHS equals RHS.   |
|     $     |  `x $= 3` | creates a [lambda function](lambda_functions.md)     |
|     ><    |  `x >< y` | returns bitwise xor of two operands                  |
|     <>    |  `<>x`    | bitflip unary operator, flips bits on operand        |
