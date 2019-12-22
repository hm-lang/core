# Function keywords

We want to make it easy to create lambda functions, e.g. for testing:

```
    TEST someBehavior(FN()
        INT x = 3
        INT y = 12
        assert(x * y == 36, OrThrow "That was unexpected")
    )
```

where `FN()` creates a new function based on the following indent+1 block.

## Concerns for keywords: static, member, standard functions, and modifiable ones

We may want to name class member functions to something besides `FN`, e.g. `MFN`
(for member function).  This may align well with future additions, e.g. 
`SFN` for static function.  We might prefer `CLASS_FN` for a static function, 
since 'static' doesn't seem descriptive.  Or even just `CLASS_FN` -> `FN`,
since only a `MFN` would have access to instance variables.

Ideally, we would use something like `FN` for non-modifiable functions,
e.g. `FN` is an alias/macro for a const version of an underlying function
type -- i.e. `CONST_FUN`.  The `FUN` type would be modifiable.  If we
can't think of a good distinction, `FUN` should maybe be equivalent to
`LFN`.  That is, a function (`FN`) is just a lambda function (`LFN`)
which cannot be changed (`TYPE FN = CONST_LFN`).

```
    #
    # define a class with a lambda function which cannot be modified:
    #
    CLASS FN_CLASS(INT FN this.hash(STRING));

    # instantiate the class
    FN_CLASS f(INT FN hash(STRING)
        ...
        return 7
    )

    # ERROR!  this fails, since hash is a constant function, `FN`.
    f.hash = INT FN(STRING)
        ...
        return 3

    #
    # define a class with a modifiable lambda function:
    #
    CLASS LAMBDA_CLASS(INT LFN this.hash(STRING));

    # instantiate the class
    LAMBDA_CLASS l(INT LFN hash(STRING)
        ...
        return 5
    )

    # this is ok:
    l.hash = INT LFN(STRING)
        ...
        return 3

    #
    # define a class with a hash method:
    #
    CLASS METHOD_CLASS(INT this.value)
        INT MFN hash(STRING)
            return value * string.size

    # instantiate:
    METHOD_CLASS m(3)

    # ERROR!  this fails, since hash is a member function, 
    # i.e. class method.
    m.hash = INT FN(STRING)
        return 1234
```
