# Comments

There are three types of comments in hm-lang.

* End-of-line (EOL) comment, starts with `#` and any other character besides
  a opening parenthesis (i.e. `(`, `[`, or `{`).

* Mid-line comment, starts with `#[` and *must finish* with `]#` on the same line.
  Similarly works for `#( ... )#` and `#{ ... }#`.

* Multi-line comment, where line must be blank until the opening `#[`, and
  all subsequent lines will be ignored until a blank line with a closing `]#`
  is encountered at the same indent as the starting `#[`.  Nothing else (besides
  blank space) should appear before the opening `#[` or after the closing `]#`.
  Similarly for `{}` and `()`.

Examples:

```
INT x = 3 # end-of-line (EOL) comment

DBL y = 12.34 * #[ midline comment ]# 5.678

#(
multiline comment
can go on as long as you like
    #[
    you can even nest multiline comments
    ]#

    #{
    you don't have to finish nested multiline comments.
    this is still a comment.

still a comment, no parse errors.
)#

STRING z = "hello" # this statement will run, of course
#[
this is a multiline comment.
    and you can even indent internally,
    and 'incorrectly' finish an indented
    multiline comment without ending the original:
    }#
    and this will still be a comment.
]#
```

Thus multiline comments can be nested quite readily.

## Incorrect syntax: mismatched parentheses

We will raise some parse errors if the midline or multiline comment
parentheses are paired incorrectly, e.g.:

```
INT x = #{ midline comment with incorrect pairing ]# 5

#(
multiline comment
with bad pairing:
#}
```

We won't care about mismatched pairings in nested comments.

## Incorrect syntax: encountering an unindented line in a multiline comment

If we create a multiline comment at some indent, and then encounter
anything besides a blank line, we require that the line with *any*
content (including a EOL-commented line) is at an indent level equal to
or greater than the indent level of the multiline comment.

This would raise a parse error:

```
class EXAMPLE
    MD doSomething()
        #{
        start of a multiline comment

    # this EOL-commented line would error

    MD doAnotherThing() # and this line would error, too

        #}
```

Unindented lines would make it harder to see what the multiline comment
covers, so they are forbidden.
