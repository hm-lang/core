# Question mark operator

Normally used for optional types, e.g. `Int?`.  In C++ the `?` symbol is a conditional ternary operator,
but in hm-lang we use it for types and other statements which should execute only if an expression is
not null.

Used after an expression, we want to do something like this:

```
Int getDefault(Int?)
    return intQ?    # returns only if intQ is not null
    return 100      # default is 100
```

We probably want to do method chaining in the same way as Typescript:

```
String getId(Int?)
    String? id = intQ?.toString()
    if result != null
        return "ID$(id)"
    return "unknown"
```
