# Comments

There are three types of comments in hm-lang.

* End-of-line (EOL) comment, starts with `#` and any other character besides `/`.

* Mid-line comment, starts with `#/` and *must finish* with `/#` on the same line.

* Multi-line comment, where line must be blank until the opening `===`, and
  all subsequent lines will be ignored until a blank line with a closing `===`
  is encountered at the same indent as the starting `===`.  Nothing else (besides
  blank space) should appear before the opening `===` or after the closing `===`.

Examples:

```
int X = 3 # end-of-line (EOL) comment

dbl Y = 12.34 * #/ midline comment /# 5.678

===
multiline comment
can go on as long as you like
    ===
    you can even nest multiline comments
    ===

    ===
    you don't have to finish nested multiline comments.
    this is still a comment.

still a comment, no parse errors.
===

string Z = "hello" # this statement will run, of course
===
this is a multiline comment.
    and you can even indent internally,
    and 'incorrectly' finish an indented
    multiline comment without ending the original:
    ===
    and this will still be a comment.
===
```

## Incorrect syntax: encountering an unindented line in a multiline comment

If we create a multiline comment at some indent, and then encounter
anything besides a blank line, we require that the line with *any*
content (including a EOL-commented line) is at an indent level equal to
or greater than the indent level of the multiline comment.

```
Int hello()
    ===
    a multiline comment.  this is ok.
    ===
    return 5

Dbl put(Dbl)
    ===
    this multiline comment is bad
because it unindents!  # ERROR!
# this would also be bad!
    ===
    return Dbl * 3
```

Unindented lines would make it harder to see what the multiline comment
covers, so they are forbidden.
