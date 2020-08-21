# Default vs. abstract

In hm-lang, some base classes are abstract; i.e., they cannot be directly
instantiated.  For ease of use, however, when someone attempts to
instantiate a base class, the default class for that base class will be
instantiated instead.  (Child class instances are assignable to a parent
class variable, if it is not [`const`](./var_vs_const.md).)  Default child
classes can be declared using a `default.` prefix on the original parent
class name, e.g.:

```
class greeter
    sayHello(string To)
        print("${getGreeting()}, ${To}!")

    # declare a class method but do not define it:
    # this is how you make it an abstract method (and thus abstract class):
    string getGreeting();

class default.greeter
    string getGreeting()
        return "hello"

greeter G
G.sayHello() # does not error
```

Note that if nothing inside of the parent class had been defined as abstract,
then the parent class would have been instantiable, and defining a default
would have thrown a compile-time error.  Classes cannot be directly declared
as abstract, but methods are abstract when declared but not defined, and
having any abstract methods defines an abstract class.

In case the child class has a more specific name, and we want to set it as
the default for the parent type, we use this syntax:

```
class key value map
    # define an abstract method:
    Value Grs This[Key];

class key value linkedHashMap extends key value map
    # implement the method:
    Value Grs This[Key]
        ... implementation ...

default.map = linkedHashMap
```

Only one default can be set for a parent class, and it cannot be overridden.
