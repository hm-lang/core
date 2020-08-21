# Template/generic classes

Defining a generic or templated class has the following syntax.

## Single template type

For a class that will depend on a single type, you can use 
`newIdentifier` where `Identifier` is an `UpperCamelCase`
name of your choosing.  This effectively declares the new
type, and it appears before the class name, e.g. `class newT myClass;`.
You can then use `newT` inside the class as a type declaration:

```
class newT awesomeArray
    NewT This[Index]
        return newT(Index)

int awesomeArray A
print(A[3]) # should be 3
```

## Multiple template types

For classes with multiple types that should be defined, you can
use any number of `lowerCamelCase` identifiers before the class name
to specify which template types are being used in the class (though
the keyword `extends` is not allowed as a type!).  In the class body,
those identifiers can be used to declare variables of those types.

For example, we can declare a `map` class with two template types,
named `key` and `value`:

```
class key value map
    key DefaultKey
    value DefaultValue
    
    ...
    Value This(Key)
        ...
```

Named types in class template must be specified, similarly to an
argument specification:

```
(string Key, int Value) map Counter
counter["blah"] = 3
counter["heyeye"] += 10 # no throw, get() will instantiate a defualt integer
```

As is typical in hm-lang, the order of types specified for multiple template
type classes does not matter:

```
(int Value, string Key) map Counter     # also valid.
```
```

## Inheritance with templated classes

Note that any inheritance can be made explicit with the `extends` keyword:

```
class newT array extends [Index, NewT] Container;

class key value map extends [Key, Value] Container;
```

Right now there are no plans for making a "default" type
in a template class.  Instead, all types must be specified when
instantiating a class which is a generic/template class.

## Advanced: renamed template types

For a single template type class, we can rename the types:

```
# non-renamed:
Int array NormalArray
NormalArray.append(3)

# renamed type:
string Name array A
A.append(Name("Pat"))
```

For multi-template-type class, e.g. `class key value map` defined above,
we can also rename the types:

```
(int value Rating, string key Game) map Database
Database[Game("Pong")] = Rating(300)

(string key Name, int value Age) Map
Map[Name("Fred")] = Age(10)
