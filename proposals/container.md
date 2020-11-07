# Container

A container class holds some number of elements.  It is the base/parent
class for a variety of useful data structures, including arrays and maps.
Anything that implements the `container` class must be convertable
to and from an `iterator`, and it must have a way to determine the number
of elements it holds, i.e., by getting its size.

```
class newT container
    from(NewT Iterator);
    to NewT Iterator;

    # returns number of elements in the container.
    view Size;
```
