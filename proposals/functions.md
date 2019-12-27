# Functions

Functions are declared using the `FN` keyword, or `MD` for class methods.
The return type comes before `FN`, an optional function name follows,
and the function arguments follow in parentheses.  Examples:

```
    FN();

    INT FN(DBL);

    DBL FN hello(STRING);

    (INT index, DBL value) FN get(STRING key);

    (INT, STRING) FN(DBL, STRING);
```

Note the trailing semicolons are only necessary because we are not
initializing the function immediately.  Without the semicolon, we can
initialize the function by creating an indent+1 block immediately
following.  For example:

```
    FN()
        print("hello there!")

    INT FN main(STRING[] args)
        print(args[0])
        return 0
```

## Default initialization of return values

Note that in hm-lang, most types can be default initialized, e.g.
`INT()` is the same as `0`.  This fact can be used when initializing
a function; the return variables will be automatically default
initialized if the function does not explicitly initialize them.

```
    INT FN sign(DBL)
        if dbl < 0
            return -1
        elif dbl > 0
            return 1
        # automatically returns 0
```

You can also initialize the return value with your own value/function:

```
    (INDEX index = -1) FN find(INT ARRAY, INT)
        INDEX i = 0
        while i < int_array.size
            if int_array[i] == int
                return Index(i)
            ++i
```

## Default initialization of input values

Input values will be default initialized if not specified, or
initialized according to the value/function specified in the arguments.

```
    FN say(STRING greeting, STRING noun = "World", INT times = 1)
        for INT < times
            print("${greeting}, ${noun}!")

    say(Greeting("Hello"), Times(2)) # prints "Hello, World!" twice
```

## Setting the return variables instead of explicitly returning them

Because the return variables have identifiers, you can also refer to
them by name inside the function.

```
    (DBL, STRING verb) get(INT)
        dbl = int * 1234.5
        if dbl > 100
            verb = "jump"
        elif dbl > 0
            verb = "walk"
        else
            verb = "swim"
```

This is particularly helpful for things like arrays:

```
    INT ARRAY getRange(SIZE)
        for INT index < size
            int_array.push(index)
```

You can also return multiple variables.  Any that are not specified,
can be default initialized if not already initialized in the function.

```
    (DBL, STRING greeting, INT index) doSomething()
        if getSomeBoolean()
            index = 1234
        # else, index will be zero, by default.
        return (Greeting("Hello!"), 1.2345)
```

## Capturing return values

Return values can be captured in various ways:

```
    (DBL, STRING greeting, INT index) doSomething();

    DBL x
    STRING greeting
    INT value
    (x, greeting, Index(value)) = doSomething()

    # or alternatively, you can declare/init them inline:

    INT value
    (DBL x, STRING greeting, Index(value)) = doSomething()
```
