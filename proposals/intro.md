# primitive types

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

The default function name is `fn`, although if you are naming your function, you
will not use `fn` when declaring/defining it.

# declaring and using primitive variables

Variables are named using `UpperCamelCase` identifiers.

```
# declaring a variable that holds a big integer
X: int

# X is default-initialized to 0 if not specified.
X += 5      # now X == 5 is True.
```

You can also initialize a variable inline.

```
# declaring and setting a variable that holds a big integer, starting with a value
Y: int = 5

# using the variable:
print(Y * 30)
```

You can also define variables which cannot be reassigned.

```
# declaring and setting a non-reassignable variable that holds a big integer
Z := 10     # or `Z := int(10)` if you want to be explicit

Z += 7      # COMPILER ERROR!! Z is non-reassignable.
```

# optional types

```
# TODO: do we want `X: int?` or `X: int|null`
```

# declaring, defining, and using a function

Functions are named using `lowerCamelCase` identifiers.

```
# declaring a function with no arguments that returns a big integer
v(): int

# setting/defining/initializing the function usually requires an indent+1 block following:
v(): int
    return 600

# function with X,Y double-precision float arguments that returns nothing
v(X: dbl, Y: dbl): null
    print("X = ${X}, Y = ${Y}, atan(Y, X) = ${atan(X, Y)}")

# function that takes a function as an argument and returns a function
wow(Input.fn(): string): fn(): int
    return fn(): int
        return Input.fn().size()
```

You can call functions with arguments in any order.  Arguments must be specified
with the named identifiers in the function definition.

```
# definition:
v(X: dbl, Y: dbl): null

# example calls:
v(X: 5.4, Y: 3)
v(Y: 3, Y: 5.4)

# if you already have variables X and Y, you don't need to re-specify their names:
X := 5.4
Y := 3
v(X, Y)     # preferred.  also ok: `v(X: X, Y: Y)` but not idiomatic.
v(Y, X)     # equivalent
```

## unnamed arguments in functions

For functions with a single argument where the variable name doesn't matter,
you can use "unnamed" variables.  For primitive types, the "unnamed" identifier
is just the UpperCamelCase version of the lowerCamelCase type.

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

## function overloads

TODO

## function templates/generics

TODO

# declaring and using a class

A class is defined with the `class` keyword with a `lowerCamelCase` identifier.
Class definitions must be constant/non-reassignable, so they are declared using
the `:=` symbols.

```
exampleClass := class(
    # class instance variables can be defined here:
    X: int

    # class methods can be defined as well:
    doSomething(Int): int
        return X + Int

    # classes must be resettable to a blank state, or to whatever is specified
    # as the starting value based on a `reset` function:
    reset(X: int)
        This.X = X

    # or short-hand: `reset(This.X: int)` will automatically set `This.X` to the passed in `X`.
)

Example: exampleClass = (X: 5)
print(Example.doSomething(7))   # should print 12
Example = exampleClass(X: 7)    # note: variable can be reassigned.
Example.X -= 3                  # internal fields can be reassigned as well.

ConstVar := exampleClass(X: 2)
ConstVar.X += 3                 # internal fields can be reassigned
ConstVar = exampleClass(X: 4)   # COMPILER ERROR! variable is non-reassignable.
```

## method overloads

TODO

## template methods

TODO

## generic/template classes

TODO
