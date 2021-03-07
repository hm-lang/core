# Strings

TODO: maybe allow '' for strings as well, to be nice.
ASCII characters can be represented within single quotes, e.g. `'a'`,
strings can be represented within double quotes, e.g. `"hello, world!"`,
or within backticks, which can be multiline:

```
string X = `hello, there!` # single line ok!
string Y = `nice to meet you!
            this is a multiline string literal.
            it ignores space up to the starting point
            of the opening backtick.`
string Z = "another nice string"
```

Note that multiline backtick strings ignore spacing up to the opening
backtick, so that you can align code/text easily.  That also means
that it will be a parse error if you have anything (besides space)
before the first non-backtick character on the first line.

```
# ERROR!!
string BadLine = `This is a promising backtick string
    but its second line does not indent far enough.
    this is definitely going to be an error.`
```

If you are worried that your variable name will change and you
will have to reindent all your lines, make sure to use a line
continuation or a parentheses setup:

```
STRING thisVariableCanBeHoweverLongItWants =
        `Because this line will always be +2 indent.
         And the following lines will always be
         right where they need to be.`

STRING thisVariableCanBeHoweverLongItWantsToo = (
    `Because this line will always be +1 indent.
     And the following lines will always be
     right where they need to be.`
)
```

## String type

Technically, the `string` type is an abstract base class, which
encompasses things like specialized character arrays.

```
class string() extends Utf8 indexedContainer()
    # ...

class charArray() extends string()
    Char Array_
    # ...

Default.string = charArray
```

## String view

We will also be able to narrow the visible part of a string for
others to look at, and make it read-only.

## String interpolation

Using `${}` inside a string will automatically trigger string interpolation;
i.e. the text inside the parentheses will be invoked as code and the result printed.
Multiple code interpolations are guaranteed to be run in the same order as they
are written in the source, regardless of translations, but it is
highly recommended not to write interpolations that affect the 
state and require a specific ordering.

```
int X = 3
string Y = "value of X = ${X}"

int Z = 5
string A = `this is weird, ${X = Z + 5}, please don't do it.
            but if you must, ${Z = X - 3} always happens last.`

# expected ordering:
# (1) X = Z + 5     --> X = 10
# (2) Z = X - 3     --> Z = 7
# things that could happen in a translation, but won't:
# (1) Z = X - 3     --> Z = 0
# (2) X = Z + 5     --> X = 5
```

And of course, parentheses must match.

To avoid string interpolation (i.e. to escape a `${` combination),
simply use `\$` instead of `$` in the string.

```
string X = "\${this will not actually execute}"
print(X) # prints "${this will not actually execute}" without the quotes.
```

## Advanced interpolation

For ease of debugging, we might also want to introduce a special
interpolation, `$${someExpression()}`, which prints out the string
literal inside the parentheses, but also prints the value of the
expression.  The literal is followed by a " = ", then the value.
For example:

```
int X = 5
print("debugging $${X}") # prints "debugging X = 5"

Int doSomething(Int)
    return Int / 2

print("$${doSomething(X)}") # prints "doSomething(X) = 2"
```
TODO: maybe see if we can also print/debug what X is in the above example.

This should work regardless of debug state.
