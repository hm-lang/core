# Iterators

When we want to iterate over a collection of values, iterators do
the job for us.  Any `CONTAINER` child class allows for conversion
to and from an `ITERATOR`, which makes it easy to work with their
values in various ways, e.g. filtering or mapping to another type.

```
class NEW_T ITERATOR
    from(
        # function which gets the next value of the iterator, or null
        # if the iterator is finished:
        NEW_T? FN this.next()
        
        # value which provides the total number of elements which will
        # be iterated over, if available.
        CONST(SIZE?) this.size
        
        # function which inserts a new value `newT` before the next value
        # in the iteration.  this value will not appear when calling `next()`,
        # and classes which implement this should guarantee that any number
        # of repeated `next()`s will not eventually return this inserted value.
        # i.e., any values inserted into the underlying container during
        # iteration should not appear in this iterator (even if insertion does
        # affect some of the later values in the iterator).
        FN? this.insert(NEW_T)
        # TODO: we may want to allow multiple insert values, i.e., to
        # make this `FN? this.insert(NEW_T ITERATOR)`.
        
        # function which deletes the current value at `peak`.
        # it will not appear when calling `next()`.
        FN? this.delete()
    );

    # provides the next value that will appear in the iterator.
    # after calling this,
    #  calling `next()` is guaranteed to produce this value,
    #  unless `delete()` is called.
    NEW_T? REF MD peak();

    # standard map and filter all in one.
    # the callback should return a null if the `NEW_U` instance
    # should not be produced in the new iterator.
    NEW_U ITERATOR MD map(NEW_U? FN(NEW_T));
```
