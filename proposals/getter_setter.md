# Getter/setter properties

We make it easy to wrap a variable within getter/setter functions
using the `GS` (Getter Setter) class.  The proposed API is:

```
    class NEW_T GS
        from(FN set(NEW_T), NEW_T FN get());

        MD this = (NEW_T)
            set(newT)

        to NEW_T
            return get()
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
