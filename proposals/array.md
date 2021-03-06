# Array

Arrays are one of the most important data collections to "get right."
We will likely have arrays inherit from some parent `container` class,
and presumably it can be a collection of template type `newT`, or it can
also be thought of as a collection of `[Index, NewT]`, since people
can iterate over the indices as well as the individual elements.
The proposed API is this:

```
class NewT array() extends NewT indexedContainer()
    from(NewT Iterator);
    from([Index, NewT] Iterator);

    to NewT Iterator;
    to [Index, NewT] Iterator;

    # access the array at the given index, either for getting or setting.
    # accessing outside the bounds of the array will resize the array and
    # create (lazily) default-initialized values up to that point.
    # the `remove()` method is also supported, which will shift all
    # values with larger indices down one.
    NewT gate.Removable This[Index];

    # get or set the size of the array.
    # shorthand:
    # * MyArray.Size.get() -> MyArray.size()
    # * MyArray.Size.set(5) -> MyArray.size(5)
    Size gate.Settable Size;

    NewT shift();
    NewT pop();

    # insert elements from iterator starting at `index At`, deleting
    # `Delete` numbers of elements of the original array first.
    splice(NewT Iterator, index At, size Deleting);

    insert(NewT Iterator, index At) = splice(Iterator, At, Deleting: 0)
    erase(Index, size Count) = splice(At: Index, Deleting: Count)
```

The parent class, `NewT IndexedContainer`, is a combination class which
inherits from `NewT Container` and `(Index key Index, newT Value) Map`.

Internally, `Array` uses a lazily default-initialized array;
only elements that are requested (at a given index) will be initialized.
This guarantees that an element at a given index will only be initialized
once, before being called with its gate's `get()`.  If `set(...)` is called
before `get()`, then the default-initialization will be skipped, and
only the `set` initialization will be performed.
