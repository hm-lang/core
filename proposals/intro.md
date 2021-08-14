# overview of types

Standard types whose instances can take up an arbitrary amount of memory:

* `int`: signed big-integer
* `rtl`: rational number (e.g. an `int` over an `int`)
* `str`: array/sequence of utf8 bytes, but note that `string` is preferred in code
    since it includes other containers which deterministically provide utf8 bytes.

Other types which have a fixed amount of memory:

* `null`: should take up no memory, but can require an extra bit for an optional type.
* `flt`: single-precision floating-point number, AKA `f32`
* `dbl`: double-precision floating-point number, AKA `f64`
* `bool`: can hold a True or False value
* `rune`: a utf8 character, presumably held within an `i32`
* `u8`: unsigned byte (can hold values from 0 to 255, inclusive)
* `u16` : unsigned integer which can hold values from 0 to 65535, inclusive
* `u32` : unsigned integer which can hold values from 0 to 2^32 - 1, inclusive
* `u64` : unsigned integer which can hold values from 0 to 2^64 - 1, inclusive

and similarly for `i8` to `i64`, using two's complement.  For example,
`i8` runs from -128 to 127, inclusive, and `u8(i8(-1))` equals `255`.

TODO: support for `u128` to `u512`, as well as `i128` to `i512`

## optional types

```
# TODO: do we want `X: int?` or `X: int|null`
```

# declaring and using variables

Variables are named using `UpperCamelCase` identifiers.  The `:` symbol is used
to declare non-reassignable variables, and `;` is used to declare reassignable
variables.

```
# declaring and setting a non-reassignable variable that holds a big integer
Y: int = 5
# also equivalent: `Y := 5` or `Y := int(5)`.

# using the variable:
print(Y * 30)

Y = 123     # COMPILER ERROR, Y is non-reassignable.
```

Reassignable variables can use `VariableName = Expression` after their first
initialization, but they must be declared with a `;` symbol.

```
# declaring a reassignable variable that holds a big integer
X; int

# X is default-initialized to 0 if not specified.
X += 5      # now X == 5 is True.

# you can also define the value inline as well:
W ;= 7
# also equivalent, if you want to be explicit about the type.
W; int = 7
# also equivalent:
W ;= int(7)
```

## more on non-reassignable variables

You can define variables which cannot be reassigned, but note that if the
type is more complicated (i.e., with nested fields), a non-reassignable
property does not necessarily mean that the variable is deeply constant.
I.e., object instances are deeply immutable, but class instances are not.

```
# declaring and setting a non-reassignable variable that holds a big integer
Z := 10     # or `Z := int(10)` if you want to be explicit

Z += 7      # COMPILER ERROR!! Z is non-reassignable.
```

You can also make a variable non-reassignable for the remainder of the current block
by using `::` prefix before the variable name.

```
X; int = 4  # defined as reassignable

if SomeCondition
    ::X = 7 # locks X after assigning it.
            # For the remainder of this indented block, you can use X but not reassign it
else
    ::X     # lock X to whatever value it was for this block.
            # You can still use X but not reassign it.

print(X)    # will either be 7 (if SomeCondition was true) or 4 (if !SomeCondition)
X += 5      # can modify X back in this block; there are no constraints here.
```

## nested/object types

You can declare an object type inline with nested fields.  Objects defined
like this are immutable; i.e., their fields are non-reassignable.

```
Vector; (X: dbl, Y: dbl, Z: dbl) = (X: 4, Y: 3, Z: 1.5)
Vector.X += 4   # COMPILER ERROR, object is immutable

# note however, as defined, Vector is reassignable since it was defined with `;`:
Vector = (X: 1, Y: 7.2)
# note, missing fields will be default-initialized.
Vector.Z == 0   # should be True.

# to make an object variable non-reassignable, use := when defining:
Vector2 := (X: 3.75, Y: 3.25)
# or you can use `:` with an explicit type specifier and then `=`:
Vector2: (X: dbl, Y: dbl) = (X: 3.75, Y: 3.25)
# then these operations are invalid:
Vector2.X += 3          # COMPILER ERROR, object is immutable
Vector2 = (X: 1, Y: 2)  # COMPILER ERROR, variable is non-reassignable
```

You can define a type/interface for objects you use multiple times.

```
vector2 := (X: dbl, Y: dbl)

# you can use `vector2` now like any other type, e.g.:
Vector2 := vector2(X: 5, Y: 10)
```

TODO: do we want to allow type definitions with mutable fields, e.g. (X; int, Y; dbl)
probably ok for function arguments, but need to be careful with hash stability.
e.g., (X: int, Y: int) can be used as a hash-table key, but (X; int, Y; int) cannot.

## hiding variables for the remainder of the block

TODO.  descopes/destructs if the variable was declared in that block.

# declaring, defining, and using a function

Functions are named using `lowerCamelCase` identifiers.  All functions
must return something, and `null` is a valid return type.  (There is no "void" type.)

