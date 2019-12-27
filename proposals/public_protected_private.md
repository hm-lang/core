# Visibility of class variables

When defining a class variable or member function, we declare it as
public/private/protected based on the number of leading and trailing
underscores.  Public variables are declared without any leading or
trailing underscores.  Protected variables are declared with a single
leading or trailing underscore, but not both.  Private variables are
declared with two leading, two trailing, or one of each underscore.

 * Public variables are gettable/settable globally from anywhere.
 * Protected variables are gettable globally, but can only be set
   by child classes, or by other classes defined in the same directory
   as the original class, i.e. *friend* classes (note that subdirectories
   are excluded from being friends).
 * Private variables are gettable/settable only by the original class,
   or by other classes/functions defined *within the same file* as the
   original class.  (This allows for testing private variables/methods,
   though this is not recommended.)

```
    # FILE: root/example/example.hm
    class EXAMPLE
        DBL x
        DBL y_
        DBL z__

    TEST example(FN()
        EXAMPLE ex
        print(ex.x) # great
        ex.x = 1    # great

        print(ex.y) # no problem
        ex.y = 2    # no problem

        print(ex.z) # OK, but not recommended
        ex.z = 3    # OK, but not recommended
    )

    # FILE: root/example/friend.hm
    class FRIEND_TO_EXAMPLE
        EXAMPLE ex
        MD doSomething()
            print(ex.x)     # ok
            ex.x = 1234.5   # ok
            print(ex.y)     # ok
            ex.y = 123.4    # ok
            # print(ex.z)   # NOT ALLOWED
            # ex.z = 3.1415 # NOT ALLOWED

    # FILE: root/example/subdir/not_a_friend.hm
    class NOT_A_FRIEND_TO_EXAMPLE
        EXAMPLE ex
        MD doSomething()
            print(ex.x)     # ok
            ex.x = 1234.5   # ok
            print(ex.y)     # ok
            # ex.y = 1234.5 # NOT ALLOWED
            # print(ex.z)   # NOT ALLOWED
            # ex.z = 3.1415 # NOT ALLOWED

    # and similarly for other files not in the same
    # directory as root/example/example.hm
```

NOTE!  Regardless of the identifier used when *declaring* a variable,
the leading/trailing underscores are stripped when *using* it.  No
one wants to remember the exact visibility of your variable.  This
also means that you cannot use leading/trailing underscores when
declaring arguments for functions; it is better to use a preceding
`new` or `old` keyword , as in the following example:

```
    class EXAMPLE
        DBL x__

        DBL MD getX()
            return x    # no leading/trailing underscores

        DBL MD setX(DBL new.x)
            x = new.x

    EXAMPLE ex
    DBL x = ex.getX() * 3
    ex.setX(x)
    ex.setX(X(1234.5))
```
