# Types

Types are written in `lowerCamelCase` format, i.e., any identifier
that starts with a lower case letter.

Primitive types are written using abbreviations, e.g. `dbl` for double.  Still,
primitive types can be considered as first-class citizens, i.e., they
can be inherited from, and they can still have methods defined on them.

Non-primitive types include things like `string` for a sequence of characters,
`int` which is a big-integer type implementation, as well as classes created
by the user.  It is highly recommended to not use abbreviations
(besides standard ones, e.g. html) when defining a class name.

| Identifier | Type description             |
|------------|------------------------------|
| bool       | boolean (1 bit unsigned)     |
| dbl, f64   | 64-bit floating-point number |
| flt, f32   | 32-bit floating-point number |
| int        | arbitrary-bit signed integer |
| i64        | 64-bit signed integer        |
| i32        | 32-bit signed integer        |
| i16        | 16-bit signed integer        |
| i8         | 8-bit signed integer         |
| string     | sequence of characters       |
| u64        | 64-bit unsigned integer      |
| u32        | 32-bit unsigned integer      |
| u16        | 16-bit unsigned integer      |
| u8         | 8-bit unsigned integer       |

## Variables

Declaring a variable with a simple type follows these rules:

```
# these variables are default-initialized to 0:
dbl X   # declare a variable `X` which is a `dbl`
Int     # declare a variable `Int` which is an `int`

# you can also initialize the variable yourself.
string Greeting = "hello"   # declare a variable `Greeting` and assign it the value "hello"
String = "world!"           # declare a variable `String` and assign it the value "world!"
# those were both `string` types.
```

Variables that are declared are always initialized (perhaps lazily),
even if no definition is provided.  I.e., the variables `X` and `Int`
above are guaranteed to be 0 if read before getting set to something else.

Note that we do not allow C++ style instance definitions of variables;
you must use an equals sign `=` to define a non-default variable.

```
string InvalidString("hello world") # ERROR, compiler error.
string MyString = "hello world"     # ok!
string OtherString = (
    "hello world"     # also ok.
)
```

The problem with C++-style initialization is suppose we have an unnamed variable
(e.g. `int Int` or `myClass MyClass`) which doesn't have its type explicitly written out.
It will look a bit like a hash-table definition (`ValueType[KeyType]`) when trying
to initialize it.  This also makes it more consistent with default function arguments,
which require `=`.  This code snippet illustrates the problem:

```
class myClass(SomeType);

SomeType = 5
myClass A(SomeType: 3)  # not ambiguous
myClass B(SomeType)     # ERROR. a little ambiguous.  looks a bit lik a hash-table of MyClass[SomeType]
MyClass(SomeType: 5)    # ERROR. a little ambiguous, not sure what we're trying to do here.
MyClass(SomeType)       # ERROR. very ambiguous.  looks like a hash-table type.
```

Since any ambiguity is bad, we disallow C++-style initialization.


## Functions, arrays, and map types

Functions use lowerCamelCase for their names.  Note that when specifying types
for functions, arrays, and maps, we need a named type (e.g. `int Name` or
`Int` which is an `int Int`).  Names allow us to ensure that the order of
inputs/outputs to these complex types doesn't matter.

```
# declare a function named `doStuff` that takes a `string` argument
Int doStuff(String)

# declare an (resizable) array named `Values`;
# equivalent to `Int array Values`:
Int[] Values

# declare an array with a fixed size named `Vector`;
# maybe equivalent to `Dbl array Vector = (FixedSize: 3)`
Dbl[3] Vector
# also equivalent:
Dbl Vector[3]

# declare a linked hash map of strings to integers;
# equivalent to `(string Key, int Value) map NameToIdMap`:
# or also `Int NameToIdMap[String]`
Int[String] NameToIdMap

# declare a hash map of Ints to Strings, and reserve some space:
String[Int] IdToNameMap = (Reserve: 5)
# equivalent to `String[Int] IdToNameMap = Reserve: 5`, or also:
String IdToNameMap[Int] = (Reserve: 5)

# declare a hash map of Strings to Functions:
fn() FunctionMap[String]
FunctionMap["hello"] = $$print("hello world!")
# TODO: determine which of these is the right way to call:
FunctionMap["hello"]()      # prints "hello world!"
FunctionMap["hello"]:fn()   # prints "hello world!"
FunctionMap["hello"]::fn()  # prints "hello world!"
FunctionMap["hello"] fn()   # prints "hello world!"

# declare a hash map of a named function with variables
Int hash(String) HashHashMap[string Name]
HashHashMap[Name: "fast hash!"] = hash(String)
    return 3
# TODO: determine which of these is the right way to call:
HashHashMap["fast hash!"]::hash("hello world!")
HashHashMap["fast hash!"] hash("hello world!")

# declare a hash map that has a key composed of two types,
# one of which is named:
Dbl[String, int Priority] FancyMap
# also equivalent:
Dbl FancyMap[String, int Priority]
# using it:
FancyMap["jam", Priority: 3] = 1.234
FancyMap["jam", Priority: 1] = 2.5

# declare a hash map of a type that has a named value:
# equivalent to `(string key Name, int value Id) map Database`
int Id Database[string Name]
# TODO: can we also do this??  not sure how grippy the syntax is:
int Id[string Name] Database
Database[Name: "Kelly"] = Id: 123
int Id = Database[Name: "John"]
# TODO: update ./functions.md, this doesn't seem right.  id would have to be a class:
Int id KellyId = Database[Name: "Kelly"]
# TODO: maybe use "from" or "as":
int KellyId from Id = Database[Name: "Kelly"]
int KellyId as Id = Database[Name: "Kelly"]
# TODO: add syntax to ./functions.md for single named return value, maybe this or ::
int KellyId = Database[Name: "Kelly"]?.Id
```

Note that the `map` type is actually an abstract class, so the exact
child class (e.g. `hashMap` or `linkedHashMap`) that is used will
depend on a particular design choice made in hm-lang.  Likely we will
want to make the default `map` subclass as surprise-free as possible,
rather than as fast as possible.  I.e., we will likely use a
`linkedHashMap`, where the order of insertion will be reflected in the
order of iteration.
