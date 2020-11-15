# Pointers

Let's create an example class `animal` with a string `type` variable, and
a few child classes `snake` and `cat` with their own extra variables.

```
class Animal(string Type = "??");

class Snake(size Length = 0) extends Animal(Type("snake"));

class Cat(size Whiskers = 4) extends Animal(Type("cat"));
```

To implement in C++, any instance of these classes will be pointers,
so that we can get inheritance working nicely out of the box.  These
pointers will be tucked away safely so that management will be easy.
TODO: consider making these `std::variant`s under the hood, see if that
performance benefit is better (no heap allocation).

## Locally-scoped variables

We can instantiate these classes in a few different ways.  In the following,
the instances belong to the variable and will be destroyed when the variable
goes out of scope:

```
animal Base(Type("unknown"))    # internally a pointer to an `animal`.

animal? Maybe1 = new(X(1))  # internally a nullable pointer to an `animal`.
animal? Maybe2 = Null       # can leave out the null initialization, that is the default.
```

Variables that are declared with an `animal` type are allowed to be instances of any
descendant, e.g., `snake` or `cat`, though only the parent-type's methods will be accessible.
`maybe` types (`?`), e.g. `animal?`, are similar -- they can be any descendant of the wrapped
type, but they can also be null.  Because of this, users are expected to check for null
before using `maybe` variables.  This will be a compiler feature.

```
animal Base(Type("unknown"))
base = Cat(Whiskers(1))     # OK
base = Snake(Length(10))    # also ok
print(Base.Length)          # ERROR! NOT OK, length is not a property of animal.

# MAYBE types can also be changed to any of the related classes:
animal? Maybe
Maybe = cat(Whiskers(100)
Maybe = snake(Length(1)
Maybe = animal(Type("back-to-parent"))
Maybe = Null    # also ok.
# must check for null before using!!
if Maybe != Null { print(Maybe.Type) }
```


## Pointers to instances not belonging to the variable

### References

For the `ref` and `ref?` types, the instance *must* outlive the variable and any copies made of the variable.

```
animal Base(Type("liger"))
# TODO: This is kinda ugly: `Base` should automatically be a `ref` type.
# I can see why Stroustrup didn't want ref's to be reassignable...
Animal ref Ref1 = new(Base)    # a `ref`erence doesn't own an instance, must get it elsewhere.

animal? Maybe(Type("tigon"))
Animal? ref Ref2 = new(Maybe) # a non-null reference to a possibly null animal.

Animal ref? RefQ    # nullable reference, which if non-null, points to a non-null animal
RefQ == Null        # True!
RefQ = new(Base)    # this is how you switch the Ref
RefQ != Null        # True!
RefQ = new(Null)    # this is how to reset to a null Ref

Animal? ref? QQ     # nullable reference, which if non-null, points to a nullable animal
QQ == Null          # True!
Maybe = Null        # resetting the Maybe
QQ = new(Maybe)     # ok. also ok to do `QQ = ref(Maybe)`
QQ == Null          # True, since Maybe is Null.
Maybe = animal(Type("spidephant"))
QQ != Null          # True, Maybe is now non-Null.
QQ = Null           # NOTICE! this resets the variable Maybe to Null.
QQ = new(Null)      # reset the Ref to not point to anything
```

### Views

A view is a way to check in on the value of some variable without being able to modify it directly.

```
int MyInt = 3
Int view MyView(MyInt)
MyView = 5  # ERROR! does not compile.  MyView cannot modify MyInt, and cannot hold a reference to a temporary.

# since MyView is not a constView (but just a view), it can be switched using this:
int MyOtherInt = 7
MyView = View(MyOtherInt)
```

Note that `Type view` may be thought of as a `ConstType ref` class.
