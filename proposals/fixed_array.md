# Fixed array

A fixed array is an unresizable array of elements.

```
class NewT fixedArray() extends NewT IndexedContainer
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
    NewT gate.Removable This[Index];

    # get the size of the array.
    get Size;
```

Internally, the `FixedArray` uses a lazily default-initialized array.
See [`Array`](./array.md) for more explanation.
