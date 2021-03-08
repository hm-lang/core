# Continuing a statement on the next line

One can continue a statement by indenting +2 indents from the previous line.
This only works if there are no blank lines in between.  (A blank line
with a comment is ok.)  Any lines that follow at the +2 indent will be considered
continuations of the original line, so they need not be indented at +4, +6, etc.

```
int Value = someLongFunctionThatTakesALotOfSpace(3) *
        DoubleIndentedLine + 5
        # comments are allowed here
        * (SomeValue + 5)
```

You can also use parentheses.  hm-lang is also opinionated here; the closing
parenthesis must be at the same indent as the opening parenthesis (based on the
start of the line), and the internal part of the parentheses should be at +1
indent (if not on the same line).  Some examples:

```
int Something = (1234 + 5) * {
    12345 + someLongFunctionThatTakesALotOfSpace[
        Value: 1.234, Greeting: "Hello"
    ] * 5
}
```
