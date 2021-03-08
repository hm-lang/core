# New/old identifiers

We use new (and old) in a few different ways.

* `newT` is a way to indicate a template type.  `T` can be any
  `UpperCamelCase` identifier, e.g., making the full type `newSnakePancake`.

* `New` can be used as a variable prefix to ensure variable identifiers are
  unique, e.g. `New.X` in a class that already has an `X` variable defined.
  We can also use `Old` in this way, e.g. `Old.ValueWeCareAbout`.

* `New` can be a shorthand for accessing the new value in a setter, with
  the name of the variable following a period, e.g. `New.Value`.

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
class NewT example(newT Value)
    setFrom(NewU)
        Value = NewU    # implicit conversion

Int example X = (Value: 3)
X.setFrom("50003")
```

For classes, see the discussion on [template classes](./template_class.md).

## Variable prefix to avoid duplicate identifiers

Here is an example of using `New.SomeVariableName`.  The specification
will use `SomeVariableName` (i.e., not prefixed with `New`).  Similarly,
we can use `Old` as a prefix.

```
class example(int X)
    Int set(int New.X)
        int Old.X = X
        X = New.X
        return Old.X

example Ex = (X: 5)
print(Ex.set(X: 1234)) # prints 5
```

## Setters

Here is an example for using a `New.SomeVariableName` in a setter.

```
class inverse()
    dbl X_

    set dbl Y
        X = 1.0/New.Y

    get dbl Y
        return 1.0/X
```
