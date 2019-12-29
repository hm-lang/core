# Fixed array

A fixed array is an unresizable array of elements.

```
class NEW_T FIXED_ARRAY extends NEW_T CONTAINER, [INDEX, NEW_T] CONTAINER
    from(CONST_SIZE size);
    from(NEW_T ITERATOR);
    from([INDEX, NEW_T] ITERATOR);

    to NEW_T ITERATOR;
    to [INDEX, NEW_T] ITERATOR;

    # access the array at the given index, either for getting or setting.
    # accessing outside of the bounds of the array will throw an error.
    NEW_T GS MD this(INDEX);

    # get the size of the array.
    get SIZE;
```

We will probably change inheritance from `[INDEX, NEW_T] CONTAINER`
to `MAP Key(INDEX) Value(NEW_T)` since that is also appropriate.
