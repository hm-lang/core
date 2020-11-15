# Nested classes

We can create nested classes in two ways, either by directly nesting
or by defining the class with the parent class prefix.  The latter
helps to avoid the indent limit.

```
class Example
    # define a nested class in the first way:  direct nesting:
    class NestedClass(Int, String)
        doSomething()
            for int I < Int
                print(string)

    # define a protected nestedClass instance, NestedClass:
    nestedClass _
    setNestedClass(nestedClass New)
        NestedClass = New

    # define a protected otherNestedClass instance, OtherNestedClass:
    otherNestedClass _
    setOtherNestedClass(otherNestedClass New)
        OtherNestedClass = New 

# define a nested class in the second way:  parent prefix:
# Note this definition doesn't have to come before the first usage above:
class example.OtherNestedClass(int Zazzle);

example Ex
Ex.setOtherNestedClass(Zazzle(1))
print(Ex.OtherNestedClass.Zazzle)

example.nestedClass Nest(5, "hi")
Ex.setNestedClass(Nest)
Ex.NestedClass.doSomething()
```

Nested classes are a good candidate for protected/private status.
See [public/protected/private](./public_protected_private.md) for more
discussion.

## Implementation

We probably would not actually do any nesting in C++, although that is possible.
To make it easier to reference nested classes from outside the unnested class,
we will probably do something like this:

```
class Example {
 /** logic **/
};

class Example__NestedClass {
 /** logic **/
};

class Example__OtherNestedClass {
 /** logic **/
};
```
