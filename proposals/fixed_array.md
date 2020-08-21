# Fixed array

A fixed array is an unresizable array of elements.

```
class newT fixedArray extends newT indexedContainer
    from(Size);
    from(NewT Iterator);
    from([Index, NewT] Iterator);

    to NewT Iterator;
    to [Index, NewT] Iterator;

    # access the array at the given index, either for getting or setting.
    # accessing outside of the bounds of the array will throw an error.
    # the `remove()` method is also supported, which will shift all
    # values with larger indices down one, and lazily initialize a new
    # value at the end of the array (so that the size remains constant).
    NewT Grsv This(Index);

    # get the size of the array.
    get Size;
```

Internally, the `fixedArray` uses a lazily default-initialized array;
only elements that are requested (at a given index) will be initialized.
This guarantees that an element at a given index will only be initialized
once, before being called with `Grsv.get()`.  If `Grsv.set(...)` is called
before `Grsv.get()`, then the default-initialization will be skipped, and
only the `set` initialization will be performed.
