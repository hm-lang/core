# overview of types

Standard types whose instances can take up an arbitrary amount of memory:

* `int`: signed big-integer
* `rtl`: rational number (e.g. an `int` over an `int`)
* `str`: array/sequence of utf8 bytes, but note that `string` is preferred in code
    since it includes other containers which deterministically provide utf8 bytes.

Other types which have a fixed amount of memory:

* `null`: should take up no memory, but can require an extra bit for an optional type.
* `flt`: single-precision floating-point number, AKA `f32`
* `dbl`: double-precision floating-point number, AKA `f64`
* `bool`: can hold a True or False value
* `rune`: a utf8 character, presumably held within an `i32`
* `u8`: unsigned byte (can hold values from 0 to 255, inclusive)
* `u16` : unsigned integer which can hold values from 0 to 65535, inclusive
* `u32` : unsigned integer which can hold values from 0 to 2^32 - 1, inclusive
* `u64` : unsigned integer which can hold values from 0 to 2^64 - 1, inclusive

and similarly for `i8` to `i64`, using two's complement.  For example,
`i8` runs from -128 to 127, inclusive, and `u8(i8(-1))` equals `255`.

TODO: support for `u128` to `u512`, as well as `i128` to `i512`

## casting between types

The default rounding behavior for `dbl` (or `flt`) to `int` is to use stochastic rounding.
This rounds 0.7 to 1 with probability 70% and to 0 with probability 30%, and similarly
for other values (using the fractional part to determine the probability weights).

```
X: dbl = 5.43
Y: int = X      # Y = 5 with probability 57%, or 6 with probability 43%.
```

## types of types

TODO: discuss type and classType.
TODO: switch ~type to Out~type everywhere.

# operators and precedence

Operator priority.

TODO: add : , ;

TODO: ternary assignment -- do not use question mark operator ?

TODO: see if member access needs to be LTR
TODO: `MyInstance someMethod someFunction() someThingElse()` should resolve as
      `(MyInstance someMethod(someFunction())) someThingElse()`.
TODO: `MyInstance someMethod someFunction() SomeField` should resolve as
      `(MyInstance someMethod(someFunction())) SomeField`.
TODO: `MyInstance someMethod someFunction()()` should resolve as
      `MyInstance someMethod(  ( someFunction() )()  )`.

| Precedence| Operator  | Name                      | Type/Usage        | Associativity |
|:---------:|:---------:|:--------------------------|:-----------------:|:-------------:|
|   0       |   `()`    | parentheses               | grouping: `(A)`   | RTL           |
|           |   `[]`    | parentheses               | grouping: `[A]`   |               |
|           |   `{}`    | parentheses               | grouping: `{A}`   |               |
|   1       |   `_`     | subscript/index/key       | binary: `A_B`     | RTL           |
|           |   `^`     | superscript/power         | binary: `A^B`     |               |
|           |   `^^`    | repeated application      | on fn: `a^^B(X)`  |               |
|           | `**x/y/z` | library module import     | special: `**a/b`  |               |
|           | `*/x/y/z` | relative module import    | special: `*/a/b`  |               |
|   2       |   `[ ]`   | function call             | on fn: `a B`      | RTL           |
|           |   `[ ]`   | member access             | binary: `A B`     |               |
|   3       |   `&`     | bitwise AND               | binary: `A&B`     | LTR           |
|           |   `\|`    | bitwise OR                | binary: `A\|B`    |               |
|           |   `><`    | bitwise XOR               | binary: `A><B`    |               |
|           |   `<>`    | bitwise flip              | unary:  `<>A`     |               |
|           |   `>>`    | bitwise right shift       | binary: `A>>B`    |               |
|           |   `<<`    | bitwise left shift        | binary: `A<<B`    |               |
|   4       |   `*`     | multiply                  | binary: `A*B`     | LTR           |
|           |   `/`     | divide                    | binary: `A/B`     |               |
|           |   `%`     | modulus                   | binary: `A%B`     |               |
|           |   `//`    | integer divide            | binary: `A//B`    |               |
|           |   `%%`    | remainder after //        | binary: `A%%B`    |               |
|   5       |   `+`     | add                       | binary: `A+B`     | LTR           |
|           |   `-`     | subtract                  | binary: `A-B`     |               |
|   6       |   `==`    | equality                  | binary: `A==B`    | LTR           |
|           |   `!=`    | inequality                | binary: `A!=B`    |               |
|   7       |   `&&`    | logical AND               | binary: `A&&B`    | LTR           |
|           |  `\|\|`   | logical OR                | binary: `A\|\|B`  |               |
|   8       |   `=`     | assignment                | binary: `A = B`   | LTR           |
|           |  `???=`   | compound assignment       | binary: `A += B`  |               |

## division and remainder operators: / // % %%

The standard division operator, `/`, will promote integer operands to a rational return value.
E.g., `dbl(3/4) == 0.75` or `6/4 == rtl(3)/rtl(2)`.

The integer division operator, `//`, will return an integer, rounded towards zero, e.g.,`3//4 == 0`
and `-3//4 == 0`.  Also, `5//4 = 1` and `-5//4 = -1`, and `12 // 3 == 4` as expected.
If any operand is a double, the resulting value will be an integer double, e.g.,
`5.1 // 2 == 2.0`.

The modulus operator, `%`, will put the first operand into the range given by the second operand.
E.g., `5 % 4 == 1`, `123.45 % 1 == 0.45`.  Mathematically, we use the relation
`A % B == A - B * floor(A/B)`.

The remainder operator, `%%`, has the property that `A %% B == A - B * (A // B)`;
i.e., it is the remainder after integer division.  The remainder operator, `%%`,
differs from the modulus, `%`, when the operands have opposing signs.

|  `A`  |  `B`  | `floor(A/B)`  |  `A % B`  | `A // B`  | `A %% B`  |
|:-----:|:-----:|:-------------:|:---------:|:---------:|:---------:|
|   1   |   5   |      0        |     1     |     0     |     1     |
|  -1   |   5   |     -1        |     4     |     0     |    -1     |
|   1   |  -5   |     -1        |    -4     |     0     |     1     |
|  -1   |  -5   |      0        |    -1     |     0     |    -1     |
|  13   |   5   |      2        |     3     |     2     |     3     |
| -13   |   5   |     -3        |     2     |    -2     |    -3     |
|  13   |  -5   |     -3        |    -2     |    -2     |     3     |
| -13   |  -5   |      2        |    -3     |     2     |    -3     |
|  56   |   7   |      8        |     0     |     8     |     0     |
|  56   |  -7   |     -8        |     0     |    -8     |     0     |
|  6.78 |   1   |      6.0      |    0.78   |     6.0   |    0.78   |
| -6.78 |   1   |     -7.0      |    0.22   |    -6.0   |   -0.78   |

# declaring and using variables

Variables are named using `UpperCamelCase` identifiers.  The `:` symbol is used
to declare deeply constant, non-reassignable variables, and `;` is used to declare
mutable, reassignable variables.

```
# declaring and setting a non-reassignable variable that holds a big integer
Y: int = 5
# also equivalent: `Y := 5` or `Y := int(5)`.

# using the variable:
print(Y * 30)

Y = 123     # COMPILER ERROR, Y is non-reassignable.
Y += 3      # COMPILER ERROR, Y is deeply constant.
```

Mutable/reassignable/non-constant variables can use `VariableName = Expression`
after their first initialization, but they must be declared with a `;` symbol.

```
# declaring a reassignable variable that holds a big integer
X; int

# X is default-initialized to 0 if not specified.
X += 5      # now X == 5 is True.

# you can also define the value inline as well:
W ;= 7
# also equivalent, if you want to be explicit about the type.
W; int = 7
# also equivalent:
W ;= int(7)
```

Note that we use `;` and `:` as if it were an annotation on the variable name (rather
than the type) so that we don't have to worry about complex types like a mutable
array of a constant integer.  Constant variables are deeply constant, and mutable
variables are modifiable/reassignable, and we only have to think about this
(as programmers using the language) at the level of the variable itself,
not based on the type of the variable.  The underlying type is the same for both
constant and mutable variables (i.e., a mutable type), but the variable is only
allowed to mutate the memory if it is declared as a mutable variable with `;`.

## nullable variable types

To make it easy to indicate when a variable can be nullable, we reserve the question mark
symbol, `?`, to place after the type.  E.g., `X: int?` declares a variable `X` that
can be an integer or null.  An optional type can be made more explicit using the notation,
`someType|null`, e.g., `int|null`.  The default value for an optional type is `Null`.

Note that for an optional type with more than one real type, it is a compiler error
to use `?`.  E.g., `someType|anotherType?` looks a bit like it could be a non-nullable
`someType` instance, or a nullable `anotherType` instance.  Instead, use
`someType|anotherType|null` to make it clear that null is its own type and does not
make any other type act like a pointer.
TODO: maybe use the typescript-like notation `X?: int` or `X?: int|dbl`.

One of the cool features of hm-lang is that we don't require the programmer
to check for null on a nullable type before using it.  The compiler will automatically
check for null on variables that can be null.  This is also helpful for method chaining
on classes (see more on those below).  If your code calls a method on an instance that
is null, a null will be returned instead (and the method will not be called).

```
# define a class with a method called `someMethod`:
someClass := { someMethod(): int }

Nullable; someClass? = Null

Value := Nullable someMethod()  # `Value` has type `int|null` now.

# eventually we want to support things like this, where the compiler
# can tell if the type is nullable or not:
if Nullable != Null
    NonNullValue := Nullable someMethod()   # `Value` here must be `int`.
```