```
# declaring a function with no arguments that returns a big integer
v(): int

# setting/defining/initializing the function usually requires an indent+1 block following:
v(): int
    return 600

# but in simple cases like this you can also define inline:
# TODO: Double check this is OK
v() := 600

# function with X,Y double-precision float arguments that returns nothing
v(X: dbl, Y: dbl): null
    print("X = ${X}, Y = ${Y}, atan(Y, X) = ${math.atan(X, Y)}")

# function that takes a function as an argument and returns a function
wow(Input.fn(): string): fn(): int
    return fn(): int
        return Input.fn().size()
```

You can call functions with arguments in any order.  Arguments must be specified
with the named identifiers in the function definition.

```
# definition:
v(X: dbl, Y: dbl): null

# example calls:
v(X: 5.4, Y: 3)
v(Y: 3, Y: 5.4)

# if you already have variables X and Y, you don't need to re-specify their names:
X := 5.4
Y := 3
v(X, Y)     # preferred.  also ok: `v(X: X, Y: Y)` but not idiomatic.
v(Y, X)     # equivalent
```

## unnamed arguments in functions

For functions with a single argument where the variable name doesn't matter,
you can use "unnamed" variables.  For primitive types, the "unnamed" identifier
is just the UpperCamelCase version of the lowerCamelCase type.

```
# this function declaration is equivalent to `f(Int: int): int`:
f(Int): int
    return Int + 5

Z := 3
f(Z)        # ok
f(int(4.3)) # ok
f(5)        # ok
```

If passing functions as an argument where the function name doesn't matter,
the "unnamed" function is `fn`.

```
q(fn(): bool): null
    if fn()
        print("function returned true!")
    else
        print("function returned false!")

q(nameItWhatYouWant(): bool
    return True
)   # should print "function returned true!"

# can pass in functions from outside as well:
anotherTestFunction(): bool
    return False
q(anotherTestFunction)  # should print "function returned false!"

# or you can create a nameless function yourself:
q(fn(): bool
    return random()
)   # will print one of the above due to randomness.
```

## redefining a function

Just like with variables, to declare a reassignable function, use `;` instead of `:`.

```
greetings(Noun: string); null
    print "Hello, ${Noun}!"

# you can use the function:
greetings(Noun: "World")

# or redefine it:
# option 1:
greetings = fn(Noun: string); null
    print "Hi, ${Noun}!"
# option 2:
greetings = "Greetings, ${$Noun}!"
# NOT OK: COMPILE ERROR: this looks like a redeclaration of the function, rather than a reassignment:
greetings(Noun: string); null
    print "Overwriting?"
```

## function overloads

Functions can do different things based on the number/type of arguments passed in.

```
greetings(String): null
    print "Hello, ${String}!"

greetings(Say: string, To: string): null
    print "${Say}, ${To}!"

greetings(Say: string, To: string, Times: int): null
    for Count: int < Times
        greetings(Say, To)

# so you call this in different ways:
greetings("World")
greetings(To: "you", Say: "Hi")
greetings(Times: 5, Say: "Hey", To: "Sam")
```

Note that you can define the function arguments as reassignable (with `;`) or
non-reassignable (with `:`) but that does **not** change the type of function.
Whether the variables can be reassigned inside the function does not matter
to the interface between function and caller.

```
# Note, these are not real overloads, and defining both would throw a COMPILE ERROR

greetings(Say: string): null
    print "${Say}, world!"

greetings(Say; string): null
    Say += " wow"
    print "${Say}, world..."

# COMPILE ERROR
```

Note also, overloads must be distinguishable based on argument **names**, not types.

```
fibonacci(Times: int): int
    Previous ;= 1
    Current ;= 0
    for Count: int < Times
        NextPrevious := Current
        Current += Previous
        Previous = NextPrevious
    return Current

fibonacci(Times: dbl): dbl
    GoldenRatio: dbl = (1.0 + math.sqrt(5)) * 0.5
    OtherRatio: dbl = (1.0 - math.sqrt(5)) * 0.5
    return (GoldenRatio^Times - OtherRatio^Times) / math.sqrt(5)

# COMPILE ERROR
```

## function templates/generics

You can create template functions which can work for a variety of types
using the `@(types...)` phrase after a function name.

```
log @(type) (Type): type
    print("got ${Type}")
    return Type

vector3 := (X: dbl, Y: dbl, Z: dbl)
Vector3 := vector3(Y: 5)
Result := log(Vector3)  # prints "got vector3(X: 0, Y: 5, Z: 0)".
Vector3 == Result       # equals True

# implicit type request:
OtherResult := log(5)   # prints "got 5" and returns 5.

# explicit type request:
DblResult := log @(dbl) (5)   # prints "got 5.0" and returns 5.0
```

# declaring and using a class

A class is defined with the `class` keyword with a `lowerCamelCase` identifier.
Class definitions must be constant/non-reassignable, so they are declared using
the `:=` symbols.

