# Arguments and specifications

## Arguments

A list of arguments (either for input values or return values) consist
of unnamed and/or named variables; there can only be one unnamed variable
of any given type.  An unnamed variable is either a variable with its
name ommitted, or a variable whose type is the same as its identifier
(ignoring trailing or preceding underscores in the identifier).  Each
argument has a type and an optional identifier.  Some examples:

* `(INT index, INT)` - An argument list consisting of an unnamed `INT`
  (which can be accessed using the identifier `int`) and a named `INT`
  (which can be accessed using the identifier `index`).

* `(INT, DBL dbl, STRING string_)` - argument list for an unnamed `INT` 
  (accessible via the identifier `int`), an unnamed `DBL` (accessible via
  `dbl`, though this is a bit redundant), and an unnamed `STRING` 
  (accessible via `string_` -- presumably so identified to avoid a name
  conflict in the parent scope, but this is still considered unnamed).

* `(INT, STRING greeting, INT)` - ERROR!  This argument list is invalid,
  since it has two unnamed variables of the same type (i.e., `INT`).

* `(DBL x, DBL y)` - An argument list consisting of two `DBL`s, one named
  `x` and the other `y`.

* `(STRING x, INT x)` - ERROR!  Each argument needs to have a unique
  identifier.

By insisting on unique identifiers for variables we can do cool things
when specifying a list of arguments.  In hm-lang, there is no such thing
as positional arguments; they are all keyword based.


## Specifications

When calling a function (or getting its return values), we do not require
the order of the specification to match that of the argument list.
Instead, we specify arguments using keywords, and there are advanced ways
of doing this (for future work), but here are some sufficiently simple ways 
of making a specification:

1. Use a variable with the same identifier (ignoring trailing/leading
   underscores) as the one in the argument list.

2. Use an `UpperCamelCase` cast, i.e. `Identifier(...)` where `Identifier`
   is the correct keyword identifier for the argument, and `...` is the
   value you want to use for that variable.

3. For an unnamed argument, create a variable on the fly using its 
   `TYPE` cast.  E.g. `INT(1.234)` to create an integer from a double.

Note that in all cases, order doesn't matter when specifying a list 
of arguments!  Examples:

* For the argument list `(STRING greeting, STRING)`

```
    STRING noun = "world!"
    (Greeting("hello"), noun)   # specifies the arguments

    STRING greeting = "hey"
    (greeting, "dude")          # also valid specification

    ("grandparents", greeting)  # also valid.  order doesn't matter.
```

* For an argument list `(INT, DBL, STRING)`:

```
    INT x = 4
    DBL dbl__ = 3.4 # dbl__ is unnamed
    STRING = "hey"  # string is unnamed

    (Int(x), string, dbl__)    # specifies the arguments

    (1.2345, INT(123.54), "pancakes")     # also valid specification

    (27, 64, "cubes")    # ERROR!  we don't know what is the `DBL`.
```
