# New/old identifiers

We use new (and old) in a few different ways.

TODO: remove `new()` and just do {} initialization
* `new` is a way to instantiate a class, usually as the right-hand-side
  of some expression, e.g. `someClass X = new(Greeting: "hello", Times: 3)`

* `newT` is a way to indicate a template type.  `T` can be any
  `UpperCamelCase` identifier, e.g., making the full type `newSnakePancake`.

* `New` can be used as a variable prefix to ensure variable identifiers are
  unique, e.g. `New.X` in a class that already has an `X` variable defined.
  We can also use `Old` in this way, e.g. `Old.ValueWeCareAbout`.

* `New` can be a shorthand for accessing the new value in a setter, with
  the name of the variable following a period, e.g. `New.Value`.

## new class instance

We can create a new class instance using the `new` syntax.  Using `new`
creates a specification with the subsequent variable names, which
can be converted into whatever class type is on the left-hand side.

```
class Example(int Times, string Greeting, string Noun = "world");

example Ex = new(Times: 5, Greeting: "hello") # normal initialization

ex = new(Greeting: "hey", Noun: "people") # overwrite
```

## `newType NewType` for templates

For functions (and class methods), we use this syntax:

```
NewV doSomethingWithTemplates(newT X, newU Y)
    print("${X} -> ${Y}")
    return Y    # transforms from newU to newV type.

# here, newV = dbl, newT = int, and newU = string:
dbl Q = doSomethingWithTemplates(X: 5, Y: "1.2345")
```

And of course, fewer templated types can be used in a function.
Here's an example also with a class:

```
class NewT Example(newT Value)
    setFrom(NewU)
        Value = NewU    # implicit conversion

Int example X = new(Value: 3)
X.setFrom("50003")
```

For classes, see the discussion on [template classes](./template_class.md).

## Variable prefix to avoid duplicate identifiers

Here is an example of using `New.SomeVariableName`.  The specification
will use `SomeVariableName` (i.e., not prefixed with `New`).  Similarly,
we can use `Old` as a prefix.

```
class Example(int X)
    Int set(int New.X)
        int Old.X = X
        X = New.X
        return Old.X

example Ex = new(X: 5))
print(Ex.set(X: 1234)) # prints 5
```

## Setters

Here is an example for using a `New.SomeVariableName` in a setter.

```
class Inverse
    dbl X_

    set dbl Y
        X = 1.0/New.Y

    get dbl Y
        return 1.0/X
```