```
exampleClass := class(object)(
    # class instance variables can be defined here:
    X; int

    # class methods can be defined as well:
    doSomething(Int): int
        return X + Int

    # TODO: revisit with immutable fields (; vs. :)
    # classes must be resettable to a blank state, or to whatever is specified
    # as the starting value based on a `reset` function:
    reset(X: int): null
        This.X = X

    # or short-hand: `reset(This.X: int)` will automatically set `This.X` to the passed in `X`.
)

Example; exampleClass = (X: 5)  # also equivalent, `Example ;= exampleClass(X: 5)`
print(Example.doSomething(7))   # should print 12
Example = exampleClass(X: 7)    # note: variable can be reassigned.
Example.X -= 3                  # internal fields can be reassigned as well.

ConstVar := exampleClass(X: 2)
ConstVar.X += 3                 # internal fields can be reassigned
ConstVar = exampleClass(X: 4)   # COMPILER ERROR! variable is non-reassignable.
```

## parent-child classes and method overloads

You can define parent-child class relationships like this.

```
animal := class(object)(
    reset(This.Name: string): null

    # define two methods on `animal`: `speak` and `go`.
    # these are "abstract" methods, i.e., not implemented by this base class.
    speak(): null
    goes(): string

    # this method is defined, so it's implemented by the base class.
    # derived classes can still change it, though.
    escape(): null
        print "${Name} ${goes()} away!!"
)

snake := class(animal)(
    # if no `reset` functions are defined,
    # child classes will inherit their parent `reset()` methods.

    speak(): null
        print "hisss!"
    goes(): string
        return "slithers"
)

Snake := snake(Name: "Fred")
Snake.escape()  # prints "Fred slithers away!!"

cat := class(animal)(
    # here we define a `reset` method, so the parent `reset` methods
    # become hidden to users of this child class:
    reset(): null
        # can refer to parent methods using class name:
        animal.reset(Name: "Cat-don't-care-what-you-name-it")

    speak(): null
        print "hisss!"
    goes(): string
        return "saunters"

    escape(): null
        print "CAT ESCAPES DARINGLY!"
)

Cat := cat()
Cat.escape()    # prints "CAT ESCAPES DARINGLY!"
```

All abstract base classes also provide ways to instantiate using lambda functions.
All abstract methods must be defined for the instance to be created, and if a
`reset` method is defined on the parent, any arguments passed into the first reset
(i.e., which is the default constructor) should be defined for the lambda class.

```
WeirdAnimal := animal(
    Name: "Waberoo"
    speak(): null
        print "Meorooo"
    goes(): string
        return "meanders"
    escape(): null
        animal.escape()
        print "${Name} ${goes()} back..."
        animal.escape()
)

WeirdAnimal.escape()    # prints "Waberoo ... meanders ... meanders back ... meanders away!!"
```

## template methods

You can define methods on your class that work for a variety of types.

```
someExample := class(object)(
    Value: int
    reset(Int): null
        This.Value = Int
    to @(type) (): type
        return type(Value)
)

SomeExample := someExample(5)

# you can use type inference here based on variable taking the return value:
ToString: string = SomeExample.to()

# or you can explicitly ask for the type
ToDbl := SomeExample.to @(dbl) ()

# or you can explicitly ask like this:
To64 := i64(SomeExample.to())
```

## generic/template classes

To create a generic class, you put the expression `@(types...)` after the
`class` keyword.  You can use these new types for any class inheritance
from a parent which is a generic/template class.

```
# create a class with two generic types, `key` and `value`:
genericClass := class @(key, value) (object) (
    reset(This.Key: key, This.Value: value): null
)

# creating an instance using type inference:
ClassInstance := genericClass(Key: 5, Value: "hello")

# creating an instance with template/generic types specified:
OtherInstance := genericClass @(key: dbl, value: string) (Key: 3, Value: 4)
# note the passed-in values will be converted into the correct type.
```


# standard container classes (and helpers)

## arrays

An array contains a list of elements in contiguous memory.  You can
implicitly define an array using the notation `VariableName: type[]`,
e.g. `MyArray: int[]` or `MyArray: array@(int)` for an integer array.

TODO: we might define `int[]` internally as a contiguous deque.

```
# some relevant pieces of the class definition
array := class @(type) (
    ...
    # always returns a non-null type, resizing the array to
    # add default-initialized values if necessary:
    This[Index: index]: type
        ...
    size(): index
        ...
    append(Type: type)
        ...
    pop(Index: index = -1)
        ...
    ...
)
```

## hash maps

A hash map can look up elements by key in O(1) time.  You can use the explicit
way to define a map, e.g., `VariableName: map@(key: keyType, value: valueType)`,
or you can use the implicit method, `VariableName: valueType[keyType]`.  E.g.,
for a map from integers to strings, you can use: `MyMap: string[int]`.

```
# some relevant pieces of the class definition
map := class @(key, value) (
    ...
    # always returns a non-null type, adding
    # a default-initialized value if necessary:
    This[Key]: value
        ...
    size(): index
        ...
    set()
        ...
    pop(Key)
        ...
    ...
)
```

## sets

TODO

## iterator

```
iterator := class @(type) (
    next(): type?
    previous?(): type?
    # returns next value of iterator without incrementing the iterator.
    # recommended to be present if `remove` is present.
    peak?(): type?
    # present only if underlying container supports removing the current element (at `peak()`)
    # returns the element, or null if no current element.
    remove?(): type?
    # present only if underlying container supports inserting a new element (before `peak()`)
    insert?(Type): null
)
```
