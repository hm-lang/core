# Getter/setter properties

We make it easy to wrap a variable within getter/setter functions
using the `GS` (Getter Setter) class.  The proposed API is:

```
    class NEW_T GS(NEW_T FN get(), FN set(NEW_T))
        to NEW_T
            return get()

        MD this = (NEW_T)
            set(newT)
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

Here is a getter/setters which is also deletable.  Note that the get/set functions
need to handle the case in which something is deleted; i.e., they can throw
or return something else if it is reasonable.  Similarly, the `delete` function
must do something reasonable if it is called twice, or throw.

```
    class NEW_T DGS(FN delete(), NEW_T FN GS.get(), FN GS.set(NEW_T)) extends NEW_T GS
        MD this = null
            delete()

        MD this = (NEW_T?)
            if newTQ
                set(newTQ)
            else
                delete()
```

Here is a getter/setter that is also viewable, i.e., no copies will be made
if we ask for just a view of the internal value.

```
    class NEW_T GSV(
        NEW_T FN GS.get()
        FN GS.set(NEW_T)
        CONST_NEW_T REF FN view()
    ) extends NEW_T GS
        to CONST_NEW_T REF
            return view()

        from(NEW_T REF)
            from(
                FN set(NEW_T)
                    newTRef = newT
                NEW_T FN get()
                    return newTRef
                CONST_NEW_T REF view()
                    return newTRef
            )
```

Note that the `REF` type can be directly converted to a `GSV` type.

Finally, you can have a `DGSV` type which can be deleted, gotten, set,
or viewed.

```
    # TODO: probably need some work for this constructor to work out correctly.
    class NEW_T DGSV(
        FN DGS.delete()
        NEW_T FN DGS.get()
        FN DGS.set(NEW_T)
        CONST_NEW_T REF FN GSV.view()
    ) extends NEW_T GSV, NEW_T DGS;
```
