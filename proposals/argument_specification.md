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
   value you want to use for that variable.  As a small variation, you can
   use `Identifier=...`

3. For an unnamed argument, create a variable on the fly using its 
   `TYPE` cast.  E.g. `INT(1.234)` to create an integer from a double.

Note that in all cases, order doesn't matter when specifying a list 
of arguments!  Examples:

* For the argument list `(STRING greeting, STRING)`

```
    STRING noun = "world!"
    (Greeting("hello"), noun)   # specifies the arguments

    (Greeting="hi", "friend")   # also ok.

    STRING greeting = "hey"
    (greeting, "dude")          # also valid specification

    ("grandparents", greeting)  # also valid.  order doesn't matter.
```

* For an argument list `(INT, DBL, STRING)`:

```
    INT x = 4
    DBL = 3.4       # dbl is unnamed
    STRING = "hey"  # string is unnamed

    ("hashtag", x, dbl)     # specifies the arguments

    (Int(x), string, dbl)   # also ok, a bit redundant (x is already an INT), but ok.

    (1.2345, INT(123.54), "pancakes")     # also valid specification

    (27, 64, "cubes")    # ERROR!  we don't know what is the `DBL`.
```

## Unused/deleted arguments

TODO:
Consider allowing an `unused` or `deleted` token to indicate that you
dynamically don't want to include an argument.  This is only useful if
the function is overloaded, and you specifically want to target a different
overload.  Otherwise, it's the same as passing null.

```
    FISH[] FN getFish(INT count, STRING reason = "too lazy to think of one");

    INT count = 2
    # The next line will set `reason` in the `getFish` function to the provided default,
    # i.e., "too lazy to think of one", since there is no overload for `getFish(INT count)`:
    FISH LIST = getFish(count, Reason(count < 5 ? deleted : "need lots of fish"))

    # Since there is no overload, you can also replace `deleted` with `null`, and it
    # will do the same thing in the function (i.e., set `reason` to the default string):
    FISH LIST list2 = getFish(count, Reason(count < 5 ? null : "need lots of fish"))
```

On the other hand, if you have an overload, you can get different behavior:

```
    CAT FN getCat(INT whiskers);
    CAT FN getCat(INT whiskers, DBL weight);

    INT whiskers = 5
    # Dynamically determines whether to call the first function or the second:
    CAT = getCat(whiskers, Weight(whiskers < 10 ? deleted : whiskers * 2.0))

    # This always calls the second function, but puts in the default value,
    # which in this case is the default for type `DBL`, i.e. 0.0
    CAT cat2 = getCat(whiskers, Weight(null))
```

This potentially makes the code slower, since we can't just use compile-time logic
to determine which overload to call, with what arguments.  We would need run-time
logic to determine if the arguments were deleted (i.e., we'd need another sentinel
value besides null), and determine the proper overload to call.  But it might make
the language more cohesive.

We would likely need to make function specifications first-class citizens -- probably
specialized structs, complete with their own subclasses.  It might make sense to invert
control a little bit:

```
struct getCat_ARGUMENTS_base {
    virtual Scoped<Cat> getCat(CallingClass *caller) {
        // first function overload
        return caller->getCat(0); // whiskers = 0
    }
};

struct getCat_MIXIN_whiskers {
    int whiskers;
};

struct getCat_MIXIN_weight {
    double weight;
};

struct getCat_ARGUMENTS_whiskers
        : public getCat_ARGUMENTS_base,
          public getCat_MIXIN_whiskers {
    virtual Scoped<Cat> getCat(CallingClass *caller) {
        // first function overload
        return caller->getCat(whiskers);
    }
};

struct getCat_ARGUMENTS_weight
        : public getCat_ARGUMENTS_base,
          public getCat_MIXIN_weight {
    virtual Scoped<Cat> getCat(CallingClass *caller) {
        // second function overload
        return caller->getCat(weight, /*whiskers*/ 0);
    }
};

struct getCat_ARGUMENTS_weight_whiskers
        : public getCat_ARGUMENTS_base,
          public getCat_MIXIN_weight,
          public getCat_MIXIN_whiskers {
    virtual Scoped<Cat> getCat(CallingClass *caller) {
        // second function overload
        return caller->getCat(weight, whiskers);
    }
};

// So that deleting an argument would be a forceful conversion
// a child-class to the parent class:

getCat_ARGUMENTS_weight_whiskers args;
// someone decides to delete weight, make a forced truncation:
getCat_ARGUMENTS_whiskers newArgs = std::move(args);
// use newArgs instead of args in function call.
```

Or there might be a better way.

*Background:*
This is essentially what I wish `undefined` was in Javascript; i.e., a Javascript object
`{someKey: undefined}` would in an ideal world be exactly equal to an empty object, `{}`,
without any key/values defined.
