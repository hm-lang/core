# Getter/setter properties

We make it easy to wrap a variable within getter/setter functions
using the `GS` (Getter Swapper) class.  The proposed API is:

```
    class NEW_T GS(NEW_T FN get(), NEW_T old FN swap(NEW_T))
        to NEW_T
            return get()

        MD this = (NEW_T)
            swap(newT)
```

Generally speaking, what looks like a class member variable is actually
hidden behind a getter/setter.  This is so that child classes can easily
change the behavior of how the variables work.  This might be harder to
implement than expected, but here is the general idea:

```
    class VECTOR2(DBL x, DBL y);

    class UNIT_VECTOR2 extends VECTOR2
        from(DBL x, DBL y)
            normalize(x, y)

        get DBL x
            return super.x
        get DBL y
            return super.y

        set DBL x
            normalize(new.x, y)
        set DBL y
            normalize(x, new.y)

        MD normalize__(DBL new.x, DBL new.y)
            DBL r = (new.x^2 + new.y^2)^0.5
            assert(r > 0, OrThrow("Cannot normalize to a unit vector!"))
            super.x = new.x / r
            super.y = new.y / r
```

In the latter case, we can access the original class's getters/setters
using the `super` keyword.


## Other mixins

Here is a getter/setters which is also removable.  Note that the get/set functions
need to handle the case in which something is removed; i.e., they can throw
or return something else if it is reasonable.  Similarly, the `remove` function
must do something reasonable if it is called twice, or throw.

```
    class NEW_T GRS(NEW_T FN remove(), NEW_T FN GS.get(), NEW_T old FN GS.swap(NEW_T)) extends NEW_T GS
        MD this = null
            remove()

        MD this = (NEW_T?)
            if newTQ
                swap(newTQ)
            else
                remove()
```

Here is a getter/setter that is also viewable, i.e., no copies will be made
if we ask for just a view of the internal value.

```
    class NEW_T GSV(
        NEW_T FN GS.get()
        NEW_T old FN GS.swap(NEW_T)
        CONST_NEW_T REF FN view()
    ) extends NEW_T GS
        to CONST_NEW_T REF
            return view()

        from(NEW_T REF)
            from(
                NEW_T old FN swap(NEW_T)
                    old = newTRef
                    newTRef = newT
                NEW_T FN get()
                    return newTRef
                CONST_NEW_T REF view()
                    return newTRef
            )
```

Note that the `REF` type can be directly converted to a `GSV` type.

Finally, you can have a `GRSV` type which can be gotten, removed, swapped,
or viewed.

```
    # TODO: probably need some work for this constructor to work out correctly.
    # might need to make each G, R, S, V, its own class to avoid diamond problem.
    class NEW_T GRSV(
        NEW_T FN GRS.remove()
        NEW_T FN GRS.get()
        FN GRS.set(NEW_T)
        CONST_NEW_T REF FN GSV.view()
    ) extends NEW_T GSV, NEW_T GRS;
```

## Simplify it??

Unfortunately, these becomes run-time errors instead of compile-time errors:

```
    # TODO: possibly make these FN's abstract methods (MDs):
    # then pointer types can just extend GATE.  we could create
    # a LAMBDA_GATE class which does this:
    class NEW_T GATE(
        NEW_T FN get()
        NEW_T old FN swap(NEW_T) = throw ERROR("unimplemented swap")
        NEW_T FN remove() = throw ERROR("unimplemented remove")
        CONST_NEW_T REF FN view() = throw ERROR("unimplemented view")
    )
        to NEW_T
            return get()

        to CONST_NEW_T REF
            return view()

        NEW_T MD this = (NEW_T)
            return swap(newT)

        # TODO - make setting equal be NIM-like:
        NEW_T MD this = (NEW_T?)
            if newTQ == null
                return remove()
            return swap(newTQ)
```
