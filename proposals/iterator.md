# Iterators

When we want to iterate over a collection of values, iterators do
the job for us.  Any `container` child class allows for conversion
to and from an `iterator`, which makes it easy to work with their
values in various ways, e.g. filtering or mapping to another type.

For instance, you can take from any peakable/removable iterator using
the `iterator` class-function `take`:

```
NewT Iterator iterator.take(NewT iterator.pR From, Bool fn(NewT view WhereValue));

Int[] X = [1, 2, 3, 100, 9, 11]
Int[] Y = iterator.take(From(X), $WhereValue < 10)
# now X = [100, 11]
# and Y = [1, 2, 3, 9]
```

The benefits of this approach are that casting is performed implicitly due
to the naming of the `From` variable.  This way we avoid creating a global
function `take`, and we avoid naming clashes there (which we don't want any of,
besides operator overloading).
