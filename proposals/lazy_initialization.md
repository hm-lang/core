# Lazy initialization

Some computations are more costly than others, and we'd like to
make it easy to perform a computation only when it's necessary.
We may potentially reserve `!` after a type to indicate that
the initialization is lazily performed.

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
