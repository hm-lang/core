# Array

Arrays are one of the most important data collections to "get right."
We will likely have arrays inherit from some parent `CONTAINER` class,
and presumably it can be a collection of some type `NEW_T`, or it can
also be thought of as a collection of `[INDEX, NEW_T]`, since people
can iterate over the indices as well as the individual elements.
The proposed API is this:

```
class NEW_T ARRAY extends NEW_T CONTAINER, [INDEX, NEW_T] CONTAINER
    from(NEW_T ITERATOR);
    from([INDEX, NEW_T] ITERATOR);

    to NEW_T ITERATOR;
    to [INDEX, NEW_T] ITERATOR;

    # access the array at the given index, either for getting or setting.
    # accessing outside the bounds of the array will resize the array and
    # create default initialized values up to that point.
    NEW_T GS MD this(INDEX);

    # get or set the size of the array.
    SIZE GS size;

    NEW_T MD shift();
    NEW_T MD pop();

    # insert elements from iterator starting at `INDEX at`, deleting
    # `delete` numbers of elements of the original array first.
    MD splice(NEW_T ITERATOR, INDEX at, SIZE delete);

    MD insert(NEW_T ITERATOR, INDEX at) = splice(newT_iterator, at, Delete(0))
    MD erase(INDEX, SIZE count) = splice(At(index), Delete(count))

    # TODO: see if we can't put this into the ITERATOR class:
    # take elements from this array and put them into a new one.
    # INT ARRAY x = [1, 2, 3, 100, 9, 11]
    # INT ARRAY y = x.take($whereValue < 10)
    # # y == [1, 2, 3, 9]
    # # x == [100, 11]
    NEW_T ARRAY MD take(BOOL FN(NEW_T REF whereValue));
```

We will probably change inheritance from `[INDEX, NEW_T] CONTAINER`
to `MAP Key(INDEX) Value(NEW_T)` since that is also appropriate.