TODO: how to handle forced conversions from null to non-null types

## nested/object types

You can declare an object type inline with nested fields.  The nested fields defined
with `:` are immutable, and `;` are mutable.

```
Vector; (X: dbl, Y: dbl, Z: dbl) = (X: 4, Y: 3, Z: 1.5)
Vector X += 4   # COMPILER ERROR, field `X` of object is immutable

# note however, as defined, Vector is reassignable since it was defined with `;`:
Vector = (X: 1, Y: 7.2)
# note, missing fields will be default-initialized.
Vector Z == 0   # should be True.

# to make an object variable non-reassignable, use := when defining:
Vector2 := (X: 3.75, Y: 3.25)
# or you can use `:` with an explicit type specifier and then `=`:
Vector2: (X: dbl, Y: dbl) = (X: 3.75, Y: 3.25)
# then these operations are invalid:
Vector2 X += 3          # COMPILER ERROR, object is immutable
Vector2 = (X: 1, Y: 2)  # COMPILER ERROR, variable is non-reassignable
```

You can define a type/interface for objects you use multiple times.

```
# a plain-old-data class with 3 non-reassignable fields, X, Y, Z:
vector3 := (X: dbl, Y: dbl, Z: dbl)

# you can use `vector2` now like any other type, e.g.:
Vector3 := vector3(X: 5, Y: 10)
```

We also allow type definitions with mutable fields, e.g. `(X; int, Y; dbl)`.
Depending on how the variable is defined, however, you may not be able to change
the fields once they are set.  If you define the variable with `;`, then you
can reassign the variable or modify the mutable fields.  But if you define the
variable with `:`, the object is deeply constant, regardless of the field definitions.

```
# vector2 has two reassignable fields, X and Y:
vector2 := (X; dbl, Y; dbl)

# when defined with `;`, the object is mutable and reassignable.
MutableVec2; vector2 = (X: 3, Y: 4)
MutableVec2 = vector2(X: 6, Y: 3)   # OK
MutableVec2 X += 4                  # OK
MutableVec2 Y -= 1                  # OK

# when defined with `:`, the object is deeply constant, so its fields cannot be changed:
ImmutableVec2: vector2 = (X: 5, Y: 3)    # note you can use : when defining.
ImmutableVec2 = vector2(X: 6, Y: 3)  # COMPILE ERROR, ImmutableVec2 is non-reassignable
ImmutableVec2 X += 4                 # COMPILE ERROR, ImmutableVec2 is deeply constant
ImmutableVec2 Y -= 1                 # COMPILE ERROR, ImmutableVec2 is deeply constant
```

## temporarily locking mutable variables

You can also make a variable non-reassignable and deeply constant
for the remainder of the current block by using `@lock` before the variable name.

```
X; int = 4  # defined as mutable and reassignable

if SomeCondition
    @lock X = 7 # locks X after assigning it to the value of 7.
                # For the remainder of this indented block, you can use X but not reassign it
                # you also can't use mutable, i.e., non-const, methods on X.
else
    @lock X # lock X to whatever value it was for this block.
            # You can still use X but not reassign/mutate it.

print(X)    # will either be 7 (if SomeCondition was true) or 4 (if !SomeCondition)
X += 5      # can modify X back in this block; there are no constraints here.
```

## hiding variables for the remainder of the block

TODO: @hide annotation, doesn't descope the variable, just hides it from being used
by new statements/functions.

# functions

Functions are named using `lowerCamelCase` identifiers.  All functions
must return something, and `null` is a valid return type.  (There is no "void" type.)
TODO: Probably it's ok to avoid writing in the `null` return type; if no
return type is specified, `null` is assumed, unless we are writing an inline function
where we can infer the return argument value immediately.

```
# declaring a function with no arguments that returns a big integer
v(): int

# setting/defining/initializing the function usually requires an indent+1 block following:
v(): int
    return 600

# but in simple cases like this you can also define inline:
v() := 600

# function with X,Y double-precision float arguments that returns nothing
v(X: dbl, Y: dbl): null
    print("X = ${X}, Y = ${Y}, atan(Y, X) = ${**math atan(X, Y)}")
    # Note this could also be defined more concisely using $$,
    # which also prints the expression inside the parentheses with an equal sign and its value,
    # although this will print `**math atan(X, Y) = ...`, e.g.:
    # print("$${X}, $${Y}, $${**math atan(X, Y)}")

# function that takes a function as an argument and returns a function
# TODO: input function must be scoped to survive however long `wow` can be called;
# how do we want to do closure??
wow(Input fn(): string): fn(): int
    return fn(): int
        return Input fn() size()
```

## calling a function

You can call functions with arguments in any order.  Arguments must be specified
with the named identifiers in the function definition.  The only exception is
if the argument is "unnamed" (i.e., it has the same name as the type), then you
don't need to specify its name.  We'll discuss that more in the next section.

```
# definition:
v(X: dbl, Y: dbl): null

# example calls:
v(X: 5.4, Y: 3)
v(Y: 3, Y: 5.4)

# if you already have variables X and Y, you don't need to re-specify their names:
X := 5.4
Y := 3
v(X, Y)     # equivalent to `v(X: X, Y: Y)` but the redundancy is not idiomatic.
v(Y, X)     # equivalent
```

TODO: Check if the syntax works out.
We also allow calling functions without parentheses for a single argument, like this:

```
print(String): null     # definition, using an "unnamed" string, e.g., `String: string`.

# example calls:
print("Hello, world!")  # with parentheses, and without explicitly naming the variable
print "Hello, world!"   # without
```

To specify multiple arguments, one must use parentheses to avoid ambiguity.
However, if you are specifying a single, named argument, you can avoid using parentheses.

```
v(X: dbl, Int): null    # definition

v 100       # executes v(X: 0.0, Int: 100)
v X: 5      # executes v(X: 5, Int: 0)

# COMPILE ERROR: need parentheses here, since commas bind less strongly than function-spaces:
v 100, X: 10
```

A function can have a function as an argument, and there are a few different ways to call
it in that case.
```
# finds the integer input that produces "hello, world!" from the passed-in function, or -1
# if it can't find it.
detect(greet(Int): string): int
    for Check: int < 100
        if greet(Check) == "hello, world!"
            return Check
    return -1

# if your function is named the same as the function argument, you can use it directly:
greet(Int): string
    return "hay"
detect(greet)       # returns -1

# if your function is not named the same, you can do argument renaming;
# internally this does not create a new function:
sayHi(Int): string
    return "hello, world" + "!" * Int
detect(greet: sayHi)    # returns 1

# you can also create a lambda function named correctly inline -- the function
# will not be available outside, after this call (it's scoped to the function arguments).
detect(greet(Int): string
    return "hello, world!!!!" substring(Length: Int)
)   # returns 13
```

## unnamed arguments in functions

For functions with a single argument where the variable name doesn't matter,
you can use "unnamed" variables.  For primitive types, the "unnamed" identifier
is just the `UpperCamelCase` version of the `lowerCamelCase` type.

```
# this function declaration is equivalent to `f(Int: int): int`:
f(Int): int
    return Int + 5

Z := 3
f(Z)        # ok
f(int(4.3)) # ok
f(5)        # ok
```

If passing functions as an argument where the function name doesn't matter,
the "unnamed" function is `fn`.

```
q(fn(): bool): null
    if fn()
        print("function returned true!")
    else
        print("function returned false!")

q(nameItWhatYouWant(): bool
    return True
)   # should print "function returned true!"

# can pass in functions from outside as well:
anotherTestFunction(): bool
    return False
q(anotherTestFunction)  # should print "function returned false!"

# or you can create a nameless function yourself:
q(fn(): bool
    return random()
)   # will print one of the above due to randomness.
```

## redefining a function

To declare a reassignable function, use `;` after the arguments.

```
greetings(Noun: string); null
    print "Hello, ${Noun}!"

# you can use the function:
greetings(Noun: "World")

# or redefine it:
# option 1:
greetings = fn(Noun: string): null
    print "Hi, ${Noun}!"
# option 2:
greetings = "Greetings, ${$Noun}!"
# NOT OK: COMPILE ERROR: this looks like a redeclaration of the function, rather than a reassignment:
# also not ok if `;` is changed to `:`.
greetings(Noun: string); null
    print "Overwriting?"
```

TODO: discussion on how it needs to be clear what function overload is being redefined.

## nullable functions

The syntax for declaring a nullable/optional function is to put a `?` after the function name
but before the argument list.  E.g., `optionalFunction?(...Args): returnType` for a non-reassignable
function and swapping `:` for `;` to create a reassignable function.

When calling a nullable function, unless the function is explicitly checked for non-null,
the return type will be nullable.  E.g., `X := optionalFunction(...Args)` will have a
type of `returnType|null`.

Some examples:

```
# creating an optional method in a class:
parent := {
    @reset(X: dbl, Y: dbl)

    optionalMethod?(Z: dbl); int
}

Example ;= parent(X: 5, Y: 1)

# define your own function for optionalMethod:
Example optionalMethod(Z: dbl); int
    return floor(Z * This X)   # note that you can use `This` to access the Example instance.

Example optionalMethod(2.15)    # returns 10

# or set it to Null:
Example optionalMethod = Null

Example optionalMethod(3.21)    # returns Null

# child classes can define a "method" that overrides the parent's optional function:
child := extend(parent) {
    optionalMethod(Z: dbl); int
        return ceil(X * Y * exp(-Z))
}

Child ;= child(X: 6, Y: 2)

Child optionalMethod(0)     # returns 12

# but note that unless the method is protected, users can still redefine it.
Child optionalMethod(Z: dbl): int
    return floor(Z)

Child optionalMethod(5.4)   # returns 5

# also note that while the child method appears to always be defined,
# we cannot stop a Null from being assigned here.  this is because
# if the instance is passed into a function which takes a parent class,
# that function scope can reassign the method to Null (since the parent
# class has no restrictions).
Child optionalMethod = Null

# therefore the executable will always check for Null before calling the method:
Child optionalMethod(1.45)  # returns Null
```

