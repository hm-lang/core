# Internal keyword

Like [New](./new.md), we should have a keyword that is useful for class-instance
specific variables or functions, which should not be modified except internally by
the class methods.  `Internal` can not show up in a method's arguments (except in
`from`s), but it can be used internally by the class.  It is useful in case a child
class has similar variables/methods as a parent class, but must distinguish them.

```
class Parent
    doSomething()
        print("yay")

class Child extends Parent
    # you can initialize an `Internal` value via a `from`:
    from(This.Internal.doSomething() = print("child class yay, too!"));

    # you can reference it without `This` in other places:
    doSomething()
        Parent.doSomething()
        Internal.doSomething()

    # but you cannot pass an `Internal` in from outside otherwise:
    doOtherThing(int Internal.Int) # ERROR, this doesn't make sense.  use `New` or `Old` keywords
        print("this doesn't work ${Internal.Int}")
```

NOTE: Variables/functions marked by the `Internal` keyword are automatically private.
This keyword should also only appear in classes, specifically class instance methods (no static methods).
See [classes](./classes.md).

TODO: it's maybe not a bad idea to remove the concept of public/protected/private via underscores,
and use something like this.
