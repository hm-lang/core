# Lambda functions and the `$` operator

The `$` operator is special in hm-lang, as it is a way to create
lambda functions.  It has many different incarnations.

## `$` prefixing an identifier

We can create lambda functions with arguments (yay!) very easily using
this syntax.  The arguments are prefixed with `$`.  Here is an example
where we take an iterator and use it to create a list.

```
# creates an iterator that goes from 0 to the passed in `int` (minus 1).
INT ITERATOR FN range(INT)
    INT current = 0
    return INT? FN next()
        return null if current >= int
        return current++

INT ARRAY x = range(10).map($int * 2 + 1)
# also equivalent:
# INT ARRAY x = range(10).map(INT FN(INT)
#     return int * 2 + 1
# )
print(x) # [1, 3, 5, 7, 9, 11, 13, 15, 17, 19]

# can convert to a different type as well.
DBL ARRAY y = range(5).map($int * 0.5)
print(y) # [0.0, 0.5, 1.0, 1.5, 2.0]
```

It's easy to extend this to multiple variables.  For example, here's
a callback with multiple arguments:

```
FN runSomething(DBL FN(DBL multiplier, DBL))
    print("multiplier=1.5, dbl=0.3: ${fn(Multiplier(1.5), 0.3)}")

runSomething($multiplier * ($dbl + 1))
```

## `$` prefixing a single function/method

TODO: ensure this doesn't grammatically interfere with `$` arguments above.

We can make a lambda function by prefixing a single function or
method call with a `$`.  The following calls to `run` are equivalent;
they create a lambda function which prints "hello, world!", and based
on the `run` command they will be executed after 3 seconds.

```
# standard lambda function:
run(After(Seconds(3)), FN()
    print("hello, world!")
)

# with $ operator pre-fixed to a single function name:
run($print("hello, world!"), After(Seconds(3)))
```

For a class method, the `$` comes before the class instance:

```
class GREETER(STRING greeting)
    MD greet(STRING noun)
        print("${greeting}, ${noun}!")

GREETER greeter(Greeting("hello"))
# standard lambda function:
run(After(Seconds(3)), FN()
    greeter.greet(Noun("world"))

# with $ operator prefixing the instance.method call:
run(After(Seconds(3)), $greeter.greet(Noun("world")))
```
