# Getter/setter classes

We make it easy to wrap a variable within getter/setter functions
using the `gs` (getter swapper) class.  The proposed API is:

```
class newT gs(NewT get(), newT Old swap(NewT))
    to newT
        return get()

    # TODO: consistent syntax here:
    newT Old This = (NewT)
        return swap(NewT)
```

Generally speaking, what looks like a class member variable is actually
hidden behind a getter/setter.  This is so that child classes can easily
change the behavior of how the variables work.  This might be harder to
implement than expected, but here is the general idea:

```
class vector2(dbl X, dbl Y);

class unitVector2 extends Vector2
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
        assert(R > 0, OrThrow("Cannot normalize to a unit vector!"))
        Vector2.X = New.X / R
        Vector2.Y = New.Y / R
```

In the latter case, we can access the original class's getters/setters
using the parent class' name (e.g. `Vector2`).


## Other mixins

Here is a getter/setters which is also removable.  Note that the get/set functions
need to handle the case in which something is removed; i.e., they can throw
or return something else if it is reasonable.  Similarly, the `remove` function
must do something reasonable if it is called twice, or throw.

```
class newT grs(NewT remove(), NewT Gs.get(), newT Old Gs.swap(NewT)) extends NewT Gs
    This = Null
        remove()

    NewT This = (NewT?)
        if NewT?
            return swap(newTQ)
        else
            return remove()
```

Here is a getter/setter that is also viewable, i.e., no copies will be made
if we ask for just a view of the internal value.

```
class newT Gsv(
    NewT Gs.get()
    newT Old Gs.swap(NewT)
    ConstNewT ConstRef view()
) extends NewT Gs
    to ConstNewT ConstRef
        return view()

    from(NewT Ref)
        from(
            newT Old swap(NewT)
                Old = NewT_Ref
                NewT_Ref = NewT
            NewT get()
                return NewT_Ref
            ConstNewT ConstRef view()
                return NewT_Ref
        )
```

Note that a `ref` type can be directly converted to a `Gsv` type.

Finally, you can have a `Grsv` type which can be gotten, removed, swapped,
or viewed.

```
# TODO: probably need some work for this constructor to work out correctly.
# need to make each G, R, S, V, its own class to avoid diamond problem.
class newT grsv(
    NewT Grs.remove()
    NewT Grs.get()
    newT Old Grs.swap(newT)
    ConstNewT ConstRef Gsv.view()
) extends NewT Gsv, NewT Grs;
```

