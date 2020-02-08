# Constant vs. mutable types

Variables which cannot be modified have `CONST` (constant) types.
In a class, note that `CONST` variables are allowed to be assigned
or initialized once, i.e., in the constructor, but can never be
reassigned/modified (even in the constructor).  Constant types
are preceded with `CONST_` concatenated into the type, e.g.
`CONST_INT` to indicate a constant `INT` variable.  Variables which
can be modified are technically `VAR` types (*variable* in the
adjective sense), but this is the default for non-functions:

```
INT x = 3

x = 4   # OK

CONST_STRING y = "hello!"

y = "oops!" # ERROR! `y` cannot be reassigned.
```

Functions are `CONST` by default, so they require a `VAR` in front
if they are allowed to be modified:

```
INT FN doSomething(INT)
    return int + 5

doSomething = $int + 3  # ERROR! `doSomething` cannot be reassigned.

INT VAR_FN doAnotherThing(STRING)
    return string.size

doAnotherThing = $string.size + 1   # OK
```

See [function keywords](./function_keywords.md) for some extra info
in this regard.


## Auto types

We may want to allow setting variables where we know the return type
automatically:

```
CONST x = 5     # x type is inferred to be CONST_INT

VAR y = 1.2     # y type is inferred to be VAR_DBL
```

This can backfire if the default return-type of an overloaded function
(i.e., the first overload that matches your function signature) is
not what you expect.

## Advanced type: Deeply constant

In case of a more complicated type, we use the syntax `SOME_TYPE CONST`
to indicate that `SOME_TYPE` should be constant.  For example,
`INT? CONST` indicates a constant `INT MAYBE` type.  No one is allowed
to change the reference (the `MAYBE`), nor the `INT` value (if it exists).
