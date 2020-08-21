# Lambda functions and the `$` operator

The `$` operator is special in hm-lang, as it is a way to create
lambda functions.  It has many different incarnations.

## `$` prefixing an identifier

We can create lambda functions with arguments (yay!) very easily using
this syntax.  The arguments are prefixed with `$`.  Here is an example
where we take an iterator and use it to create a list.

```
# creates an iterator that goes from 0 to the passed in `int` (minus 1).
# TODO: closures won't work off the bat correctly, this needs to be a class.
Int Iterator range(Int)
    int Current = 0
    return Int? next()
        return Null if Current >= Int
        return Current++

Int[] X = range(10).map($Int * 2 + 1)
# also equivalent:
# Int array X = range(10).map(Int fn(Int)
#     return Int * 2 + 1
# )
print(X) # [1, 3, 5, 7, 9, 11, 13, 15, 17, 19]

# can convert to a different type as well.
Dbl array Y = range(5).map($Int * 0.5)
print(Y) # [0.0, 0.5, 1.0, 1.5, 2.0]
```

It's easy to extend this to multiple variables.  For example, here's
a callback with multiple arguments:

```
runSomething(Dbl fn(dbl Multiplier, Dbl))
    print("Multiplier=1.5, Dbl=0.3: ${fn(Multiplier(1.5), 0.3)}")

runSomething($Multiplier * ($Dbl + 1))
```

## `$` prefixing a single function/method without arguments

TODO: ensure this doesn't grammatically interfere with `$` arguments above.

We can make a lambda function by prefixing a single function or
method call with a `$`.  The following calls to `run` are equivalent;
they create a lambda function which prints "hello, world!", and based
on the `run` command they will be executed after 3 seconds.

```
# standard lambda function:
run(After(Seconds(3)), fn()
    print("hello, world!")
)

# with $ operator pre-fixed to a single function name:
run($print("hello, world!"), After(Seconds(3)))
```

For a class method, the `$` comes before the class instance:

```
class greeter(string Greeting)
    greet(string Noun)
        print("${Greeting}, ${Noun}!")

greeter Greeter(Greeting("hello"))
# standard lambda function:
run(After(Seconds(3)), fn()
    Greeter.greet(Noun("world"))
)

# with $ operator prefixing the Instance.method() call:
run(After(Seconds(3)), $Greeter.greet(Noun("world")))
```