## nullable arguments

When you call a function with an argument that is null, conceptually we choose the
overload that doesn't include that argument.  In other words, a null argument is
the same as a missing argument when it comes to function overloads.  Thus, we are
not free to create overloads that attempt to distinguish between all of these cases:
(1) missing argument, (2) present argument, (3) nullable argument, and (4) default argument.
Only functions for Cases (1) and (2) can be simultaneously defined; any other combination
results in a compile-time error.  Cases (3) and (4) can each be thought of as defining two function
overloads, one for the case of the missing argument and one for the case of the present argument.

Defining conflicting overloads, of course, is impossible.  Here are some example overloads;
again, only Cases (1) and (2) are compatible and can be defined together.

```
# missing argument (case 1):
someFunction(): dbl
    return 987.6

# present argument (case 2):
someFunction(Y: int): flt
    return 2.3 * flt(Y)

# nullable argument (case 3):
someFunction(Y: int?): string
    return "why not ${Y}"

# default argument (case 4):
someFunction(Y := 3): i64
    return i64(Y)
```

TODO: check if this breaks class instantiation assumptions.

What are some of the practical outcomes of these overloads?  Suppose
we define present and missing argument overloads in the following way:

```
overloaded(): dbl
    return 123.4
overloaded(Y: int): string
    return "hi ${Y}"
```

The behavior that we get when we call `overloaded` will depend on whether we
pass in a `Y` or not.  But if we pass in a null `Y`, then we also will end up
calling the overload that defined the missing argument case.  I.e.:

```
Y; int? = ... # Y is maybe null, maybe non-null

# the following calls `overloaded()` if Y is Null, otherwise `overloaded(Y)`:
Z := overloaded(Y)  
# Z has type `dbl|string` due to the different return types of the overloads.
```

The reason behind this behavior is that in hm-lang, an argument list is conceptually an object
with various fields, since an argument has a name (the field name) as well as a value (the field value).
An object with a field that is `Null` should not be distinguishable from an object that
does not have the field, since `Null` is the absence of a value.  Thus, if we count up
the number of fields in an object using `size()`, we'll get results like this:
`object() size() == 0`, `{Y: Null} size() == 0`, and `{Y: 5} size() == 1`.

We also want to make it easy to chain function calls with variables that might be null,
where we actually don't want to call an overload of the function if the argument is null.

```
# in other languages, you might check for null before calling a function on a value.
# this is also valid hm-lang but it's not idiomatic:
X := overloaded(Y) if Y != Null else Null

# TODO: see if this syntax is ok with all other uses of ?
# instead, you should use the more idiomatic hm-lang version.
# putting a ? after the function name will check all arguments for presence;
# if any argument is Null, the function will not be called and
# a Null will be returned instead.
X := overloaded?(Y)

# either way, X has type `string|null`.
```

## constant versus mutable arguments

Functions can be defined with mutable or immutable arguments, but that does
not change the function signature (cf. section on function overloads).
The important difference is that arguments defined with `;` must be copied
in from the outside (unless the external variable is already a temporary),
whereas arguments defined with `:` can be referenced without a copy.  This
is because arguments defined with `;` can be modified inside the function,
but they should not modify any variables outside of the function, even if
they are passed in.  Examples:

```
# this function makes a copy of whatever string is passed in:
copiedArgumentFunction(CopyMe; string): string
    CopyMe += "!!??"    # OK since CopyMe is defined as mutable via `;`.
    print(CopyMe)
    return CopyMe

# this function just references whatever string is passed in (no copy):
reffedArgumentFunction(Ref: string): string
    print(Ref)
    return Ref + "??!!"

MyValue: string = "immutable"
copiedArgumentFunction(CopyMe: MyValue) # prints "immutable!!??"
print(MyValue)                          # prints "immutable"
reffedArgumentFunction(Ref: MyValue)    # prints "immutable??!!"
print(MyValue)                          # prints "immutable"

Mutable; string = "mutable"
copiedArgumentFunction(CopyMe: Mutable) # prints "mutable!!??"
print(Mutable)                          # prints "mutable"
reffedArgumentFunction(Ref: Mutable)    # prints "mutable??!!"
print(Mutable)                          # prints "mutable"
```

The reason that the `;` or `:` argument definition doesn't change the function
signature is because in either case, the variables passed in from the outside
are not affected by the internal parts of the function.  That is, the function
cannot modify the external variables at all.

## move-modify-return (MMR) pattern

Since a function cannot modify variables outside of the function, any changes
that are to be made outside of the function must be effected by using the
return values of the function.  To modify an object outside of a function
using its methods inside the function, use the move-modify-return pattern.  E.g.,

```
MyObject ;= myObjectType(WhateverArgs: 5)
MyObject = modify(MyObject move())
# where the `modify` function is whatever you want:
modify(MyObjectType; myObjectType): myObjectType
    MyObjectType someMethod(12345)
    return MyObjectType move()      # compiler can probably figure out this move()
```

For this pattern to avoid unnecessary copies, the modifying function must
use the mutable argument definition (e.g., `;`), and the external caller
of the modifying function must `move()` the object into the function's arguments.

To indicate what you're doing a more explicitly, you can use the `@mod` annotation.
The `@mod` annotation adds the argument to both the function's arguments and return value.
Other values can also be passed into the function (or returned) at the regular spot(s),
but will not have the move-modify-return pattern applied.  For example:

```
modify @mod(MyObjectType; myObjectType) ():
    MyObjectType someMethod(12345)
    return MyObjectType move()      # compiler can probably figure out this move()

SomeInstance ;= myObjectType(...)
# you can also use `@mod` to call the function and have it update the variable.
modify @mod(SomeInstance) ()

# which expands into:
modify(@moved MyObjectType; myObjectType): myObjectType
    MyObjectType someMethod(12345)
    return MyObjectType move()      # compiler can probably figure out this move()

SomeInstance ;= myObjectType(...)
SomeInstance = modify(SomeInstance move())
```

TODO: discuss allowing @moved as an argument annotation in order to require someone
to `move()` a variable into the function.  don't just allow compiler warnings,
those will expand unnecessarily.

## function overloads

Functions can do different things based on which arguments are passed in.
For one function to be an overload of another function, it must be defined in the same file,
and it must have different arguments or return values.

```
greetings(String): null
    print "Hello, ${String}!"

greetings(Say: string, To: string): null
    print "${Say}, ${To}!"

greetings(Say: string, To: string, Times: int): null
    for Count: int < Times
        greetings(Say, To)

# so you call this in different ways:
greetings("World")
greetings(To: "you", Say: "Hi")
greetings(Times: 5, Say: "Hey", To: "Sam")
```

Note that you can define the function arguments as mutable (with `;`) or
immutable (with `:`) but that does **not** change the type of function.
Whether the variables can be mutated inside the function does not matter
to the interface between function and caller.

```
# Note, these are not real overloads, and defining both would throw a COMPILE ERROR

greetings(Say: string): null
    print "${Say}, world!"

greetings(Say; string): null
    Say += " wow"
    print "${Say}, world..."

# COMPILE ERROR, function overloads of `greetings` only differ in mutability of arguments;
# argument names must be unique in this case.
```

Note also, overloads must be distinguishable based on argument **names**, not types.

```
fibonacci(Times: int): int
    Previous ;= 1
    Current ;= 0
    for Count: int < Times
        NextPrevious := Current
        Current += Previous
        Previous = NextPrevious
    return Current

fibonacci(Times: dbl): dbl
    GoldenRatio: dbl = (1.0 + **math sqrt(5)) * 0.5
    OtherRatio: dbl = (1.0 - **math sqrt(5)) * 0.5
    return (GoldenRatio^Times - OtherRatio^Times) / **math sqrt(5)

# COMPILE ERROR: function overloads of `fibonacci` must have unique argument names, not argument types.
```

There is the matter of how to determine which overload to call.  We consider
only overloads that possess the specified input arguments.  If there is an explicit type
for the output (return) value, e.g., `X: dbl = calling(InputArgs...)`, then we also filter
by overloads which have that output type, e.g., `dbl` in this case.  We choose the overload
that has the fewest number of additional arguments (or zero), breaking ties with the overload
that was defined first in the file.

Note that if the output type is a data class, e.g., `{X: dbl, Y: str}`, then we
want to support destructuring, e.g., `{Y: str} = calling(InputArgs...)` and using
`Y` on subsequent lines as desired.  In this case, the output fields essentially count
as extra arguments, and we again choose the overload that has the fewest number of additional
arguments (or output fields).

We also allow calling functions with any dynamically generated arguments, so that means
being able to resolve the overload at run-time.

## function templates/generics

You can create template functions which can work for a variety of types
using the syntax `~(type1, type2, ...)` or `~type` (for just one generic type) 
after a function name.

TODO: allow defining templates inline for convenience.  e.g., `logger(T: ~t): t`,
using `~` the first time you see the type.  Use `In~t` or `Out~t`, or some more
verbose type name (e.g., `~moreVerbose`).

