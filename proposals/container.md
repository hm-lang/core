# Container

A container class holds some number of elements.  It is the base/parent
class for a variety of useful data structures, including arrays and maps.
Anything that implements the `CONTAINER` class must be convertable
to and from an `ITERATOR`, and it must have a way to determine the number
of elements it holds, i.e., by getting its size.

```
abstract class NEW_T CONTAINER
    from(NEW_T ITERATOR);
    to NEW_T ITERATOR;

    # returns number of elements in the container.
    get SIZE;
```
