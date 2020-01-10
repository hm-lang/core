# Error handling

hm-lang will utilize a C++ style error handling mechanism,
with `try`/`catch`/`finally` and `throw`.

```
try
    mayThrowAnError()
catch(ERROR e)
    warn("we did catch an error: ${e}")
finally
    print("runs after both cases: success or failure!")

FN mayThrowAnError()
    throw(ERROR("yes, this did in fact throw."))
```

We will likely not require parentheses after a `catch` or `throw`,
but we include them here as a first pass.

In addition, each class automatically has a corresponding `ERROR`
subclass, and errors from a child class can be caught by catching
a parent error.  Any time a `throw` is used inside a class, the
corresponding class' `ERROR` subclass will be thrown, unless a
more (or less) specific error type is used.

```
class PARENT
    MD doSomething(INT)
        print("${int} from parent")

    MD throwAnError(STRING)
        # throws a `PARENT.ERROR`:
        throw(string)

class CHILD extends PARENT
    MD doSomething(INT)
        # throws a `CHILD.ERROR`:
        throw("child un-implements this method.")

    MD throwAnError(STRING)
        # specifically throws a more generic `ERROR` type:
        throw(ERROR(string))

CHILD child
try
    child.doSomething(5)
catch PARENT.ERROR e
    # this will also catch the CHILD.ERROR
    warn("parent error was ${e}")
```

As in other languages, we'll probably have the ability to catch
different types of errors as multiple `catch` blocks.  The
`ERROR` type will catch any error, but errors can escape
the catch block if they are not a subtype of the caught errors.

```
class A_CLASS(INT);
class B_CLASS(DBL);
class AA_CLASS(INT, DBL) extends A_CLASS;

# this will cause an ERROR to continue propagating up,
# possibly stopping execution of the program:
try
    A_CLASS a(3)
    a.int /= 0
catch B_CLASS.ERROR
    print("should not produce a B_CLASS.ERROR")
    print("A_CLASS.ERROR should be produced.")
    print("this block never executes!")

# catching a child error using the parent error:
try
    AA_CLASS aa(5, 3.14)
    aa.dbl /= 0.0
catch A_CLASS.ERROR
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
