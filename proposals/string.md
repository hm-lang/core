# String type

Technically, the `STRING` type is an abstract base class, which
encompasses things like specialized character arrays.

```
class STRING extends UTF8 INDEXED_CONTAINER
    # ...

class CHAR_ARRAY extends STRING
    CHAR ARRAY _
    # ...

DEFAULT_STRING = CHAR_ARRAY
```

## String view

We will also be able to narrow the visible part of a string for
others to look at, and make it read-only.