```
# declaration equivalent to `logger ~(type) (Type): type`:
logger ~type (Type): type
    print("got ${Type}")
    return Type

vector3 := (X: dbl, Y: dbl, Z: dbl)
Vector3 := vector3(Y: 5)
Result := logger(Vector3)   # prints "got vector3(X: 0, Y: 5, Z: 0)".
Vector3 == Result           # equals True

# implicit type request:
OtherResult := logger(5)    # prints "got 5" and returns the integer 5.

# explicit generic request:
DblResult := logger ~dbl (5)    # prints "got 5.0" and returns 5.0

# explicit type request:
AnotherDblResult := logger(dbl(4))  # prints "got 4.0" and returns 4.0
# can also use `logger dbl(4)` above or `logger dbl 4` with function spaces.
```

# classes

A class is defined with a `lowerCamelCase` identifier.
Class definitions must be constant/non-reassignable, so they are declared using
the `:=` symbol.  Variables defined within the class body are known as instance
variables, and functions defined within the class body are known as class
instance methods, or methods for short.  Class methods can access instance variables
and call other class methods.  Mutating methods -- i.e., that modify the class
instance, `This` -- must be defined with a `;;` before the method name.
We'll use the notation `SomeInstance;;someMutatingMethod()` to refer to these.
Methods which keep `This` constant are default, and we can use the notation
`SomeInstance::someMethod()` to refer to these non-mutating methods.  Note
that you can use `this` to refer to the current class instance type.

Class constructors are defined with a `;;reset(Args...)` style function,
which also allow you to reset the class instance as long as the variable is mutable.
The first `;;reset` method defined in the class is also the default constructor,
which will be called with default-constructed arguments (if any) if a default
instance of the class is needed.  It is a compiler error if a `;;reset()` method
(with no arguments) is defined after other `;;reset` methods (with arguments).

```
exampleClass := {
    # class instance variables can be defined here:
    X; int

    # class methods can be defined as well.
    # this one does not change the underlying instance:
    doSomething(Int): int
        return X + Int

    # this method mutates the class instance, so it is prefaced with `;;`:
    ;;addSomething(Int): null
        X += Int

    # classes must be resettable to a blank state, or to whatever is specified
    # as the starting value based on a `reset` function.  this is true even
    # if fields are defined as immutable.
    ;;reset(X; int): null
        This X = X move()

    # or short-hand: `;;reset(This X: int)` or even `@reset(X: int)`
    # adding `This` to the arg name will automatically set `This X` to the passed in `X`.
}

Example; exampleClass = (X: 5)  # also equivalent, `Example ;= exampleClass(X: 5)`
print(Example doSomething(7))   # should print 12
Example = exampleClass(X: 7)    # note: variable can be reassigned.
Example X -= 3                  # internal fields can be reassigned as well.

# note that if you define an instance of the class as immutable, you can only operate
# on the class with functions that do not mutate it.
ConstVar := exampleClass(X: 2)
ConstVar X += 3                 # COMPILER ERROR! `ConstVar` is deeply constant.
ConstVar = exampleClass(X: 4)   # COMPILER ERROR! variable is non-reassignable.
```

TODO: rework this now that we're using `;;` and `::` for static methods on enums/masks.

Note:  all functions defined on the class are methods, i.e., functions which operate
on a class instance (with a `This`).  There are thus no "static" functions that you
can define on the class (cf. C++).  This aids in simplicity, since we don't need to be
explicit about methods needing to access `This` for methods, since that's the default,
and not include a `This` for static functions.  If you want a static-like function,
create a function in the same file as the class.  Due to how public/private/protected
visibility works, you can still access private fields inside of the class within the same file.
One of the primary uses for static functions is to effectively have named constructors,
e.g., `MyDate: dateClass = dateClass fromIsoString("2020-05-04")`, but these are
not necessary in hm-lang since arguments are named, and we can have multiple constructors.
So the hm-lang way to do this would be `MyDate := dateClass(IsoString("2020-05-04"))`.

