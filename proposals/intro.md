# primitive types

Standard types whose instances can take up an arbitrary amount of memory:

* `int`: signed big-integer
* `rtl`: rational number (e.g. an `int` over an `int`)
* `str`: array/sequence of utf8 bytes, but note that `string` is preferred in code
    since it includes other containers which deterministically provide utf8 bytes.

Other types which have a fixed amount of memory:

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

You can also define variables which cannot change.

```
# declaring and setting a constant variable that holds a big integer
Z := 10     # or `Z := int(10)` if you want to be explicit

Z += 7      # COMPILER ERROR!! Z is constant.
```

# declaring and defining a function

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
wow(fn(): string): fn(): int
    return fn(): int
        return Args.fn().size()
```
