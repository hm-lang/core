# Question mark operator

Normally used for optional types, e.g. `Int?`.  In C++ the `?` symbol is a conditional ternary operator,
but in hm-lang we use it for types, and it can be the last part of an identifier.
TODO: see if we want to convert it to a Q, or if not, remove all Q usage.

```
Int getDefault(Int?)
    return Int? if Int? != Null
    return 100      # default is 100
```

We probably want to do method chaining in the same way as Typescript:

```
String getId(Int?)
    string? Id = Int?.string()
    if result != Null
        return "ID${Id}"
    return "unknown"
```

TODO: maybe automatically check for existence when using ? unless we already know it's non-Null.


## Ternary operator: inline if-else

We can do an inline if-else statement like this:

```
int X = 5 if someCondition() else 3
```

We can have a multi-line statement as well (if we indent by two or use parentheses):

```
int Quibble = 4
        if someCondition() else
        1000

int Quandary = (
    300 if someOtherCondition() else
    1000
)
```

We can also have multiple ternary operators; the first condition to match will
stop firing off other conditions.

```
int Z

# indent by two to allow statement to continue after first line:
int Extra =
        5 if Z < 0 else
        4 if Z < 3 else
        1000

# or use parentheses:
int Ok = (
    -5 if Z < -5 else
    -4 if Z < 0 else
    1000 if Z < 5 else
    100000
)
```

Although not recommended, you can use the ternary operator in a nested fashion:

```
dbl Nested = 4 if Z < 100 else
        100 * (3 if Y < 0 else 4) if Z < 1000 else
        5 * (10 if X > 0 else 1)
```
