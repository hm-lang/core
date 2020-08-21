# Functions

We can create a function using a lowerCamelCase identifier followed by some
nested parentheses, and define it in the following indent+1 block:

```
hello(string Noun)
    print("hello, ${Noun}!")

hello(Noun("world"))
```

Note that any pair of matching parentheses works to declare (or call) a function, 
i.e. `fn()`, `fn[]`, and `fn{}` are equally valid.

## Function definitions

### Function declarations

Functions are declared with optional return value(s) in front, the function
name in lowerCamelCase, and the input argument(s) enclosed in parentheses:

```
fn();

Int fn(Dbl);

Dbl hello(String);

(int Index, dbl Value) get(string Key);

(Int, String) doStuff(Dbl, String);
```

Note the trailing semicolons are only necessary because we are not
initializing the function immediately.  Without the semicolon, we can
initialize the function with the following input.

### Simple function definitions

A function definition includes a declaration and the internal function logic
on an indent+1 block immediately following the declaration.

```
fn()
    print("hello there!")

Int main(String[] Args)
    print(Args[0])
    return 0
```

TODO: add one-line function definitions, maybe using `fn() := print("3")`

*Historical aside*:
In older versions of hm-lang, we thought it would be ok to define a function
on the same line using `fn(), print("single line function")` type syntax, but
this leads to some grammar ambiguities when passing in a function as an argument
to another function; is the next bit another passed-in argument or the default
definition of the function?


## Function inputs (arguments) and outputs (return values)

### Default initialization of return values

Note that in hm-lang, most types can be default initialized, e.g.
`int()` is the same as `0`.  This fact can be used when initializing
a function; the return variables will be automatically default
initialized if the function does not explicitly initialize them.

```
Int sign(Dbl)
    if Dbl < 0
        return -1
    elif Dbl > 0
        return 1
    # automatically returns 0
```

You can also initialize the return value with your own value/function:

```
(Index = -1) find(Int[], Int)
    index I = 0
    while I < IntS.size()
        if IntS[I] == Int
            return I
        ++I

Int[] Stack = [1234, 2, 1]
Index = find(Stack, 2)  # Index == 1 # 0-based indexing on arrays.
index OfOne = find(1, Stack)  # OfOne == 2, of course.
index X = find(-3, Stack)   # X == -1
```

### Default initialization of input values

Input values will be default initialized if not specified, or
initialized according to the value/function specified in the arguments.

```
say(string Greeting, string Noun = "World", int Times = 1)
    for Int < Times
        print("${Greeting}, ${Noun}!")

say(Greeting("Hello"), Times(2)) # prints "Hello, World!" twice
```

### Setting return variables by name

Because the return variables have identifiers, you can also refer to
them by name inside the function.

```
(Dbl, string Verb) get(Int)
    Dbl = Int * 1234.5
    if Dbl > 100
        Verb = "jump"
    elif Dbl > 0
        Verb = "walk"
    else
        Verb = "swim"
```

This is particularly helpful for things like arrays:

```
Int[] getRange(Size)
    for Index < Size
        # Note an unnamed `Int[]` type is referred to by `IntS`:
        IntS.append(Index)
```

You can also return multiple variables.  Any return values that are not specified
in the return statement will be whatever they were set to within the function body,
or the initialization provided (by default if necessary).

```
(Dbl, string Greeting, Index) doSomething()
    if getSomeBoolean()
        Index = 1234
    # else, index will be zero, by default.
    return (Greeting("Hello!"), 1.2345)
```

### Capturing named return values

Named return values can be captured in various ways:

```
(Dbl, string greeting, int Counter) doSomething();

dbl X           # this doesn't need to be renamed, return `dbl` is unnamed.
string Greeting # already named correctly
int Value       # not named correctly...
(X, Greeting, Counter(Value)) = doSomething()

# or alternatively, you can declare/init them inline:

(dbl X, string Greeting, int counter Value) = doSomething()
print(X) # ok, X and other variables are still in scope based on indent, not parentheses.
```

### Name-clashes between inputs and outputs

TODO

## const-ness and more examples

Functions are by default `const`, i.e. they cannot be changed.

```
hello()
    print("hello, world!")

# this errors:
hello = fn()
    print("hey, you!")
# ERROR!  functions are constant/non-mutable by default.
```

If you want to define a function which can change, use the 
[`var`](./var_vs_const.md) keyword.  See also
[lambda functions](./lambda_functions.md) helpful notation.

```
hello() var
    print("hello, world!")

hello = fn()
    print("hey, you!")
# ok!  even though this definition doesn't have a `var`,
# hello is still mutable, based on its original definition.

hello = fn() var
    print("hi, friend.")
# also ok.
```
