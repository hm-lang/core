# Pointers

Let's create an example class `animal` with a string `type` variable, and
a few child classes `snake` and `cat` with their own extra variables.

```
class animal(string Type = "??");

class snake(size Length = 0) extends animal(Type("snake"));

class cat(size Whiskers = 4) extends animal(Type("cat"));
```

To implement in C++, any instance of these classes will be pointers,
so that we can get inheritance working nicely out of the box.  These
pointers will be tucked away safely so that management will be easy.

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
if (Maybe != Null) print(Maybe.Type)
```


## Pointers to instances not belonging to the variable

For the `ref` and `ref?` types, the instance *must* outlive the variable and any copies made of the variable.

```
animal Base(Type("liger"))
animal ref Ref1 = Base      # a `ref`erence doesn't own an instance, must get it elsewhere.

animal? Maybe(Type("tigon"))
animal? ref Ref2 = Maybe    # a non-null reference to a possibly null animal.

animal ref? RefQ    # nullable reference, which if non-null, points to a non-null animal
RefQ == Null        # True!
RefQ = Base         # ok
RefQ != Null        # True!

animal? ref? QQ     # nullable reference, which if non-null, points to a nullable animal
QQ == Null          # True!
Maybe = Null        
QQ = Maybe          # ok
QQ == Null          # True, since Maybe is Null.
Maybe = new(Type("spidephant"))
QQ != Null          # True, Maybe is now non-Null.
```

