# Pointers

Let's create an example class `ANIMAL` with a string `type` variable, and
a few child classes `SNAKE` and `CAT` with their own extra variables.

```
class ANIMAL(STRING type = "??");

class SNAKE(SIZE length = 0) extends ANIMAL(Type("snake"));

class CAT(SIZE whiskers = 4) extends ANIMAL(Type("cat"));
```

To implement in C++, any instance of these classes will be pointers,
so that we can get inheritance working nicely out of the box.  These
pointers will be tucked away safely so that management will be easy.

## Locally-scoped variables

We can instantiate these classes in a few different ways.  In the following,
the instances belong to the variable and will be destroyed when the variable
goes out of scope:

```
ANIMAL base(Type("unknown"))    # internally a pointer to an `ANIMAL`.

ANIMAL? maybe1 = NEW(X(1))  # internally a nullable pointer to an `ANIMAL`.
ANIMAL? maybe2 = null       # can leave out the null initialization, that is the default.
```

Variables that are declared with an `ANIMAL` type are allowed to be instances of any
descendant, e.g., `SNAKE` or `CAT`, though only the parent-type's methods will be accessible.
`MAYBE` types (`?`), e.g. `ANIMAL?`, are similar -- they can be any descendant of the wrapped
type, but they can also be null.  Because of this, users are expected to check for null
before using `MAYBE` variables.  This will be a compiler feature.

```
ANIMAL base(Type("unknown"))
base = CAT(Whiskers(1))     # OK
base = SNAKE(Length(10))    # also ok
print(base.length)          # ERROR! NOT OK, length is not a property of ANIMAL.

# MAYBE types can also be changed to any of the related classes:
ANIMAL? maybe
maybe = CAT(Whiskers(100)
maybe = SNAKE(Length(1)
maybe = ANIMAL(Type("back-to-parent"))
maybe = null    # also ok.
# must check for null before using!!
if (maybe != null) print(maybe.type)
```


## Pointers to instances not belonging to the variable

For the `REF` and `REF?` types, the instance *must* outlive the variable and any copies made of the variable.

```
ANIMAL base(Type("liger"))
ANIMAL REF ref1 = base      # a `REF`erence doesn't own an instance, must get it elsewhere.

ANIMAL? maybe(Type("tigon"))
ANIMAL? REF ref2 = maybe    # a non-null reference to a possibly null ANIMAL.

ANIMAL REF? refQ    # nullable reference, which if non-null, points to a non-null ANIMAL
maybeRef = base     # ok

ANIMAL? REF? superQ # nullable reference, which if non-null, points to a nullable ANIMAL
superQ = maybe      # ok
```


## Abstract base class

Trying to instantiate an abstract base class without a default non-abstract child class
will fail in hm-lang:

```
class ACTOR
    MD nonAbstractMethod()
        # this method is defined, so doesn't make ACTOR abstract
        pass

    MD act();   # a non-initialized method is considered abstract
                # an abstract method makes a class abstract as well.

class GREAT_ACTOR extends ACTOR
    MD act()
        print "everyone cried tears of cathartic joy"

class DECENT_ACTOR extends ACTOR
    MD act()
        print "there was much rejoicing"

ACTOR actor     # ERROR!  ACTOR is abstract.
```

But you can set a default for the base class:

```
class STAR extends ACTOR
    MD act()
        print "even aliens heard and were amazed"

default ACTOR = STAR

ACTOR hiddenStar    # OK.
hiddenStar.act()    # prints "even aliens heard and were amazed"
```

Different defaults can be set for the same abstract base class, and used
on a per-file basis, but exactly how conflicts will be resolved still needs
to be decided.

The `default` operator also has some other use-cases [detailed here](./default.md).


## Implementation details

Maybe use the underlying C++ type in templates, but whenever using them as types, use a
wrapped value, e.g.:

```
template <typename T>
struct ScopedQ {
    typedef std::unique_ptr<T> Type;

    static T *get(Type &t) {
        return t.get();
    }
};

template <typename T>
struct Scoped {
    typedef std::unique_ptr<T> Type;

    static T *get(Type &t) {
        return t.get();
    }
};

template <>
struct Scoped<int> {
    typedef int Type;

    static int *get(Type &i) {
        return &i;
    }
};
// and so on, specifying all primitive types.
```

So that
```
class NEW_T PANCAKES(NEW_T? topping);
```
becomes
```
template <class NewT>
struct Pancakes {
protected:
    typename ScopedQ<NewT>::Type topping;

public:
    Pancakes(typename ScopedQ<NewT>::Type topping_) : topping(std::move(topping_)) {}

    typename ScopedQ<NewT>::Type getTopping() {
        return topping;
    }

    const typename ScopedQ<NewT>::Type &viewTopping() const {
        return topping;
    }

    typename ScopedQ<NewT>::Type setTopping(typename ScopedQ<NewT>::Type newTopping) {
        typename ScopedQ<NewT>::Type oldTopping = std::move(topping);
        topping = std::move(newTopping);
        return std::move(oldTopping);
    }
};
```
