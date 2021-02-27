# Getter/setter classes

We make it easy to wrap a variable within getter/setter functions
using the `gate` or one of its specialized subclassses.

```
# this is defined automatically based on gate.Settable:
class NewT gate.settable.Lambda(NewT get(), set(NewT))
    to NewT
        return get()

    NewT Old This = (NewT)
        Old = get()
        set(NewT)
```

Generally speaking, what looks like a class member variable is actually
hidden behind a getter/setter.  This is so that child classes can easily
change the behavior of how the variables work.  This might be harder to
implement than expected, but here is the general idea:

```
class Vector2(dbl X, dbl Y);

class UnitVector2 extends Vector2
    from(dbl X, dbl Y)
        normalize(X, Y)

    get dbl X
        return Vector2.X
    get dbl Y
        return Vector2.Y

    set dbl X
        normalize(New.X, Y)
    set dbl Y
        normalize(X, New.Y)

    normalize__(dbl New.X, dbl New.Y)
        dbl R = (New.X^2 + New.Y^2)^0.5
        assert(R > 0, OrThrow: "Cannot normalize to a unit vector!")
        Vector2.X = New.X / R
        Vector2.Y = New.Y / R
```

In the latter case, we can access the original class's getters/setters
using the parent class' name (e.g. `Vector2`).

```
vector2 Parent = new(X: 5, Y: 3)
Parent.x(3) # equivalent to `Parent.X = 3`
Parent.x()  # equivalent to `Parent.X`
unitVector2 Unit = new(X: 1, Y: 4)
Unit.x(5)   # uses the child setter. equivalent to `Unit.X = 5`
```


## Other mixins

Here is a getter/setters which is also removable.  Note that the get/set functions
need to handle the case in which something is removed; i.e., they can throw
or return something else if it is reasonable.  Similarly, the `remove` function
must do something reasonable if it is called twice, or throw.

```
class NewT gate.Removable
    This = Null
        remove()

    NewT This = (NewT?)
        if NewT? != Null
            return swap(NewT?)
        else
            return remove()
```

Here is a getter/setter that is also viewable, i.e., no copies will be made
if we ask for just a view of the internal value.

```
class newT gate.Viewable
    to ConstNewT ConstRef
        return view()
```

Note that a `ref` type can be directly converted to a `gate.Refable` type.

The specific hierarchy is explored in the code library.
