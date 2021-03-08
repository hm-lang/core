# Error handling

hm-lang will utilize a Java style error handling mechanism,
with `try`/`catch`/`finally` and `throw`.

```
try
    mayThrowAnError()
catch Error
    warn("we did catch an error: ${Error}")
finally
    print("runs after both cases: success or failure!")

mayThrowAnError()
    throw error("yes, this did in fact throw.")
    # parentheses are optional, `throw(...)` also works.
```

In addition, each class automatically has a corresponding `error`
subclass, and errors from a child class can be caught by catching
a parent error.  Any time a `throw` is used inside a class, the
corresponding class' `error` subclass will be thrown, unless a
more (or less) specific error type is used.

```
class parent()
    doSomething(Int)
        print("${Int} from parent")

    throwAnError(String)
        # throws a `parent.error`:
        throw String

class child() extends parent()
    doSomething(Int)
        # throws a `child.Error`:
        throw("child un-implements this method due to bad design choices.")

    throwAnError(String)
        # specifically throws a more generic `error` type:
        throw(Error: String)

child Child
try
    Child.doSomething(5)
catch(parent.Error)
    # this will also catch the child.error
    warn("parent error was ${Error}")

# note that the `catch(parent.Error)` block will not catch the more generic `Error`.
```

As in other languages, we'll probably have the ability to catch
different types of errors as multiple `catch` blocks.  The
`error` type will catch any error, but errors can escape
the catch block if they are not a subtype of the caught errors.

```
class aClass(Int);
class bClass(Dbl);
class abClass(Int, Dbl) extends aClass(Int);

# this will cause an error to continue propagating up,
# possibly stopping execution of the program:
try
    aClass A = 3
    A.Int /= 0
catch bClass.Error
    print("should not produce a bClass.Error")
    print("aClass.Error should be produced.")
    print("this block never executes!")

# catching a child error using the parent error:
try
    abClass Ab = (5, 3.14)
    Ab.Dbl /= 0.0
catch aClass.Error
    print("${Error}: shouldn't divide by zero.")
    print("your parent was watching!")
    print("this block should execute.")
```

When multiple `catch` blocks are used, the first error
to match will get its block executed.  So if you have
a parent and a child class error both showing up, you
should catch the child error first, as it is more specific;
otherwise a parent error catch block will match a child
error, and the later child error block would never
execute.
