# String literals

ASCII characters can be represented within single quotes, e.g. `'a'`,
strings can be represented within double quotes, e.g. `"hello, world!"`,
or within backticks, which can be multiline:

```
    STRING x = `hello, there!` # single line ok!
    STRING y = `nice to meet you!
                this is a multiline string literal.
                it ignores space up to the starting point
                of the opening backtick.`
    STRING z = "another nice string"
```

Note that multiline backtick strings ignore spacing up to the opening
backtick, so that you can align code/text easily.  That also means
that it will be a parse error if you have anything (besides space)
before the first non-backtick character on the first line.

```
    STRING badLine = `This is a promising backtick string
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
