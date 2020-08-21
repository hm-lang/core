# Blocks

A block is a set of lines at the same indent.  A single blank line
(either empty or consisting entirely of spaces) can interrupt a
block without changing the indent.  Also, a line that is blank
except for a comment will also not change the indent.  Examples:

```
Int hello()
    # indent 1
    print("this is a block at indent 1")

    print("this is still the same block")

    # this comment keeps things at the same block
    return 1
```

But two blank lines will break a block:

```
Int hello()
    # notice two following blank lines, breaks the block:


    # ERROR, this next line is an unexpected block indent
    return 1
```

Or obviously an unindent will break the block:

```
Int hello()
    return 3

print("unindenting breaks the block, obviously")

    # ERROR, this is an unexpected block indent
    return 1
```
