# Lazy initialization

Some computations are more costly than others, and we'd like to
make it easy to perform a computation only when it's necessary.
We reserve `!` after a type to indicate that the initialization is lazily performed.

```
    DBL someFunction()
        DBL x = 1234
        INT! complicated = someComplicatedComputation(x)
        if q == 1
            print(complicated)
        elif q == 2
            x = -345.6
            # will run `someComplicatedComputation` with the new `x` value
            print(complicated)

        # will only calculate this once in total, in this whole function:
        return complicated
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
    DBL! x = someComplicatedComputation()
    if q
        # do not perform `someComplicatedComputation()`
        x = 3
    print(x)
```

The spec ensures that what appears to be a reassignment never 
triggers the original lazy initialization.

## Usage in function arguments and return values

Lazy initialization will be used internally to define default
input arguments, and/or short-circuit their initialization,
in the case that their calculation is complicated:
[TODO: revisit -- we don't want pointer-like arguments unless specified]

```
    FN doSomething(INT x = somethingComplicated(), DBL z)
        if z > 0
            # `somethingComplicated` will never trigger, even
            # if `x` was not defined in the calling arguments.
            x = 0
            print Warning("ignoring original value for x.")
        doOtherThing(x, z)
```

Also for return values:

```
    (DBL = complicatedExpression(), STRING) FN doThings(INT times)
        if times <= 0
            # ensures `complicatedExpression()` is not evaluated:
            dbl = 0.0
            string = "invalid!"
            return
        else
            dbl *= times
            string = "it was performed"
```

## Internal implementation

The lazy initialized variable will probably be some extension
of a `GS` (getter-setter) class, where the getter will automatically
invoke the original initialization definition, if nothing has
overridden it by using the setter.

In the underlying code, the `LAZY` class is essentially a C++ `std::unique_ptr`,
but with the added feature that if the pointer is null (uninitialized) when asked for,
it will either initialize to a default instance of the wrapped type, or it will use
the initialization provided at its declaration.
