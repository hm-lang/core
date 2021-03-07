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
   by friends of the class.

 * Private variables are gettable/settable by the original class,
   and are gettable by other friends of the class.

One can qualify as a friend of a class in two different ways:

1. You are a child class of the class in question.

2. You are another class or function defined *within the same file*
   as the class in question.

```
class example()
    dbl X   # public (get/set global)
    dbl Y_  # protected (get global, settable by friends only)
    dbl Z__ # private (gettable by friends only)

class friendOfExample()
    example Ex
    doSomething()
        print(Ex.X)     # ok
        Ex.X = 1234.5   # ok
        print(Ex.Y)     # ok
        Ex.Y = 123.4    # ok
        print(Ex.Z)     # ok
        Ex.Z = 3.1415   # ERROR! NOT ALLOWED

test ExampleVisibility = fn()
    example Ex
    print(Ex.X) # great
    Ex.X = 1    # great

    print(Ex.Y) # no problem
    Ex.Y = 2    # no problem, test in this file is a friend.

    print(Ex.Z) # OK, but not recommended
    Ex.Z = 3    # ERROR, cannot set private variables
```

NOTE!  Regardless of the identifier used when *declaring* a variable,
the leading/trailing underscores are stripped when *using* it.  No
one wants to remember the exact visibility of your variable.  This
also means that you cannot use leading/trailing underscores when
declaring arguments for functions; it is better to use a preceding
`New` or `Old` keyword, explained in more detail [here](./new.md).


## Visibility of functions/methods

Calling a function or a method from a class requires as much privileges
as *setting* a variable of similar visibility.  That is,

 * Public functions are callable globally from anywhere.

 * Protected functions are callable only by friends.

 * Private functions are callable only within the same class.

Private functions outside of a class wouldn't be callable at all, so
these are compile-time errors.


## Visibility of types

Similarly, types can be defined as public/protected/private within a given
file using leading/trailing underscore(s).  E.g. `publicType`,
`protectedType_`, and `privateType__`.  Note that types are like functions,
so they follow the same visibility rules:

 * Public types can be referenced and instantiated anywhere/globally.

 * Protected types can only be instantiated by friends.

 * Private types are only allowed nested within a class, and are only
   instantiable within that class.

Private types outside of a class wouldn't be instantiable at all, so
they are forbidden.

As a note: protected and private types can be instantiated indirectly,
but they cannot be directly referenced outside of their visibility.
