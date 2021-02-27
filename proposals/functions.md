# Functions

We can create a function using a lowerCamelCase identifier followed by some
nested parentheses, and define it in the following indent+1 block:

```
hello(string Noun)
    print("hello, ${Noun}!")

hello(Noun:"world")
```

Note that any pair of matching parentheses works to declare (or call) a function,
i.e. `fn()`, `fn[]`, and `fn{}` are equally valid.
TODO: check if this is true again.

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

TODO: add one-line function definitions, maybe using `fn() = print("3")`


## Function inputs and outputs

Function inputs (arguments) and outputs (return values) follow the
[argument specification](./argument_specification.md) for the most part:  the
only exception is that the return type can be omitted for a void-returning function.


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
(Index = -1) find(Int[] In, Int)
    index I = 0
    while I < In.size()
        if In[I] == Int
            return I
        ++I

Int[] Stack = [1234, 2, 1]
Index = find(In: Stack, 2)  # Index == 1 # 0-based indexing on arrays.
index OfOne = find(1, In: Stack)  # OfOne == 2, of course.
index X = find(-3, In: Stack)   # X == -1
```

### Default initialization of input values

Input values will be default initialized if not specified, or
initialized according to the value/function specified in the arguments.

```
say(string Greeting, string Noun = "World", int Times = 1)
    for Int < Times
        print("${Greeting}, ${Noun}!")

say(Greeting: "Hello", Times: 2) # prints "Hello, World!" twice
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

This is particularly helpful for return types like arrays, which are
default initialized for you:

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
    return (Greeting: "Hello!", 1.2345)
```

### Capturing named return values

Named return values can be captured in various ways:

```
(Dbl, string Greeting, int Counter) doSomething();

dbl X           # this doesn't need to be renamed, return `dbl` is unnamed.
string Greeting # already named correctly
int Value       # not named correctly...
(X, Greeting, Counter: Value) = doSomething()

# or alternatively, you can declare/init them inline:

(dbl X, string Greeting, Int counter Value) = doSomething()
print(X) # ok, X and other variables are still in scope based on indent, not parentheses.
```

### Name-clashes between inputs and outputs

TODO

## Miscellaneous

### const-ness and more examples

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

### Making parentheses optional

Some hm-lang built-in functions, e.g. `return` and `print`, do not require parentheses.
It would be fun to do this for all functions.  It might not be grammatically pleasant
to allow this for function definitions, but it would be nice to do it for function calls.

For simple functions it doesn't seem too tricky:

```
# defining the function:
Int hello(String)
    print "hello, ${String}!"
    return String.size()

# calling the function:
hello "world"   # equivalent to `hello("world")` or `hello String:"world"` or `hello(String:"world")`
```

For void functions, we still require parentheses, since it's ambiguous with passing a function:

```
# defining the function:
hello()
    print "hello, world!"

# calling the function:
hello()     # OK
# TODO: check if ok??
hello       # also works, you can't specifically pass function pointers around
```

For functions with multiple return values, we always require parentheses around them:

```
# declaring the function:
(Int, String, dbl Value) doSomething();

# calling the function:
(Int, String, dbl Value) = doSomething()
print "got Int = ${Int} for $${String} and $${Value}"
```

For functions with multiple arguments:

```
# declaring the function:
doSomething(Int, String, dbl Value);

# calling the function, these are all equivalent:
doSomething Int: 3, String: "Hello", Value: 3.4

doSomething "Hello", Value: 3.4, 3

doSomething(Int: 3, String: "Hello", Value: 3.4)

doSomething(
    String: "Hello"
    3
    Value: 3.4
)
```

May need to have an "Argument stack" (or blob) in the grammar.
