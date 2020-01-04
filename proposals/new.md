# New/old identifiers

We use new (and old) in a few different ways.

* `NEW` is a way to instantiate a class, usually as the right-hand-side
  of some expression, e.g. `SOME_CLASS x = NEW(Greeting("hello"), Times(3))`

* `NEW_TYPE` is a way to indicate a template type.  `TYPE` can be any
  `UPPER_SNAKE_CASE` identifier, e.g. `NEW_T` or `NEW_SNAKE_PANCAKE`.

* `new` can be used as a variable prefix to ensure variable identifiers are
  unique, e.g. `new.x` in a class that already has an `x` variable defined.
  We can also use `old` in this way, e.g. `old.valueWeCareAbout`.

* `new` can be a shorthand for accessing the new value in a setter, with
  the name of the variable following a period, e.g. `new.value`.

## NEW class instance

We can create a new class instance using the `NEW` syntax.  Using `NEW`
creates a specification with the subsequent variable names, which
can be converted into whatever class type is on the left-hand side.

```
class EXAMPLE(INT times, STRING greeting, STRING noun = "world");

EXAMPLE ex(Times(5), Greeting("hello")) # normal initialization

ex = NEW(Greeting("hey"), Noun("people")) # a NEW way to do it.
```

## `NEW_TYPE` for templates

For functions (and class methods), we use this syntax:

```
NEW_V FN doSomethingWithTemplates(NEW_T x, NEW_U y)
    print("${x} -> ${y}")
    return y    # transforms from NEW_U to NEW_V type.

# NEW_V = DBL, NEW_T = INT, and NEW_U = STRING:
DBL q = doSomethingWithTemplates(X(5), Y("1.2345"))
```

And of course, fewer templated types can be used in a function.
Here's an example also with a class:

```
class NEW_T EXAMPLE(NEW_T value)
    MD setFrom(NEW_U)
        value = newU    # implicit conversion

INT EXAMPLE x(Value(3))
x.setFrom("50003")
```

For classes, see the discussion on [template classes](./template_class.md).

## Variable prefix to avoid duplicate identifiers

Here is an example of using `new.someVariableName`.  The specification
will use `SomeVariableName` (i.e., not prefixed with `New`).  Similarly,
we can use `old` as a prefix.

```
class EXAMPLE(INT x)
    INT MD set(INT new.x)
        INT old.x = x
        x = new.x
        return old.x

EXAMPLE ex(X(5))
print(ex.set(X(1234))) # prints 5
```

## Setters

Here is an example for using a `new.someVariableName` in a setter.

```
class INVERSE
    DBL x_

    set DBL y
        x = 1.0/new.y

    get DBL y
        return 1.0/x
```
