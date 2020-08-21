# Error handling

hm-lang will utilize a C++ style error handling mechanism,
with `try`/`catch`/`finally` and `throw`.

```
try
    mayThrowAnError()
catch(error E)
    warn("we did catch an error: ${E}")
finally
    print("runs after both cases: success or failure!")

mayThrowAnError()
    throw error("yes, this did in fact throw.")
    # parentheses are optional, `throw(...)` also works.
```

We will likely not require parentheses after a `catch` or `throw`,
but we include them here as a first pass.

In addition, each class automatically has a corresponding `error`
subclass, and errors from a child class can be caught by catching
a parent error.  Any time a `throw` is used inside a class, the
corresponding class' `error` subclass will be thrown, unless a
more (or less) specific error type is used.

```
class parent
    doSomething(Int)
        print("${Int} from parent")

    throwAnError(String)
        # throws a `parent.error`:
        throw String

class child extends parent
    doSomething(Int)
        # throws a `child.error`:
        throw("child un-implements this method due to bad design choices.")

    throwAnError(String)
        # specifically throws a more generic `error` type:
        throw(error(String))

child Child
try
    Child.doSomething(5)
catch parent.error E
    # this will also catch the child.error
    warn("parent error was ${E}")

# note that a `catch parent.error` block will not catch the more generic `error`.
```

As in other languages, we'll probably have the ability to catch
different types of errors as multiple `catch` blocks.  The
`error` type will catch any error, but errors can escape
the catch block if they are not a subtype of the caught errors.

```
class aClass(Int);
class bClass(Dbl);
class abClass(Int, Dbl) extends aClass;

# this will cause an error to continue propagating up,
# possibly stopping execution of the program:
try
    aClass A(3)
    A.Int /= 0
catch bClass.error
    print("should not produce a bClass.error")
    print("aClass.error should be produced.")
    print("this block never executes!")

# catching a child error using the parent error:
try
    abClass Ab(5, 3.14)
    Ab.Dbl /= 0.0
catch aClass.error
    print("shouldn't divide by zero.")
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
