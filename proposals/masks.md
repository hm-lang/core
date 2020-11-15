# Masks

Masks are generalized from [enumerations](./enumerations.md), and allow multiple
values to be held simultaneously.  Each value can also be thought of as a flag or option,
which are `or`d together (`|`) for the variable instance.  Under the hood, these are
unsigned integer types, `uint`.  Trying to assign a negative value will throw a compiler error.

```
# the mask is required to have types that are powers of two, except for 0:
mask NonMutuallyExclusiveType
    None = 0
    X = 1
    Y = 2
    Z = 4
    T = 8

# has all the same static methods as enum, though perhaps they are a bit surprising:
nonMutuallyExclusiveType.count() == 16
nonMutuallyExclusiveType.min() == 0
nonMutuallyExclusiveType.max() == 15    # = X | Y | Z | T

nonMutuallyExclusiveType Options
Options |= X
Options |= nonMutuallyExclusiveType.Z

Options.hasX()  # True 
Options.hasY()  # False
Options.hasZ()  # True
Options.hasT()  # False
Options.isX()   # False, since Options is a combo of X and Z

Options = T
Options.isT()   # True, since Options is now solely T

Options = None | X
Options.hasNone()   # False
Options.hasX()      # True
```

The first flag can be zero, but note that this is the one option that cannot be set alongside
any other option; i.e., `None | X` below will resolve to `X`, and similarly for any non-zero
value.
