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
to indicate that `SOME_TYPE` should be deeply constant.  For example,
`INT? CONST` indicates a constant `INT MAYBE` type.  No one is allowed
to change the reference (the `MAYBE`), nor the `INT` value (if it exists).


## Container const-ness

Suppose we have an array of some other type.  We have a few
different ways we can use `CONST` (or not), and they each have
a different nuance:

Here we create an array that can be modified in any way, including
being reassigned:

```
INT ARRAY int_array = [10, 9, 8]
int_array.push(7)    # OK
int_array[2] = -1    # OK
int_array.shift()    # OK
int_array = [1, 2]   # OK
print(int_array[99]) # OK, resizes to 100 and gives a default INT
```

Here we create an array whose internal values can be modified,
but whose size is fixed, and which cannot be reassigned:

```
INT CONST_ARRAY int_constArray = [1, 2, 3, 4]
int_constArray.push(7)    # ERROR, cannot change size!
int_constArray[2] = -1    # OK, individual elements aren't CONST.
int_constArray.shift()    # ERROR, cannot change size!
int_constArray = [1, 2]   # ERROR, cannot reassign!
print(int_constArray[99]) # ERROR, cannot resize!
```

Here we create an array where the values can't be changed after
they are set.  However, because array modifications can be
performed, we can end up removing old values and inserting new
elements in the same spot.  We don't want to disallow moving
`CONST` types, since we would still have similar issues if people
popped from the end of the array and pushed new values.

```
CONST_INT ARRAY constInt_array = [1, 3, 5, 7]
constInt_array.push(7)    # OK
constInt_array[2] = -1    # ERROR, individual elements are CONST
constInt_array.shift()    # OK
constInt_array = [1, 2]   # OK
print(constInt_array[99]) # OK, resizes to 100 and gives a default INT
constInt_array[98] = 5    # OK, this is the first time it is assigned.
```

Here we create a deeply constant array; i.e. the values cannot be
modified and the array cannot be modified.

```
CONST_INT CONST_ARRAY constInt_constArray = [3, 2, 1]
constInt_constArray.push(7)    # ERROR
constInt_constArray[2] = -1    # ERROR
constInt_constArray.shift()    # ERROR
constInt_constArray = [1, 2]   # ERROR
print(constInt_constArray[99]) # ERROR
constInt_constArray[98] = 5    # ERROR
```

Note that this shorthand may be desirable for the deeply constant
version:

```
INT ARRAY CONST int_array_const = [-1, 5, 11]
```

The arguments above are similar in case the contained type
is some non-primitive type, e.g. `class VECTOR2(DBL x, DBL y);`.
In the case that the contained type is `CONST`, then you cannot
reassign or change the instance, e.g. `vector2.x = 3`.
