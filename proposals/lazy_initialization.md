# Lazy initialization

Some computations are more costly than others, and we'd like to
make it easy to perform a computation only when it's necessary.
We reserve `!` after a type to indicate that the initialization is lazily performed.

```
Dbl someFunction()
    dbl X = 1234
    int! Complicated = someComplicatedComputation(X)
    if Q == 1
        print(Complicated)
    elif Q == 2
        X = -345.6
        # will run `someComplicatedComputation` with the new `X` value
        print(Complicated)

    # will only calculate this once in total, in this whole function:
    return Complicated
```

The initialization of the variable is only performed once, so
`someComplicatedComputation` is only called once.  To do this,
the RHS of the lazy expression becomes a lambda function, which
captures any relevant variables.  That is why any updates to 
input values will be reflected in the lazy-initialized variable,
as long as the inputs are updated before the lazy-initialized
variable is first used.

We also allow short-circuiting the lazy initialization:

```
dbl! X = someComplicatedComputation()
if Q
    # does not perform `someComplicatedComputation()`
    X = 3
print(X)
```

The spec ensures that what appears to be a reassignment never 
triggers the original lazy initialization.  (Unless of course,
it is a get-then-set type operation, e.g. `+=`.)

## Usage in function arguments and return values

Lazy initialization *might* be used internally to define default
input arguments, and/or short-circuit their initialization,
in the case that their calculation is complicated:
[TODO: revisit -- we don't want pointer-like arguments unless specified]

```
doSomething(int X = somethingComplicated(), dbl Z)
    if Z > 0
        # `somethingComplicated` will never trigger, even
        # if `X` was not defined in the calling arguments.
        X = 0
        print Warning("ignoring original value for X.")
    doOtherThing(X, Z)
```

Also for return values:

```
(Dbl = complicatedExpression(), String) doThings(int Times)
    if Times <= 0
        # ensures `complicatedExpression()` is not evaluated:
        Dbl = 0.0
        String = "invalid!"
        return
    else
        Dbl *= Times # get-then-set operation.
        String = "it was performed"
```

## Internal implementation

The lazy initialized variable will probably be some extension
of a `gs` (getter-setter) class, where the getter will automatically
invoke the original initialization definition, if nothing has
overridden it by using the setter.

In the underlying code, the `lazy` class is essentially a C++ `std::unique_ptr`,
but with the added feature that if the pointer is null (uninitialized) when asked for,
it will either initialize to a default instance of the wrapped type, or it will use
the initialization provided at its declaration.
