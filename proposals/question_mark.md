# Question mark operator

Normally used for optional types, e.g. `Int?`.  In C++ the `?` symbol is a conditional ternary operator,
but in hm-lang we use it for types, and it can be the last part of an identifier.

```
Int getDefault(Int?)
    return Int? if Int? != Null
    return 100      # default is 100
```

We probably want to do method chaining in the same way as Typescript:

```
String getId(Int?)
    string? Id = Int?.toString()
    if result != Null
        return "ID$(Id)"
    return "unknown"
```
