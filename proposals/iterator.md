# Iterators

When we want to iterate over a collection of values, iterators do
the job for us.  Any `container` child class allows for conversion
to and from an `iterator`, which makes it easy to work with their
values in various ways, e.g. filtering or mapping to another type.

For instance, you can take from any peakable/removable iterator using
the function `take`:
TODO: maybe make it a static function `iterator.take`

```
NewT Iterator take(NewT iterator.pR From, Bool fn(NewT view WhereValue));

Int[] X = [1, 2, 3, 100, 9, 11]
Int[] Y = take(From(X), $WhereValue < 10)
```

The benefits of this approach are that casting is performed implicitly due
to the naming of the `From` variable.  The downside is that this is a global
function, and we want to minimize these, since we don't want *any* naming
clashes.  We could make the `take` function a static (class) function on `iterator`:

```
NewT Iterator iterator.take(NewT iterator.pR From, Bool fn(NewT view WhereValue));

Int[] X = [1, 2, 3, 100, 9, 11]
Int[] Y = iterator.take(From(X), $WhereValue < 10)
```

Naming clashes for functions aren't so bad as long as overloading makes sense.
