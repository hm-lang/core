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
    # the `GS.delete` method is also supported, which will shift all
    # values with larger indices down one, and lazily initialize a new
    # value at the end of the array (so that the size remains constant).
    NEW_T GS MD this(INDEX);

    # get the size of the array.
    get SIZE;
```

We will probably change inheritance from `[INDEX, NEW_T] CONTAINER`
to `MAP Key(INDEX) Value(NEW_T)` since that is also appropriate.

Internally, the `FIXED_ARRAY` uses a lazily default-initialized array;
only elements that are requested (at a given index) will be initialized.
This guarantees that an element at a given index will only be initialized
once, before being called with `GS.get()`.  If `GS.set(...)` is called
before `GS.get()`, then the default-initialization will be skipped, and
only the `set` initialization will be performed.
