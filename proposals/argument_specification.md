# Arguments and specifications

## Arguments

A list of arguments (either for input values or return values) consist
of unnamed and/or named variables; there can only be one unnamed variable
of any given type.  An unnamed variable is either a variable with its
name ommitted, or a variable whose type is the same as its identifier
(ignoring trailing or preceding underscores in the identifier).  Each
argument has a type and an optional identifier.  Some examples:

* `(int Index, Int)` - An argument list consisting of an unnamed `int`
  (which can be accessed using the identifier `Int`) and a named `int`
  (which can be accessed using the identifier `Index`).

* `(Int, dbl Dbl, string New.String)` - argument list for an unnamed `int` 
  (accessible via the identifier `Int`), an unnamed `dbl` (accessible via
  `Dbl`, though this is redundant/discouraged), and an unnamed `string` 
  (accessible via `New.String` -- presumably so identified to avoid a name
  conflict in the parent scope, but this is still considered unnamed).

* `(Int, string Greeting, Int)` - ERROR!  This argument list is invalid,
  since it has two unnamed variables of the same type (i.e., `int`).

* `(dbl X, dbl Y)` - An argument list consisting of two `dbl`s, one named
  `X` and the other `Y`.

* `(string X, int X)` - ERROR!  Each argument needs to have a unique
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

1. Use a variable with the same identifier (ignoring `New.` or `Old.`
   prefixes and trailing/leading underscores) as the one in the argument list.

2. Use an `UpperCamelCase` cast, i.e. `Identifier(...)` where `Identifier`
   is the correct keyword identifier for the argument, and `...` is the
   value you want to use for that variable.  As a small variation, you can
   use `Identifier=...`

3. For an unnamed argument, create a variable on the fly using its 
   `type` cast.  E.g. `int(1.234)` to create an integer from a double.

Note that in all cases, order doesn't matter when specifying a list 
of arguments!  Examples:

* For the argument list `(string Greeting, String)`

```
    string Noun = "world!"
    (Greeting("hello"), Noun)   # specifies the arguments

    (Greeting="hi", "friend")   # also ok.

    string Greeting = "hey"
    (Greeting, "dude")          # also valid specification

    ("grandparents", Greeting)  # also valid.  order doesn't matter.
```

* For an argument list `(Int, Dbl, String)`:

```
    int X = 4
    Dbl = 3.4       # Dbl is an unnamed dbl
    String = "hey"  # String is unnamed

    ("hashtag", X, Dbl)     # specifies the arguments

    (int(X), String, Dbl)   # also ok, a bit redundant (x is already an INT), but ok.

    (1.2345, int(123.54), "pancakes")     # also valid specification

    (27, 64, "cubes")    # ERROR!  we don't know what is the `DBL`.
```

## Unspecified arguments

TODO:
Consider allowing an `Unspecified` token to indicate that you
dynamically don't want to include an argument.  This is only useful if
the function is overloaded, and you specifically want to target a different
overload.  Otherwise, it's the same as passing null.

```
    Fish[] getFish(int Count, string Reason = "too lazy to think of one");

    int Count = 2
    # The next line will set `Reason` in the `getFish` function to the provided default,
    # i.e., "too lazy to think of one", since there is no overload for `getFish(int Count)`:
    Fish[] list1 = getFish(Count, Reason(Unspecified if Count < 5 else "need lots of fish"))

    # Since there is no overload, you can also replace `Unspecified` with `Null`, and it
    # will do the same thing in the function (i.e., set `Reason` to the default string):
    Fish[] list2 = getFish(Count, Reason(Null if Count < 5 else "need lots of fish"))
```

On the other hand, if you have an overload, you can get different behavior:

```
    Cat getCat(int Whiskers);
    Cat getCat(int Whiskers, dbl Weight);

    int Whiskers = 5
    # Dynamically determines whether to call the first function or the second:
    Cat = getCat(Whiskers, Weight(Unspecified if Whiskers < 10 else Whiskers * 2.0))

    # This always calls the second function, but puts in the default value,
    # which in this case is the default for type `DBL`, i.e. 0.0
    Cat cat2 = getCat(Whiskers, Weight(Null))
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
struct __getCat_specification_base {
    virtual scoped<cat> getCat() {
        // first function overload
        return getCat_Whiskers(0); // whiskers = 0
    }

    scoped<cat> getCat_Whiskers(int Whiskers) { ... }

    scoped<cat> getCat_Weight_Whiskers(double Weight, int Whiskers) { ... }
};

struct __mixin__int__Whiskers {
    int Whiskers;
};

struct __mixin__double__Weight {
    double Weight;
};

struct __getCat_specification_Whiskers
        : public __getCat_specification_base,
          public __mixin__int__Whiskers {
    virtual scoped<cat> getCat() {
        return getCat_Whiskers(Whiskers);
    }
};

struct __getCat_specification_Weight
        : public __getCat_specification_base,
          public __mixin__double__Weight {
    virtual scoped<cat> getCat() {
        return getCat_Weight_Whiskers(Weight, 0);
    }
};

struct __getCat_specification_Weight_Whiskers
        : public __getCat_specification_base,
          public __mixin__double__Weight,
          public __mixin__int__Whiskers {
    virtual scoped<cat> getCat() {
        return getCat_Weight_Whiskers(Weight, Whiskers);
    }
};
```

Or there might be a better way.

*Background:*
This is essentially what I wish `undefined` was in Javascript; i.e., a Javascript object
`{someKey: undefined}` would in an ideal world be exactly equal to an empty object, `{}`,
without any key/values defined.
