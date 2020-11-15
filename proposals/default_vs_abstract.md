# Default vs. abstract

In hm-lang, some base classes are abstract; i.e., they cannot be directly
instantiated.

```
class Greeter
    sayHello(string To)
        print("${getGreeting()}, ${To}!")

    # declare a class method but do not define it:
    # this is how you make it an abstract method (and thus abstract class):
    String getGreeting();

greeter G   # ERROR! Greeter has no default class, it's abstract and not instantiable.
```

Classes cannot be directly declared as abstract,
but methods are abstract when declared but not defined,
and having any abstract methods defines an abstract class.

To make life easier for users who don't care about the specific implementation
of some abstract class, we can define a default implementation for that class.


## Unnamed default classes

Default child classes can be declared using a `default.` prefix on the original parent
class name, e.g.:

```
class default.Greeter
    # make a beautiful implementation of the parent's abstract class:
    String getGreeting()
        return "hello"

greeter G
G.sayHello() # does not error
```

Note you don't have to use `extends Greeter` in the class definition.


## Named default classes

In case the child class has a more specific name, and we want to set it as
the default for the parent type, we use this syntax:

```
class (Key, Value) Map
    # define an abstract method:
    Value gate.Removable This[Key];

class (Key, Value) LinkedHashMap extends (Key, Value) Map
    # implement the method:
    Value gate.Removable This[Key]
        ... implementation ...

default Map = LinkedHashMap
```

Only one default can be set for a parent class, and it cannot be overridden.

## Automatic lambda implementations for abstract classes

TODO
