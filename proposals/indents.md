# Indenting

hm-lang takes a side on the space vs. tab debate, and even a stance on
how many spaces are required for an indent.  There is also a hard limit
on the maximum allowed number of indents.  (Refactor your code if it
has 5 or more levels of indentation.)  An indent consists of four spaces:

```
# indent 0
INT x = 0

FN doSomething()
    # indent 1
    print("something was done")

for INT i < 10
    for INT j < 100
        # indent 2
        print("${i}, ${j}")

# indent 0
class EXAMPLE
    # indent 1
    DBL MD doSomething(INT)
        # indent 2
        if int % 3 == 0
            # indent 3
            if int % 5 == 0
                # indent 4.  NO MORE ALLOWED AFTER THIS!
                return 1.125
            return 1.0625
        return 1.0        
```

## Blocks

A block is a set of lines at the same indent.  A single blank line
(either empty or consisting entirely of spaces) can interrupt a
block without changing the indent.  Also, a line that is blank
except for a comment will also not change the indent.  Examples:

```
INT FN hello()
    # indent 1
    print("this is a block at indent 1")

    print("this is still the same block")

    # this comment keeps things at the same block
    return 1
```

But two blank lines will break a block:

```
INT FN hello()
    # notice two following blank lines, breaks the block:


    # ERROR, this next line is an unexpected block indent
    return 1
```

Or obviously an unindent will break the block:

```
INT FN hello()
    return 3

print("unindenting breaks the block, obviously")

    # ERROR, this is an unexpected block indent
    return 1
```

## Continuing a statement on the next line

One can continue a statement by indenting +2 indents from the previous line.
This only works if there are no blank lines in between.  (A blank line
with a comment is ok.)  Any lines that follow at the +2 indent will be considered
continuations of the original line, so they need not continuously be indented at
+2.

```
INT value = someLongFunctionThatTakesALotOfSpace(3) *
        doubleIndentedLine + 5
        # comments are allowed here
        * (someValue + 5)
```

You can also use parentheses.  hm-lang is also opinionated here; the closing
parenthesis must be at the same indent as the opening parenthesis (based on the
start of the line), and the internal part of the parentheses should be at +1
indent (if not on the same line).  Some examples:

```
INT doStuff = (1234 + 5) * {
    12345 + someLongFunctionThatTakesALotOfSpace[
        Value(1.234), Greeting("Hello")
    ] * 5
}
```
