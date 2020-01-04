# Visibility

When defining a class variable or member function, we declare it as
public/private/protected based on the number of leading and trailing
underscores.  Public identifiers are declared without any leading or
trailing underscores.  Protected identifiers are declared with a single
leading or trailing underscore, but not both.  Private identifiers are
declared with two leading, two trailing, or one of each underscore.

Note: When referring to the variable/function/method outside of the
declaration, *no leading/trailing underscores are necessary*.

## Visibility of variables

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

## Visibility of functions/methods

Functions and methods have the same rules, but differ from variables,
since we don't want to distinguish "getting" a function from "calling"
it (i.e., executing the function) at this point in time:

 * Public functions are callable globally from anywhere.
 * Protected functions are callable only by child classes or in other
   files in the same directory as the original function.
 * Private functions are callable only *within the same file*.

To elaborate on why we don't want to consider functions as "gettable":
"Getting" a function might correspond to obtaining a representation of
what it does internally, or perhaps getting a memory address at which it
is defined.  We don't allow any visibility into this (at the moment),
though this may change if we want to allow functions to be allowed
membership into `SET`s.


## Visibility of types

Similarly, types can be defined as protected/private within a given
file/directory using leading/trailing underscore(s).  E.g.
`PUBLIC_TYPE`, `PROTECTED_TYPE_`, and `PRIVATE_TYPE__`.

 * Public types can be referenced and instantiated anywhere/globally.
 * Protected types can only be instantiated directly in the same
   directory as the original type declaration (note that subdirectories
   are excluded).
 * Private types can only be instantiated directly in the file where
   they are defined/declared.

Of course protected and private types can be instantiated indirectly,
but they cannot be directly referenced outside of their visibility.
