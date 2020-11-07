# Types

Types are written in `lowerCamelCase` format, i.e., any identifier
that starts with a lower case letter.

Primitive types are written using abbreviations, e.g. `int` for integer
(specifically a 64-bit signed integer) and `dbl` for double.  Still,
primitive types can be considered as first-class citizens, i.e., they
can be inherited from, and they can still have methods defined on them.

Non-primitive types include things like `string` for an array of
characters, as well as classes created by the user.  It is highly
recommended to not use abbreviations (besides standard ones, e.g. html)
when defining a class name.

| Identifier | Type description             |
|------------|------------------------------|
| bool       | boolean (1 bit unsigned)     |
| dbl, f64   | 64-bit floating-point number |
| flt, f32   | 32-bit floating-point number |
| int, i64   | 64-bit signed integer        |
| i32        | 32-bit signed integer        |
| i16        | 16-bit signed integer        |
| i8         | 8-bit signed integer         |
| string     | array of characters/letters  |
| uint, u64  | 64-bit unsigned integer      |
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
above are guaranteed to be 0.

Note that we don't allow C++ style instance definitions of variables;
we always require an equals sign `=` to define a non-default variable.

```
string MyString("hello world")      # ERROR!  ok in C++, not in hm-lang.
string MyString = "hello world"     # ok!
```

## Functions, arrays, and map types

In hm-lang, all parentheses are equal: `() == {} == []`, but of course
they must match at each level.  They can be used to define functions,
arrays, and maps, depending on the context.  Functions use lowerCamelCase
for their names.  Note that when specifying types for functions, arrays,
and maps, we need a named type (e.g. `int Name` or `Int` which is an `int Int`).
Names allow us to ensure that the order of inputs/outputs to these complex types
doesn't matter.

```
# declare a function named `doStuff` that takes a `string` argument
Int doStuff(String)

# declare an (resizable) array named `Values`;
# equivalent to `Int array Values`:
Int[] Values

# declare an array with a fixed size named `Vector`;
# equivalent to `Dbl fixedArray Vector = new(Size(3))`
Dbl[3] Vector

# declare a hash map of strings to integers;
# equivalent to `(string Key, int Value) map NameToIdMap`:
Int[String] NameToIdMap

# declare a hash map of Ints to Strings, and reserve some space:
String[Int] IdToNameMap = new(Reserve(5))

# declare a hash map that has a key composed of two types,
# one of which is named:
Dbl[String, int Priority] FancyMap
FancyMap["jam", Priority(3)] = 1.234
FancyMap["jam", Priority(1)] = 2.5
```

Note that the `map` type is actually an abstract class, so the exact
child class (e.g. `hashMap` or `linkedHashMap`) that is used will
depend on a particular design choice made in hm-lang.  Likely we will
want to make the default `map` subclass as surprise-free as possible,
rather than as fast as possible.  I.e., we will likely use a
`linkedHashMap`, where the order of insertion will be reflected in the
order of iteration.
