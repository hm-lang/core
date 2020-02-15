# Iterators

When we want to iterate over a collection of values, iterators do
the job for us.  Any `CONTAINER` child class allows for conversion
to and from an `ITERATOR`, which makes it easy to work with their
values in various ways, e.g. filtering or mapping to another type.

The syntax for casting an iterable type to an `ITERATOR` works like this:

```
INT ARRAY x = [1, 2, 3, 100, 9, 11]
INT ARRAY y = ITERATOR(x).take($whereValue < 10)
```

But we *might* also want to offer this shorthand:

```
DBL ITERATOR odd = y->map(0.5 * $int^2)
# equivalent to this:
DBL ITERATOR odd = ITERATOR(y).map(0.5 * $int^2)
```

Alternatively, we define `map` on the `CONTAINER` class, to defer
to the `ITERATOR` if no more specialized implementation is available.
