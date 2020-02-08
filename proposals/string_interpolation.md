# String interpolation

Using `${}` or `$()` or `$[]` inside a string will automatically
trigger string interpolation; i.e. the text inside the parentheses
will be invoked as code and the result printed.  Multiple code
interpolations are guaranteed to be run in the same order as they
are written in the source, regardless of translations, but it is
highly recommended not to write interpolations that affect the 
state and require a specific ordering.

```
INT x = 3
STRING y = "value of x = ${x}"

INT z = 5
STRING a = `this is weird, $(x = z + 5), please don't do it.
            but if you must, $[z = x - 3] always happens last.`

# expected ordering:
# (1) x = z + 5     --> x = 10
# (2) z = x - 3     --> z = 7
# things that could happen in a translation, but won't:
# (1) z = x - 3     --> z = 0
# (2) x = z + 5     --> x = 5
```

And of course, parentheses must match.

To avoid string interpolation (i.e. to escape a `${` combination),
simply use `\$` instead of `$` in the string.

```
STRING x = "\${this will not actually execute}"
print(x) # prints "${this will not actually execute}" without the quotes.
```

## Advanced interpolation

For ease of debugging, we might also want to introduce a special
interpolation, `$${someExpression()}`, which prints out the string
literal inside the parentheses, but also prints the value of the
expression.  The literal is followed by a " -> ", then the value.
For example:

```
INT x = 5
print("debugging $${x}") # prints "debugging x -> 5"

INT FN doSomething(INT)
    return int / 2

print("$${doSomething(x)}") # prints "doSomething(x) -> 2"
```

This should work regardless of debug state.
