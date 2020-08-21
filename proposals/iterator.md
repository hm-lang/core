# Iterators

When we want to iterate over a collection of values, iterators do
the job for us.  Any `container` child class allows for conversion
to and from an `iterator`, which makes it easy to work with their
values in various ways, e.g. filtering or mapping to another type.

The syntax for casting an iterable type to an `iterator` works like this:

```
Int[] X = [1, 2, 3, 100, 9, 11]
Int[] Y = iterator(X).take($WhereValue < 10)
# X == [100, 11]
# Y == [1, 2, 3, 9]
```

But we *might* also want to offer this shorthand, `->`, which will
look through the methods of other classes that the instance can be transformed to:

```
Int[] Y = X->take($WhereValue < 10)
```

Ideally, we avoid adding extra notation/syntax that confuses users,
but we don't want to clutter up the `container` class with extra
methods.

TODO: make a choice:

1.  Force users to cast to `iterator`
2.  Define `map` on the `container` class, defering to the `iterator` approach
    (subclasses can create their own specialized implementation)
3.  Use a special `->` operator which searches through `to` classes
    for the subsequent method.
4.  Create a global `take` function (and similar), so you can do stuff like this.

```
take(NewT iterator From, Bool fn(ConstNewT constRef WhereValue));

Int[] X = [1, 2, 3, 100, 9, 11]
Int[] Y = take(From(X), $WhereValue < 10)
```

The benefits of this approach are that casting is performed implicitly due
to the naming of the `From` variable.
