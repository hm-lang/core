# Return

Functions that produce output values typically have a `return`
statement.  In hm-lang, we have some specializations.

## Return if

If we return a value only if a certain condition is met, we can use
two equivalent ways:

```
INT FN doSomething(DBL x)
    if x > 0
        return 1
    else
        return 0

# using return if:
INT FN doSomething2(DBL x)
    return 1 if x > 0
    return 0
```

In another example, we can use "return if" to specify a default:

```
INT FN getDefault(INT?)
    if intQ != null
        return intQ # has the correct `INT` type
    return -1

# using return not-null:
INT FN getDefault(INT?)
    return intQ if intQ != null
    return -1
```
