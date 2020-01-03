# Nested classes

We can create nested classes in two ways, either by directly nesting
or by defining the class with the parent class prefix.  The latter
helps to avoid the indent limit.

```
class EXAMPLE
    class NESTED_CLASS(INT, STRING)
        MD doSomething()
            for INT i < int
                print(string)

    NESTED_CLASS _
    MD setNestedClass(NESTED_CLASS new)
        nestedClass = new.nestedClass 

    OTHER_NESTED_CLASS _
    MD setOtherNestedClass(OTHER_NESTED_CLASS new)
        otherNestedClass = new.otherNestedClass 

# Note this definition doesn't have to come before the first usage above:
class EXAMPLE.OTHER_NESTED_CLASS(INT zazzle);

EXAMPLE ex
ex.setOtherNestedClass(NEW(Zazzle(1)))
# TODO: make this syntax also work:
# ex.setOtherNestedClass(Zazzle(1))
print(ex.otherNestedClass.zazzle)

EXAMPLE.NESTED_CLASS nest(5, "hi")
ex.setNestedClass(nest)
ex.nestedClass.doSomething()
```

Nested classes are a good candidate for protected/private status.
See [public/protected/private](./public_protected_private.md) for more
discussion.
