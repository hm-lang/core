# Array

Arrays are one of the most important data collections to "get right."
We will likely have arrays inherit from some parent `container` class,
and presumably it can be a collection of template type `newT`, or it can
also be thought of as a collection of `[Index, NewT]`, since people
can iterate over the indices as well as the individual elements.
The proposed API is this:

```
class newT array extends NewT IndexedContainer
    from(NewT Iterator);
    from([Index, NewT] Iterator);

    to NewT Iterator;
    to [Index, NewT] Iterator;

    # access the array at the given index, either for getting or setting.
    # accessing outside the bounds of the array will resize the array and
    # create (lazily) default-initialized values up to that point.
    # the `remove()` method is also supported, which will shift all
    # values with larger indices down one.
    # TODO: `This(Index)` may be hard to distinguish from a map definition.
    NewT Grsv This(Index);

    # get or set the size of the array.
    Size Gs Size;

    NewT shift();
    NewT pop();

    # insert elements from iterator starting at `index At`, deleting
    # `Delete` numbers of elements of the original array first.
    splice(NewT Iterator, index At, size Delete);

    insert(NewT Iterator, index At) = splice(NewT_Iterator, At, Delete(0))
    erase(Index, size Count) = splice(At(Index), Delete(Count))
```

The parent class, `newT indexedContainer`, is a combination class which
inherits from `newT container` and `(index key Index, newT Value) map`.

Internally, `array` uses a lazily default-initialized array;
only elements that are requested (at a given index) will be initialized.
This guarantees that an element at a given index will only be initialized
once, before being called with `Grsv.get()`.  If `Grsv.set(...)` is called
before `Grsv.get()`, then the default-initialization will be skipped, and
only the `set` initialization will be performed.