Lengthy aside: `someMethod(This, OtherArgs...)` could be a pythonic way to indicate
a class method and `someFunction(Args...)` could be the corresponding static function.
While it is true that a method operates on a class instance, i.e., a `This`, the notation
leaves much to be desired, since we call class methods like this:
`MyClassInstance someMethod(OtherArgs...)` and not like this:
`someMethod(MyClassInstance, OtherArgs...)`.
Instead, we think of a class body as a scope, and functions defined in a scope have access
to variables in the scope.  This saves us as programmers in the language some time
(since we don't have to include a `This` in every method) and heartache (since
we don't have dissonance between the definition syntax and call syntax).
We could support another syntax instead, requiring a `This` before every non-static
method, e.g., `This::someMethod(Args...)` and `This;;otherMethod(Args...)`,
but that requires a lot of extra typing for the programmer.

## unicode/localization support

We intend hm-lang to support all languages, and so the upper/lower-case requirements
may seem a bit strange in other alphabets.  To set a custom "UpperCamelCase" default name
for an instance of the class, use this notation:

```
örsted := {
    # define a custom default name:
    This := Örsted
    ... other class methods ...
}

# TODO: rename "unnamed" to "default-named" everywhere
# Now we can use `Örsted` to mean a default-named variable of the class örsted:
doSomething(Örsted): bool
    return ...
```

We will throw a compile error when a class begins with a non-ascii letter, unless
the class defines the default-name of a variable of the class.  Also, you'll get
a compile error unless the custom default name is the first statement in the class
definition.

## public/private/protected

We use annotations `@public`, `@private`, and `@protected` to indicate various
levels of access to a class instance's variables.  Public is default, and means
that the variable can be both accessed and modified by anyone.  Protected means
that the variable can be accessed and modified by friends, but for non-friends
the variable can only be accessed.  Friendship is defined by being in the same
directory as the module in question; i.e., being in the same file or being in
neighboring files in the same filesystem folder.  Private means the variable
can be accessed by friends only, and modified only by functions in the same module; i.e.,
the class (or other instances of the class) *or other functions in the same
file as the class definition*.  Non-friends are not able to access or modify private variables.

|  variable access  |  public   | protected |  private  |
|:-----------------:|:---------:|:---------:|:---------:|
|   module access   |   yes     |   yes     |   yes     |
|   module mutate   |   yes     |   yes     |   yes     |
|   friend access   |   yes     |   yes     |   yes     |
|   friend mutate   |   yes     |   yes     |   no      |
| non-friend access |   yes     |   yes     |   no      |
| non-friend mutate |   yes     |   no      |   no      |

The privacy for methods on a class works slightly different, but essentially
follows the same table.  Using the method depends on visibility as well
as if the method modifies the class or not, i.e., whether
the method was defined as `This;;mutatingMethod(): returnType`
or `This::nonMutatingMethod(): returnType`.  Note that the
latter are default.  Mutating methods follow the "mutate"
visibility in the table above, and non-mutating methods follow
the "access" visibility in the table above.

To put into words -- `@public` methods can be called by anyone, regardless
of whether the method modifies the class instance or not.  `@protected`
methods which modify the class instance cannot be called by non-friends,
but constant methods can be called by anyone.  `@private` methods which
modify the class instance can only be called by module functions, and
constant methods can be called by friends.

Note that reassignable methods, e.g., those defined with
`someConstantMethod(...Args); returnType` or `;;someMutatingMethod(...Args); returnType`
can only be reassigned based on their visibility as if they were variables.
I.e., public reassignable methods can be reassigned by anyone,
protected reassignable methods can be reassigned by friends or module,
and private reassignable methods can only be reassigned within the module.

## getters and setters on class instance variables

Note that all variables defined on a class are given methods to access/set
them, but this is done with syntactical sugar.  That is,
*all uses of a class instance variable are done through getter/setter methods*,
even when accessed/modified within the class.  The getters/setters are methods
named the `lowerCamelCase` version of the `UpperCamelCase` variable,
with various arguments to determine the desired action.

```
# for example, this class:
example := {
    @visibility
    X; str = "hello"
}
W = example()
W X += ", world"
print(W X)  # prints "hello, world"

# expands to this:
example := {
    @invisible
    X; str

    # getter: calls an external function with X, which can
    #         avoid a copy if the function argument is immutable.
    @visibility
    x(fn(Str): Out~type) := fn(X)

    # swapper: swaps the value of X with whatever is passed in
    #          returns the old value of X.
    @visibility
    ;;x(Str;): str
        # TODO: explain `A = B` returns the old value of `A`
        return X = Str move()
    # or, slightly more explicit (in that Str is to be modded):
    @visibility
    ;;x @mod(Str) ():
        Str = X = Str move()

    # modifier: allows the user to modify the value of X
    #           without copying it, using the MMR pattern.
    @visibility
    ;;x(fn(@moved Str): str): null
        X = fn(X move())
    # or, slightly more explicit (in that Str is to be modded):
    @visibility
    ;;x(fn @mod(Str): Out~type) := fn @mod(X)
}
W = example()
W x(fn(Str;): str
    Str += ", world"
    return Str move()
)
W x(fn(Str) := print(Str))
```

TODO: check that getter/setter visibility is correct.

If you define overloads for any of these methods on child classes,
they will be used as the getters/setters for that variable.  Anyone
trying to access the variable (or set it) will use the overloaded methods.

TODO: some example of child class overriding parent class getter/setters.


## parent-child classes and method overrides

You can define parent-child class relationships with the following syntax.
For one parent, `extend(parentClassName)`. Multiple inheritance is
allowed as well, e.g., `extend(parentOne, ParentTwo, ...)`.
We can access the current class instance using `This`,
and `this` will be the current instance's type.  Thus, `this` is
the parent class if the instance is a parent type, or a subclass if the instance
is a child class.  E.g., a parent class method can return a `this` type instance,
and using the method on a subclass instance will return an instance of the subclass.

We can access member variables or functions that belong to that the parent type,
i.e., without subclass overloads, using the syntax `parentClassName::Variable` or
`parentClassName::someMethod()`.  Use `;;` to access variables or methods that will
mutate the underlying class instance, e.g., `parentClassName;;Variable = 3`.

Some examples:

```
animal := {
    ;;reset(This Name: string): null

    # define two methods on `animal`: `speak` and `go`.
    # these are "abstract" methods, i.e., not implemented by this base class.
    speak(): null
    goes(): string

    # this method is defined, so it's implemented by the base class.
    # derived classes can still change it, though.
    escape(): null
        print "${Name} ${goes()} away!!"

    # a method that returns an instance of whatever the class instance
    # type is known to be.  e.g., an animal returns an animal instance,
    # while a subclass would return a subclass instance:
    clone(): this
        return this(Name)
}

snake := extend(animal) {
    # if no `reset` functions are defined,
    # child classes will inherit their parent `reset()` methods.

    speak(): null
        print "hisss!"
    goes(): string
        return "slithers"

    # no need to override `clone`, since we can create a snake using a name.
}

Snake := snake(Name: "Fred")
Snake escape()  # prints "Fred slithers away!!"

cat := extend(animal) {
    # here we define a `reset` method, so the parent `reset` methods
    # become hidden to users of this child class:
    ;;reset(): null
        # can refer to parent methods using class name:
        animal;;reset(Name: "Cat-don't-care-what-you-name-it")

    speak(): null
        print "hisss!"
    goes(): string
        return "saunters"

    escape(): null
        print "CAT ESCAPES DARINGLY!"

    # the parent `clone()` method won't work, so override:
    clone(): this
        # cats are essentially singletons, that cannot have their own name;
        return this()
}

Cat := cat()
Cat escape()    # prints "CAT ESCAPES DARINGLY!"
```

All abstract base classes also provide ways to instantiate using lambda functions.
All abstract methods must be defined for the instance to be created, and if a
`reset` method is defined on the parent, any arguments passed into the first reset
(i.e., which is the default constructor) should be defined for the lambda class.

```
WeirdAnimal := animal(
    Name: "Waberoo"
    speak(): null
        print "Meorooo"
    goes() := "meanders"
    escape(): null
        # to call the parent method `escape()` in here, we can use this:
        animal::escape()
        print "${Name} ${goes()} back..."
        # or we can use this:
        animal::escape()
)

WeirdAnimal escape()    # prints "Waberoo ... meanders ... meanders back ... meanders away!!"
```

TODO: explain that lambda functions that are defined as methods on the class
can use the `This` of the class, but they must be declared as `;;someLambdaMethod(...Args): returnType`
if they want to call any modifying methods of the class or change any instance variables.

## template methods

You can define methods on your class that work for a variety of types.

```
someExample := {
    Value: int
    ;;reset(Int): null
        This Value = Int
    to ~(type) (): type
        return type(Value)
}

SomeExample := someExample(5)

# you can use type inference here based on variable taking the return value:
ToString: string = SomeExample to()

# or you can explicitly ask for the type
ToDbl := SomeExample to ~(dbl) ()

# or you can explicitly ask like this:
To64 := i64(SomeExample to())
```

## generic/template classes

To create a generic class, you put the expression `~(types...)` after the
class identifier.  You can use these new types for any class inheritance
from a parent which is a generic/template class.

```
# create a class with two generic types, `key` and `value`:
genericClass~(key, value) := {
    ;;reset(This Key: key, This Value: value): null
}
# if this class is just POD, you can use the equivalent type:
# genericClass := ~(key, value) (Key: key, Value: value)

# creating an instance using type inference:
ClassInstance := genericClass(Key: 5, Value: "hello")
 
# creating an instance with template/generic types specified:
OtherInstance := genericClass ~(key: dbl, value: string) (Key: 3, Value: 4)
# note the passed-in values will be converted into the correct type.
```

TODO:
You can also have generic methods on generic classes, but we may
have to think of a clever way to achieve this.  E.g., C++ does not allow this...

## common class methods

All classes have a few compiler-provided methods which cannot be overridden.

* `;;move(): this` creates a temporary with the current instance's values, while
    resetting the current instance to a default instance -- i.e., calling `reset()`.
    Internally, this swaps pointers, but not actual data, so this method
    should be faster than copy for types bigger than the processor's word size.
* `::Type: classType` provides the class type.  This makes it easy to determine
    what the current type of a combo-type variable is at run-time.  E.g.,
    TODO: discuss how `consider`'s `case` statements cast their arguments
    to the same type as the type of the `consider` statement.
    ```
    X; null|int|dbl_
    X = [1.2, 3.4]
    consider X Type
        case null
            print("X was Null")
        case int
            print("X was an integer")
        case dbl_
            print("X was an array of double")
    ```
    Note that the `classType` type can be created directly from the type
    constructors, e.g. `null` and `int`, e.g., `classType(int) == 5 Type`.
    Note also, the `classType` type also can be printed nicely, e.g.,
    `print("asdf" Type)` prints "str", and `str(1 Type Type) == "classType"`.

TODO: check if `copy()` should be in the list.

# modules

Every file in hm-lang is its own module, and we make it easy to reference
code from other files to build applications.  All `.hm` files must be known
at compile time, so referring to other files gets its own special notation.
The operator `*/` begins a file-system search in the current directory.
and two asterisks becomes a search for a library module, e.g., `**math`.

Subsequent subdirectories are separated using forward slashes, e.g.,
`*/relative/path/to/file` to reference the file at `./relative/path/to/file.hm`,
and `..` is allowed between backslashes to go to the parent directory relative
to the current directory, e.g., `*/../subdirectory_in_parent_directory/other/file`.
Note that we don't include the `.hm` extension on the final file, and we allow
underscores in file and directory names.

For example, suppose we have two files, `vector2.hm` and `main.hm` in the same
directory.  Each of these is considered a module, and we can use backslashes
to invoke logic from these external files.

```
# vector2.hm
vector2 := {
    ;;reset(This X: dbl, This Y: dbl): null

    dot(Vector2: vector2) := X * Vector2 X + Y * Vector2 Y
}

# main.hm
Vector2Module: hm = */vector2    # .hm extension must be left off.
Vector2 := Vector2Module vector2(X: 3, Y: 4)
print(Vector2)
# TODO, support destructuring in the future:
# {vector2} := */vector2
```

You can use this `*/` notation inline as well, which is recommended
for avoiding unnecessary imports.  It will be a language feature to
parse all imports when compiling a file, regardless of whether they're used,
rather than to dynamically load modules.  This ensures that if another imported file
has compile-time errors they will be known at compile time, not run time.

```
# importing a function from a file in a relative path:
print(*/path/to/relative/file functionFromFile("hello, world!"))

# importing a function from the math library:
Angle := **math atan2(X: 5, Y: -3)
```

To import a path that has special characters, just use the special characters
inline after the `*/`, e.g., `*/sehr/übel` to reference the file at `./sehr/übel.hm`.
For a path that has spaces (e.g., in file or directory names), use parentheses to
surround the path, e.g., `[**library/path/with spaces]` for a library path or 
`[*/relative/path/with a space/to/a/great file]` for a relative path.  Or you can
use a backslash to escape the space, e.g., `**library/path/with\ spaces` or
`*/relative/path/with\ a\ space/to/a/great\ file`.  Other standard escape sequences
(using backslashes) will probably be supported.

## file access

TODO: how does file access work with the MMR pattern

# standard container classes (and helpers)

## arrays

An array contains a list of elements in contiguous memory.  You can
define an array explicitly using the notation `array~elementType`
for the type `elementType`, or implicitly with the subscript operator, `_`
(AKA "key" or "indexing" operator), using the notation `elementType_`.
E.g. `MyArray: int_` or `MyArray: array~int` for an immutable integer array.
The mutable versions of course use `;` instead of `:`.

Side note: as we will see, the subscript operator is usually a binary operator, i.e.,
requiring two operands, `A _ B`, read "A subscript B".  We make an exception for the
array type not to require a second operand -- in fact, adding one would create a
different type, the map type.

The unnamed version of an array does not depend on the element type;
it is always `Array`.  Example usage and declarations:

```
# this is an immutable array:
MyArray: dbl_ = [1.2, 3, 4.5]       # converts all to dbl
MyArray append(5)   # COMPILE ERROR: MyArray is immutable
MyArray_1 += 5      # COMPILE ERROR: MyArray is immutable

# mutable integer array:
Array; int_    # declaring a mutable, "unnamed" integer array
Array append(5)     # now Array == [5]
Array_3 += 30       # now Array == [5, 0, 0, 30]
Array_4 = 300       # now Array == [5, 0, 0, 30, 300]
Array_2 -= 5        # now Array == [5, 0, -5, 30, 300]

# mutable string array:
StringArray; string_ = ["hi", "there"]
print(StringArray pop())    # prints "there".  now StringArray == ["hi"]
```

The default implementation of `array` might be internally a contiguous deque,
so that we can pop or insert into the beginning at O(1).  We might reserve
`stack` for a contiguous list that grows in one direction only.

```
# some relevant pieces of the class definition
array~type := {
    # getter, which creates a default-initialized value if necessary.
    # (also will add default-initialized values up to this index, if above current size.)
    This;; _ (Index, fn(Type): Out~type): Out type

    # swapper, sets the value at the index, returning the old value:
    This;; _ (Index, @moved Type): type?

    # modifier, allows access to modify the internal value, via MMR pattern.
    # passes the current value at the index into the passed-in function (to be specific, moves it).
    # the return value of the passed-in function will become the new value at the index.
    This;; _ (Index, fn(@moved Type): type): null

    # returns a Null if index is out of bounds in the array:
    # getter, which returns a Null if index is out of bounds in the array:
    This _ (Index, fn(Type?): Out~type): Out type

    size(): index

    ;;append(Type;): null

    ;;pop(Index: index = -1): type

    ...
}
```

### fixed-size arrays

We declare an array with a fixed number of elements using the notation
`elementType_FixedSize`, where `FixedSize` is a constant integer expression (e.g., 5)
or a variable that can be converted to the `index` type.  Fixed-size array elements
will be initialized to the default value of the element type, e.g., 0 for number types.
Most of the methods of `array` are present on the fixed-size array type as well,
except those that modify the size of the array.  As usual, the arrays are zero-indexed,
so the first element in a fixed-size array `Array` is `Array_0`.

As an optimization, fixed-size arrays can be passed in without a copy to functions taking
an array as an immutable argument, but will be of course copied into a 
resizable array if the argument is mutable.  Some examples:

```
# immutable array of size 4
Int4: int_4 = [-1, 5, 200, 3450]
# mutable array of fixed-size 3:
Vector3; dbl_3 = [1.5, 2.4, 3.1]
print("Vector3 is {${Vector3_0}, ${Vector3_1}, ${Vector3_2}}")

# a function with a mutable argument:
doSomething(CopiedArray; dbl_): dbl_2
    # you wouldn't actually use a mutable array argument, unless you did
    # some computations using the array as a workspace.
    # PRETENDING TO DO SOMETHING USEFUL WITH CopiedArray:
    return [CopiedArray pop(), CopiedArray pop()]

# a function with an immutable argument:
doSomething(ConstArray: dbl_): dbl_2
    return [ConstArray_-2, ConstArray_-1]

# copies Vector3, of course:
print(doSomething(CopiedArray: Vector3))    # prints [3.1, 2.4]
# can bring in Vector3 by reference (i.e., no copy) here:
print(doSomething(ConstArray: Vector3))     # prints [3.1, 2.4]
```

There may be optimizations if the fixed-array size is known at compile-time,
i.e., being defined on the stack rather than the heap.  But when the fixed
size is unknown at compile time, the fixed-size array will be defined on the heap:

```
# note you can use an input argument to define the return type's
# fixed-array size, which is something like a generic:
range(Int): int_Int
    if Int < 0
        throw error("can't have a fixed negative-size array")
    Result; int_Int
    for I: int < Int
        Result_I = I
    return Result

print(range(10))    # prints [0,1,2,3,4,5,6,7,8,9]
```

## hash maps

A hash map can look up elements by key in O(1) time.  You can use the explicit
way to define a map, e.g., `VariableName: map~(key: keyType, value: valueType)`,
or you can use the implicit method with the subscript operator (`_`),
`VariableName: valueType_keyType`.  You can read the operator `_` as "keyed by",
e.g., `valueType_keyType` as "`valueType` keyed by `keyType`".  For example,
for a map from integers to strings, you can use: `MyMap: string_int`.
The default name for a map variable is `Map`, regardless of key or value type.
Note that while an array can be thought of as a map from the `index` type to
whatever the array element type is, `elementType_index` indicates a map type,
not an array type.  The array type, `elementType_` would be useful for densely
packed data (i.e., instances of `elementType` for most indices), while the map
type `elementType_index` would be useful for sparse data.

To define a map (and its contents) inline, use this notation:

```
# map from string to ints:
EmployeeIds: int_string = [
    # option 1: `X: Y` syntax
    "Jane": 123
    # option 2: `{Key: X, Value: Y}` syntax
    {Key: "Jane", Value: 123}
    # option 3: `[X, Y]` syntax, ok if key and value types are different
    ["Jane", 123]
]
# note that commas are optional if elements are separated by newlines,
# but required if elements are placed on the same line.

# equivalent definition would occur with this first line:
# `EmployeeIds := map~(key: string, value: int) [`
```

Maps require a key type whose instances can hash to an integer or string-like value.
E.g., `dbl` and `flt` cannot be used, nor can types which include those (e.g., `array ~dbl`).

```
DblDatabase; dbl_int       # OK, int is an OK key type
DblDblDatabase; dbl_dbl    # COMPILE ERROR, dbl is an invalid key type.
```

However, we allow casting from these prohibited types to allowed key types.  For example:

```
NameDatabase; string_int
NameDatabase_123 = "John"
NameDatabase_124 = "Jane"
print(NameDatabase_123.4)   # prints "John" with 60% probability, "Jane" with 40%.

# note that the definition of the key is an immutable array:
StackDatabase; string_(int_)    # parentheses are grammatically unnecessary,
                                # since subscripts go right to left.
                                # i.e., `StackDatabase; string_int_` would be ok too.
StackDatabase_[1,2,3] = "stack123"
StackDatabase_[1,2,4] = "stack124"
print(StackDatabase_[1.0, 2.0, 3.1])    # prints "stack123" with 90% probability, "stack124" with 10%
# things get more complicated, of course, if all array elements are non-integer.
# the array is cast to the key type (integer array) first.
StackDatabase_[2.2, 3.5, 4.8] = "odd"
# result could be stored in [2, 3, 4], [2, 3, 5], [2, 4, 4], [2, 4, 5],
#                           [3, 3, 4], [3, 3, 5], [3, 4, 4], [3, 4, 5]
# but the key is decided first, then the map is added to.
```

Note: when used as a map key, objects with nested fields become deeply constant,
regardless of whether the internal fields were defined with `;` or `:`.
I.e., the object is defined as if with a `:`.  This is because we need key
stability inside a map; we're not allowed to change the key or it could
change places inside the map and/or collide with an existing key.

Some relevant pieces of the class definition:

```
map~(key, value) := {
    # always returns a non-null type, adding
    # a default-initialized value if necessary:
    # returns a copy of the value at key, too.
    This;; _ (Key): value

    # getter, which will create a value instance if it's not present at Key.
    This;; _ (Key, fn(Value): ~type): type

    # returns a Null if key is not in the map.
    This _ (Key): value?

    # getter, which will pass back a Null if the key is not in the map.
    This _ (Key, fn(Value?): ~type): type

    # sets the value at the key, returning the old value:
    This;; _ (Key, Value;): value

    # allows access to modify the internal value, via MMR pattern.
    # passes the current value at the key into the passed-in function (to be specific, moves it).
    # the return value of the passed-in function will become the new value at the key.
    This;; _ (Key, fn(@moved Value): value): null

    size(): index

    ;;pop(Key): value
}
```

## sets

A set contains some elements, and makes checking for the existence of an element within
fast, i.e., O(1).  Like with map keys, the set's element type must satisfy certain properties
(e.g., integer/string-like).  The syntax to define a set is `VariableName: set~elementType`
to be explicit or `VariableName: _elementType` using the subscript operator `_` on the
opposite side of the array type (i.e., the array looks like `arrayElementType_`).
The "unnamed" variable name for a set is `Set`.

```
set~type := {
    # returns true if the passed-in element is present in the set.
    This_(Type): bool

    size(): index

    This;; += (Type;): null

    ;;pop(): type

    ...
}
```

Like the keys in hash maps, items added to a set become deeply constant,
even if the set variable is mutable.

## iterator

```
iterator~type := {
    # next value via getter function:
    next(fn(Type: type?): Out~type): Out type

    # peak via no-copy getter function:
    peak?(fn(Type: type?): Out~type): Out type

    # present only if underlying container supports removing the current element (at `peak()`)
    # returns the element, or null if no current element.
    # TODO: figure out a nice syntax for this method to automatically
    # be defined IF replace is defined, and to return a null in replace,
    # but allow it to be overridden if remove is defined separately.
    remove?(): type?

    # present only if underlying container supports inserting a new element (before `peak()`)
    insert?(Type): null

    # replaces the element at `next()` based on the return value;
    # the next value is passed in as an argument to `replace`,
    # and the iterator should increment.  cf. MMR pattern.
    # if there was an element at this point in the container,
    # and a null is returned, the element (and its former location)
    # should be deleted out of the container.
    replace(Type?): type?
}
```

For example, here is a way to create an iterator over some number of indices:

```
range~index := extend(iterator~index) {
    @private
    NextIndex: index = 0

    ;;reset(StartAt: index = 0, This LessThan: index = 0): null
        NextIndex = StartAt

    ;;next(): index?
        if NextIndex < LessThan
            Result := NextIndex
            ++NextIndex
            return Result
        return Null

    peak() := NextIndex if NextIndex < LessThan else Null
}

# TODO: ensure syntax is ok with `in` here.  maybe use annotations, e.g. @in or @iter
for Index: index in range(LessThan: 10)
    print Index
# prints "0" to "9"
```

TODO: how does this work with the MMR framework for remove, insert, etc.?

For example, here is an array iterator:

```
arrayIterator~type := extend(iterator~type) {
    # to use MMR, we need to pass in the array;
    # move the array in to avoid copying.
    # this @reset annotation creates a function signature of
    # ;;reset(
    #   This Array; type_, This NextIndex; index = 0
    # ): {Array: type_, NextIndex: index}
    # which automatically returns the old value of the Array (and NextIndex) if requested.
    @reset(Array: type_, NextIndex: index = 0)
    # To take an Array and return the Array back, no-copy, use the `with @holding` syntax:
    # e.g., 
    #   MyArray; int_ = [1,2,3,4]
    #   with Iterator ;= iterator @holding(MyArray)
    #       for Int: int in Iterator
    #           ...
    #   print(MyArray)
    #   # MyArray is now back to [1,2,3,4] unless there were changes during iteration,
    #   # but in any case, without a copy,

    ;;next(): type?
        ???
}
```

Or should we define iterators on the container itself?  E.g.,

```
array~type := {
    # const iteration, with no-copy if possible:
    forEach(Input fn(Type): forLoop): null
        for Index: index < size()
            # use the no-copy getter, here:
            # explicit:
            ForLoop := This_(Index, fn(Type) := Input fn(Type))
            # implicit:
            ForLoop := Input fn(This_Index)
            if ForLoop == forLoop Break
                break

    # no-copy iteration, but can mutate the array.
    ;;forEach(Input fn @mod(Type) (): forLoop): null
        for Index: index < size()
            # do a swap on the value based on the passed in function:
            # explicit:
            ForLoop := This_(Index, fn @mod(Type) () := Input fn @mod(Type) ())
            # implicit:
            ForLoop := Input fn @mod(This_Index) ()
            if ForLoop == forLoop Break
                break
}
```

# standard flow constructs / flow control

TODO -- description, plus `consider+case` and `if/else/elif`

## for loops

```
# for-loop with counter that is immutable inside the for-loop's block:
for Value: int < 10
    # Value goes from 0 to 9 by 1;
    # Value is not allowed to be mutated (defined with :).
    # trying to mutate it throws a compiler error.
    print Value
# prints "0" to "9" on separate newlines.

# for-loop whose counter can be modified inside the block.
for Special; int < 5
    print("A: ${Special}")
    ++Special
    print("B: ${Special}")
    ++Special
# prints "A: 0", "B: 1", "A: 3" "B: 4"

# for-loop iterating over non-number elements:
vector2 := {X: dbl, Y: dbl}
Array: vector2_ = [{X: 5, Y: 3}, {X: 10, Y: 17}]
for Vector2: in Array
    print(Vector2)
```

# printing and echoing output

TODO: allow tabbed print output.  e.g., `\n` inside of a `string()` method
will tab to the correct indent.

# enums and masks

## enumerations

We can create a new type that exhaustively declares all possible values it can take.
The syntax is `lowerCamelCase := enumerate` followed by a list of named values
(each an `UpperCamelCase` identifier), with optional values they take.  Enumerations
are mutually exclusive -- no two values may be held simultaneously.  See
masks for a similar class type that allows multiple options at once.

For example:

```
# Enums use a similar syntax as maps when being defined,
# except that the left-hand side must be an UpperCamelCase identifier:
bool := enumerate(
    False: 0
    True: 1
)
```

Enums provide a few extra additional properties for free as well, including the number of
values that are enumerated via the method `count(): index`, the min and max values
`min(): index`, `max(): index`, and some convenience methods on any instance of the enumeration.

```
Test: bool = False  # or `Test := bool::False`

# use `isUpperCamelCaseName()` to check for equality:
if Test isTrue()
    print "test is true :("
if Test isFalse()
    print "test is false!"

# get the size (number of enumerated values) of the enum:
print "bool has ${bool::count()} possibilities:"
# get the lowest and highest values of the enum:
print "starting at ${bool::min()} and going to ${bool::max()}"
```

Because of this, it is a bit confusing to create an enum that has `Count` as an
enumerated value name, but it is not illegal, since we can still distinguish between the
enumerated value (`enumName::Count`) and total number of enumerated values (`enumName::count()`).

Also note that the `count()` method will return the total number of
enumerations, not the number +1 after the last enum value.  This can be confusing
in case you use non-standard enumerations (i.e., with values less than 0):

```
sign := enumerate(
    Negative: -1
    Zero: 0
    Positive: 1
)

print "sign has ${sign::count()} values" # 3
print "starting at ${sign::min()} and going to ${sign::max()}"  # -1 and 1

weird := enumerate(
    X: 1
    Y: 2
    Z: 3
    Q: 9
)

print(weird::count())   # prints 4
print(weird::min())     # prints 1
print(weird::max())     # prints 9
```

### Testing enums with lots of values

Note that if you are checking many values, a `consider-case` statement may be more useful
than testing each value against the various possibilities.  Also note that you don't need
to explicitly set each enum value.

```
option := enumerate(
    Unselected
    NotAGoodOption
    ContentWithLife
    BetterOptionsOutThere
    BestOptionStillComing
    OopsYouMissedIt
    NowYouWillBeSadForever
)

print "number of options should be 7:  ${option::count()}"

Option1 := option ContentWithLife

# avoid doing this if you are checking many possibilities:
if Option1 isNotAGoodOption()
    print "oh no"
elif Option1 isOopsYouMissedIt()
    print "whoops"
...

# instead, consider doing this:
consider Option1
    case NotAGoodOption
        print "oh no"
    case BestOptionStillComing
        print "was the best actually..."
    case Unselected
        fallThrough
    default
        print "that was boring"
```

### Default value of an enumeration

Defaults to whatever value is 0 (the first, if no values are specified).

If no value is zero, then the first specified value is default.

## masks

Masks are generalized from enumerations to allow multiple values held simultaneously.
Each value can also be thought of as a flag or option, which are bitwise OR'd together
(i.e., `|`) for the variable instance.  Under the hood, these are unsigned integer types.
Trying to assign a negative value will throw a compiler error.

TODO: allow specifying the integer type backing the mask (or enum), e.g., `enumerate~i64(...)`

```
# the mask is required to specify types that are powers of two:
nonMutuallyExclusiveType := mask(
    # `None: 0` is automatically added as a mask option.
    X: 1
    Y: 2
    Z: 4
    T: 8
)

# has all the same static methods as enum, though perhaps they are a bit surprising:
nonMutuallyExclusiveType::count() == 16
nonMutuallyExclusiveType::min() == 0
nonMutuallyExclusiveType::max() == 15   # = X | Y | Z | T

Options ;= nonMutuallyExclusiveType()
Options isNone()    # True.  note there is no `hasNone()` method, since that doesn't
                    #        make sense -- either the mask is None or it has something.
Options |= X        # TODO: make sure it's ok to implicitly add the mask type here.
Options |= nonMutuallyExclusiveType Z   # explicit mask type

Options hasX()  # True
Options hasY()  # False
Options hasZ()  # True
Options hasT()  # False
Options isX()   # False, since Options is a combo of X and Z

Options = T
Options isT()   # True, since Options is now solely T
```

# pointers/references don't exist

To modify a value that is held by another class instance, e.g., the
element of an array, we use the MMR pattern.  The class instance will
pass in the current value (via a move) to a modifying function. 
Whatever value the modifying function returns will be used to replace the
value held by the class instance.  This obviates the need for pointers,
and though the explicit usage of the MMR pattern can look clumsy,
we can make MMR invisible using some syntactical sugar.

```
ArrayArray; int__ = [[1,2,3], [5]]
# to modify the array held inside the array, we can use this syntax:
ArrayArray_0 append(4)  # now ArrayArray == [[1,2,3,4], [5]]
# but under the hood, this is converted to something like this:
ArrayArray_(0, fn(@moved Array: int_): int_
    Array append(4)
    return Array
)
```

This function approach avoids lifetime issues that can arise with pointers,
because the function cannot escape with a reference to the element held
by the class instance.  In languages with pointers, like C++, you could
e.g. get a pointer to an element of an array, and later try to modify
the element via the pointer.  The element might not exist anymore
(e.g., the array was shrunk), and in the worst case, the array
might not even exist (e.g., the array was descoped).

In this way, ownership of another variable is very strict in hm-lang.
Only one object can modify the memory at some location (i.e., of a variable).
TODO: see if this is still true with functions modifying variables out of their scope...
We want to make it easy for lambda functions to capture variables in
the outer scope, which is important to do if those variables' lifetimes
end before that of the lambda function.

```
createLambda(Int;): fn(): int
    # without `@take`, the `Int` would not remain in scope
    return fn @take(Int;) ():
        Int += 1
        return Int
```

Ideally, `@take` would not be a built-in compiler annotation, but accomplishable
by other language features.  E.g., by creating a class that is callable.  For example,
the `@take` annotation would expand to something like this:

```
# TODO: this might be against hm-lang principles, see TODO below.
@private
hiddenClass := {
    @reset(Int; int)

    # TODO: maybe rename this method to `call()` instead of `()`:
    This;;(): int
        Int += 1
        return Int
}
createLambda(Int;): fn(): int
    return hiddenClass(Int move())
```

TODO: how does this work when the function is mutating itself (i.e., its scope)?
do we need to return a `fn(); int` type to indicate this?  or do we disallow
`fnClass`??

TODO: how to `take` a class instance to use its method

TODO: cleanup:
TODO: make sure people can't get around this by passing in a method
of an object as a function to some class instance's method, which
then goes on to keep a copy of that function as a lambda.
Do we need to ensure that people can't copy the function that gets
passed in?  As long as the only usage of the passed-in (e.g., inner) function
occurs when the outer function is called, then there should be no problem.

```
logger := {
    LogCount ;= 0

    ;;log(String): null
        ++LogCount
        print("${LogCount}: String")
}

announcer := {
    log(String); null   # mutable, lambda function

    # this indirection isn't strictly necessary;
    # we could set `log` directly since it's a public variable,
    # but this goes to show how the issue can be subtle.
    ;;use(New log(String): null): null
        log = New log

    begin(): null
        log("Begin!")
    end(): null
        log("End!")
}

main(WithLogger: bool): null
    Announcer ;= announcer()
    if WithLogger 
        Logger ;= logger()
        Announcer use(Logger log)
        # after this block ends, Logger goes out of scope;
        # so in something like C++ we'd destroy Logger
        # and Announcer would break when using log().
        # if we allow closures, then we need to pass in Logger as a shared pointer
        # to the generated lambda function `Logger log`, so that only once the
        # user resets `Announcer log` (or destroys Announcer) would the Logger get cleaned up.
        # ideally we would make it easy for a function to take responsibility for some
        # variables (i.e., via closure), but variables could theoretically be shared
        # between multiple lambda functions, so we'd be back to needing shared pointers.
    else
        Announcer use(log: print)
    Announcer begin()
    ... # do fancy logic
    Announcer end()
```

TODO: clean up:

What we could do is allow methods to be passed in as functions no problem, but mark
those lambda-method functions as special, e.g., with some metadata on whether the
function can be copied or not.  The methods that are taking the passed-in function
will also provide some metadata on whether the passed-in function will be copied or not.
(TODO: this needs to happen at the level of the function signature, since we can have
child classes that can override parent class methods, otherwise these are run-time errors.)
Then we would allow a lambda-method function to be passed in to some method `SomeThis;;useFunction()`
if the instance `OtherClass` backing the passed-in method `OtherThis;;usedMethod()`
outlived the `SomeClass` instance (i.e., the `OtherClass was defined before `SomeClass`,
and they are in the same scope).

# grammar/syntax

TODO: discussion on parentheses -- we'd like all to be equal () == {} == [], but
our notation might be difficult to distinguish arrays from arguments.

Note on terminology:

* Declaration: declaring a variable or function (and its type) but not defining its value

* Definition: declaration along with an assignment of the function or variable.

* `Identifier`: starts with an alphabetical character, can have numerical characters after that.
    Note that underscores are **not** permitted, since they are an operator.  

* `LowerCamelCase`: Identifier which starts with a lowercase alphabetical character.

* `UpperCamelCase`: Identifier which starts with an uppercase alphabetical character.

```
# list of elements that can compose the grammar.
# doesn't include stuff like LowerCamelCase or UpperCamelCase,
# which are not grammatically relevant.
grammarElement := enumerate(
    TypeMatcher
    VariableName
    VariableDeclaration
    VariableDefinition
    FunctionName
    FunctionDeclaration
    FunctionDefinition
    FunctionArgument
    FunctionCall
    RhsStatement
    AtomicStatement
    ClassName
    ClassDefinition
    EndOfInput
)

tokenMatcher := {
    @reset(Name: str = "")
    # TODO: tokenIterator needs to support putting back a token.
    match @mod(TokenIterator) (): bool
}

grammarMatcher := tokenMatcher | grammarElement | token

Grammar := singleton() {
    @private
    Elements: tokenMatcher_grammarElement = [
        TypeMatcher: TokenMatcher() #TODO
        VariableName: UpperCamelCase
        VariableDeclaration: sequence([
            VariableName
            oneOf([operator(":"), operator(";")])
            TypeMatcher
        ])
        VariableDefintion: oneOf([
            # VariableName: type = ...
            sequence([VariableDeclaration, operator("="), RhsStatement])
            # VariableName := ...
            sequence([VariableName, oneOf([operator(";="), operator(":=")]), RhsStatement])
        ])
        FunctionName: LowerCamelCase
        FunctionDeclaration: sequence([
            FunctionName
            list(FunctionArgument)
            oneOf([operator(":"), operator(";")])
            TypeMatcher
        ])
        FunctionDefinition: oneOf([
            # fnName(Args...): returnType
            #   BlockStatements
            sequence([FunctionDeclaration, Block])
            # fnName(Args...) := Statement
            sequence([
                FunctionName 
                list(FunctionArgument)
                oneOf([operator(":="), operator(";=")])
                RhsStatement
            ])
        ])
        FunctionArgument := oneOf([
            FunctionDefinition
            FunctionDeclaration
            VariableDefinition
            VariableDeclaration
        ])
        FunctionCall: sequence([FunctionName, AtomicStatement])
        RhsStatement: oneOf([
            AtomicStatement,
            sequence([AtomicStatement, AnyOperator, RhsStatement]),
        ])
        AtomicStatement: oneOf([
            VariableName
            FunctionCall
            parentheses(RhsStatement)
            list(DefinedArgument)
        ])
        ClassName: LowerCamelCase
        ExtendingParentClasses: sequence([
            keyword("extend")
            list(ClassName)
        ])
        ClassDefinition: sequence([
            ClassName
            optional(TemplateArguments)
            oneOf([
                operator(":=")
                doNotAllow(operator(";="), "Classes cannot be mutable.")
            ])
            optional(ExtendingParentClasses)
            parentheses(Block)
        ])
        EndOfInput: tokenMatcher(
            match @mod(TokenIterator) () := TokenIterator peak() == Null
        )
    ]

    match @mod(TokenIterator) (GrammarMatcher): bool
        consider GrammarMatcher Type
            case tokenMatcher
                return GrammarMatcher match @mod(TokenIterator) ()
            case grammarElement
                return Elements_GrammarMatcher match @mod(TokenIterator) ()
            case token
                if TokenIterator peak() == GrammarMatcher
                    TokenIterator next()
                    return True
                return False
}

# TODO: consider doing something Kotlin-y, where we could do
# match(...) : bool = consider
#   case ...
#       returnValue # without return statement
#   case ...
#       otherReturnValue # without return
# that might work well with the comma/newline approach,
# as well as ternary if/then/elif without new symbols/syntax

# TODO: actually compiling code will require going through the TokenMatchers
# in a specific order to avoid running through all options to see what fits.

# TODO: support for labeling token matchers, e.g. "parentClassNames" and "classBlock"


# a list encompasses things like (), (GrammarMatcher), (GrammarMatcher, GrammarMatcher), etc.,
# but also lists with newlines if properly tabbed.
list(GrammarMatcher) := parentheses(repeat(Until: EndOfInput, [
    GrammarMatcher
    CommaOrBlockNewline
])

sequence := extend(tokenMatcher) {
    # TODO: some annotation to pass a variable up to the parent class,
    # e.g., `reset(@passTo(TokenMatcher) Name: str, OtherArgs...):`
    ;;reset(Name: str, This Array: grammarMatcher_):
        tokenMatcher;;reset(Name)

    match @mod(TokenIterator) (): bool
        # TODO: probably have to take snapshots everywhere in most token matchers:
        Snapshot := TokenIterator snapshot()
        for GrammarMatcher: in Array
            if not Grammar match @mod(TokenIterator) (GrammarMatcher)
                TokenIterator restore(Snapshot)
                return False
        return True
}

# TODO: make `block` a type of token as well.
parentheses := extend(tokenMatcher) {
    ;;reset(Name: str, This GrammarMatcher):
        tokenMatcher;;reset(Name)

    match @mod(TokenIterator) (): bool
        Token := TokenIterator peak()
        consider Token Type
            case parenthesesToken
                if Token internallyMatches(GrammarMatcher) 
                    TokenIterator next()
                    return True
                return False
            default
                return False
}

repeat := extend(tokenMatcher) {
    # until `Until` is found, checks matches through `Array` repeatedly.
    # note that `Until` can be found at any point in the array;
    # i.e., breaking out of the array early (after finding `Until`) still counts as a match.
    # if you need to ensure a non-breakable sequence is found before `Until`,
    # use the `sequence` token matcher inside `Array`.
    ;;reset(Name: str, This Until: GrammarMatcher, This Array: GrammarMatcher_):
        tokenMatcher;;reset(Name)

    match @mod(TokenIterator) (): bool
        Snapshot := TokenIterator snapshot()
        while True
            for GrammarMatcher: in Array
                # always check the escape sequence, Until:
                if Grammar match @mod(TokenIterator) (Until)
                    return True
                if not Grammar match @mod(TokenIterator) (GrammarMatcher)
                    TokenIterator restore(Snapshot)
                    return False
}
# TODO: make sure the cyclic dependency is ok: i.e., Grammar match being called inside of
# these token matchers...

===
# e.g.
for Variable: variableType < UpperBoundExclusive
    ... use Variable from 0 to ceil(UpperBoundExclusive) - 1 ...
# or
for Variable: variableType <= UpperBoundInclusive
    ... use Variable from 0 to floor(UpperBoundInclusive) ...
# TODO: support starting value, or just variable names
Variable; variableType = 5
for @lock Variable < UpperBoundExclusive
    ... use Variable from 0 to ceil(UpperBoundExclusive) - 1 ...
# starting at number in the for loop
for Variable := StartingValue, Variable < UpperBoundExclusive
    ... use Variable from StartingValue ...
===
ForLoop := sequence([
    keyword("for")
    VariableDeclaration
    oneOf([operator("<"), operator("<=")])
    Expression
    Block
])

TODO: support internationalization.  do we really require Upper/lower+CamelCase for variables/functions?
or is the syntax unambiguous enough to not need them?

# tokenizer

TODO
