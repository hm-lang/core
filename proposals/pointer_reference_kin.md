# Pointers

Because our first milestone is to transpile to C++, we want to re-use some
useful concepts from this language, but with hm-lang's unique spin.
Our top user-facing goals are ease-of-use and native-executable speed/efficiency,
with greater pointer safety baked in.

Let's create an example class `ANIMAL` with a string `type` variable, and
a few child classes `SNAKE` and `CAT` with their own extra variables.

```
class ANIMAL(STRING type = "??");

class SNAKE(SIZE length = 0) extends ANIMAL(Type("snake"));

class CAT(SIZE whiskers = 4) extends ANIMAL(Type("cat"));
```


## Locally-scoped variables

We can instantiate these classes in a few different ways.  In the following,
the instances belong to the variable and will be destroyed when the variable
goes out of scope:

```
ANIMAL baseClass(Type("unknown"))   # internally does not use a pointer

ANIMAL KIN kin(Type("general"))     # internally a pointer to an `ANIMAL`.

ANIMAL? maybe1 = NEW(X(1))  # internally a nullable pointer to an `ANIMAL`.
ANIMAL? maybe2 = null       # can leave out the null initialization, that is the default.
```

We use the `KIN` type to name an owned pointer.  It is essentially a C++ `std::unique_ptr`,
but with the added feature that if the pointer is null (uninitialized) when asked for,
a default instance of the wrapped type will be created.  The `KIN` type is useful because
any descendant of the wrapped type is also allowed to be placed inside the variable --
thus the kinship.  We might take `!` as a short-hand for `KIN`, but we need to
check with [lazy initialization](./lazy_initialization.md) to see if we can simplify.

`MAYBE` types, e.g. `ANIMAL?`, are similar -- they can be any descendant of the wrapped
type, but they can also be null.  To avoid multiple ways of doing things, a `KIN?` type will
throw a compiler error, recommending instead the unwrapped type followed by a `?`.
Because `MAYBE` variables are allowed to be null, users are expected to check for this before
using them.  This will be a compiler feature.

```
# this variable cannot be changed to a subclass, e.g. SNAKE or CAT:
ANIMAL baseClass(Type("unknown"))
baseClass = CAT(Whiskers(1)) # ERROR!!!

# KIN types can be reset to any of the classes which descend from the original wrapped type:
ANIMAL KIN kin(Type("shapeshifter"))
kin = SNAKE(Length(100))
kin = CAT(Whiskers(10))
kin = ANIMAL(Type("back-to-parent")) # also fine.

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
ANIMAL baseClass(Type("liger"))
ANIMAL KIN kin(Type("tigon"))
ANIMAL REF ref1 = baseClass # a `REF`erence doesn't own an instance, must get it elsewhere.
ANIMAL REF ref2 = kin       # also ok.

ANIMAL REF? maybeRef1       # starts out at null by default
ANIMAL REF? maybeRef2 = kin
maybeRef2 = baseClass       # ok
maybeRef2 = null
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

ACTOR KIN kin   # run-time error if kin.act() is called before actor is initialized with a non-abstract class
kin = DECENT_ACTOR()    # OK
```

But you can set a default for the base class:

```
class STAR extends ACTOR
    MD act()
        print "even aliens heard and were amazed"

default ACTOR = STAR

ACTOR hiddenStar    # OK.
hiddenStar.act()    # prints "even aliens heard and were amazed"

ACTOR KIN hiddenStarKin # no more run-time error even if not initialized properly
hiddenStarKin.act() # OK
```

Different defaults can be set for the same abstract base class, and used
on a per-file basis, but exactly how conflicts will be resolved still needs
to be decided.

The `default` operator also has some other use-cases [detailed here](./default.md).


## Obscure hm-lang history

An early proposal was to make all class instances effectively pointers, so
that we could do things like this, for example:

```
MAP Key(STRING) Value(INT) employeeIdMap = HASH_MAP(ExpectedSize(10))
...
employeeIdMap = DLL_HASH_MAP(employeeIdMap) # convert to a DLL_HASH_MAP
```

The upsides of this approach are:

* it is very easy to perform moves/swaps between variables.

* it does make things easier to use, since child types can be immediately
  assigned to parent type variables.

The downsides of this approach include:

* many pointers need to be allocated everywhere, possibly leading to worse
  memory fragmentation; there is better memory locality with the opposite approach.

* it differs than the usual C++ approach, so the transpilation would require two
  classes for each hm-lang class.  We would have an internal class, corresponding
  to the real C++ class, and a different class -- essentially the C++ class
  wrapped in a  `std::unique_ptr`.  We get even more redirects as we start wrapping
  the class in e.g. `MAYBE` (`?`) or `REF` types.
