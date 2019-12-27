# Types

Types are written in `UPPER_SNAKE_CASE` format, and require at least
two non-numeric characters.  This allows us to distinguish types from
`UpperCamelCase` casts (for specifications), which are allowed to be
a single (non-numeric) character.  E.g., `PK43` is a valid type,
whereas `X` is not a valid type (but would be a valid specification).

Primitive types are written using abbreviations, e.g. `INT` for integer
(specifically a 64-bit signed integer) and `DBL` for double.  Still,
primitive types can be considered as first-class citizens, i.e., they
can be inherited from, and they can still have methods defined on them.

Non-primitive types include things like `STRING` for an array of
characters, as well as classes created by the user.  It is highly
recommended to not use abbreviations (besides standard ones, e.g. HTML)
when defining a class name.

    | Identifier | Type description             |
    |------------|------------------------------|
    | BOOL       | boolean (1 bit unsigned)     |
    | DBL, FLT64 | 64-bit floating-point number |
    | FLT, FLT32 | 32-bit floating-point number |
    | INT, INT64 | 64-bit signed integer        |
    | INT32      | 32-bit signed integer        |
    | INT16      | 16-bit signed integer        |
    | INT8       | 8-bit signed integer         |
    | STRING     | array of characters/letters  |
    | UNS, UNS64 | 64-bit unsigned integer      |
    | UNS32      | 32-bit unsigned integer      |
    | UNS16      | 16-bit unsigned integer      |
    | UNS8       | 8-bit unsigned integer       |

Templated types can be created using the syntax `NEW_...`, where `...`
is any `UPPER_SNAKE_CASE` format, including a single character, e.g.
`NEW_T` or `NEW_TYPE`.  Declaring a variable with a certain type is
as easy as using this syntax:

```
    DBL x   # declare a variable `x` which is a `DBL`
    INT     # declare a variable `int` which is an `INT`
```

Variables that are declared are initialized (perhaps lazily),
either by default (to zero) or explicitly.

## Functions

Functions are declared using `FN` (or `MD` for a method inside a class).
An optional return type comes before this identifier, and the arguments
follow within parentheses.  Functions of course have their own types, 
which can be written out as the function declaration without the function
name.

```
    FN()    # declare function `fn` with no arguments and no outputs
    # -> type `FN()`

    # declare function `hello` which returns an integer,
    # and takes one string argument named `noun`:
    INT FN hello(STRING noun)
    # -> type `INT FN(STRING noun)`
```

## Arrays and map types

Because we use an explicit `FN` keyword to create functions, we have
special meanings for a type that is followed by parentheses.  In
hm-lang, all parentheses are equal: `() == {} == []`, but of course
they must match at each level.  If the parentheses are empty, an
array is being defined; if the parentheses include another type, a
map is being defined; and if a number is in the parentheses, a fixed
array is being defined.

```
    # declare a function named `doStuff` that takes a `STRING` argument
    INT FN doStuff(STRING)

    # declare an (automatically resizing) array named `values`;
    # equivalent to `INT ARRAY values`:
    INT[] values    

    # declare an array with a fixed size named `vector`;
    # equivalent to `DBL FIXED_ARRAY vector(Size(3))`
    DBL[3] vector

    # declare a hash map of strings to integers;
    # equivalent to `MAP Key(STRING) Value(INT) userIdMap`:
    INT[STRING] userIdMap

    # declare a hash map that has a key composed of two types,
    # one of which is named:
    DBL[STRING, INT priority] fancyMap
    fancyMap["jam", Priority(3)] = 1.234
    fancyMap["jam", Priority(1)] = 2.5
```

Note that the `MAP` type is actually an abstract class, so the exact
child class (e.g. `HASH_MAP` or `DLL_HASH_MAP`) that is used will
depend on a particular design choice made in hm-lang.  Likely we will
want to make the default `MAP` subclass as frustration-free as possible,
rather than as fast as possible.  I.e., we will likely use a
`DLL_HASH_MAP`, where the order of insertion will be reflected in the
order of iteration.
