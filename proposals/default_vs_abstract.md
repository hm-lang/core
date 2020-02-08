# Default vs. abstract

In hm-lang, some base classes are abstract; i.e., they cannot be directly
instantiated.  For ease of use, however, when someone attempts to
instantiate a base class, the default class for that base class will be
instantiated instead.  (Child class instances are assignable to a parent
class variable, if it is not [`CONST`](./var_vs_const.md).)  Default child
classes can be declared using a `DEFAULT_` prefix on the original parent
class name, e.g.:

```
class GREETER
    MD sayHello(STRING to)
        print("${getGreeting()}, ${to}!")

    STRING ABSTRACT_MD getGreeting()

class DEFAULT_GREETER
    STRING MD getGreeting()
        return "hello"

GREETER g
g.sayHello() # does not error
```

Note that if nothing inside of the parent class had been defined as abstract,
then the parent class would have been instantiable, and defining a default
would have thrown a compile-time error.  Classes cannot be directly defined
as abstract, but methods can be defined as abstract by using `ABSTRACT_MD`
instead of `MD`.

In case the child class has a more specific name, and we want to set it as
the default for the parent type, we use this syntax:

```
class MAP KEY VALUE
    # ...
    VALUE GS ABSTRACT_MD this[KEY]

class LINKED_HASH_MAP KEY VALUE extends KEY VALUE MAP
    # ...
    VALUE GS MD this[KEY]
        # implementation

DEFAULT_MAP = LINKED_HASH_MAP
```

Only one default can be set for a parent class, and it cannot be overridden.
