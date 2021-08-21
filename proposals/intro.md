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

## casting between types

The default rounding behavior for `dbl` (or `flt`) to `int` is to use stochastic rounding.
This rounds 0.7 to 1 with probability 70% and to 0 with probability 30%, and similarly
for other values (using the fractional part to determine the probability weights).

```
X: dbl = 5.43
Y: int = X      # Y = 5 with probability 57%, or 6 with probability 43%.
```

## optional types

```
# TODO: do we want `X: int?` or `X: int|null`
```

# operators and precedence

TODO: add ; : ,

TODO: see if member access needs to be LTR

| Precedence| Operator  | Name                      | Type/Usage        | Associativity |
|:---------:|:---------:|:--------------------------|:-----------------:|:-------------:|
|   1       |   `_`     | subscript/index/key       | binary: `A_B`     | RTL           |
|           |   `^`     | superscript/power         | binary: `A^B`     |               |
|           |   `^^`    | repeated application      | on fn: `a^^B(X)`  |               |
|   2       |   `[ ]`   | function call             | on fn: `a B`      | RTL           |
|           |   `[ ]`   | member access             | binary: `A B`     |               |
|   3       |   `&`     | bitwise AND               | binary: `A&B`     | LTR           |
|           |   `\|`    | bitwise OR                | binary: `A\|B`    |               |
|           |   `><`    | bitwise XOR               | binary: `A><B`    |               |
|           |   `<>`    | bitwise flip              | unary:  `<>A`     |               |
|           |   `>>`    | bitwise right shift       | binary: `A>>B`    |               |
|           |   `<<`    | bitwise left shift        | binary: `A<<B`    |               |
|   4       |   `*`     | multiply                  | binary: `A*B`     | LTR           |
|           |   `/`     | divide                    | binary: `A/B`     |               |
|           |   `//`    | integer divide            | binary: `A//B`    |               |
|   5       |   `+`     | add                       | binary: `A+B`     | LTR           |
|           |   `-`     | subtract                  | binary: `A-B`     |               |
|   6       |   `==`    | equality                  | binary: `A==B`    | LTR           |
|           |   `!=`    | inequality                | binary: `A!=B`    |               |
|   7       |   `&&`    | logical AND               | binary: `A&&B`    | LTR           |
|           |  `\|\|`   | logical OR                | binary: `A\|\|B`  |               |
|   8       |   `=`     | assignment                | binary: `A = B`   | LTR           |
|           |  `???=`   | compound assignment       | binary: `A += B`  |               |

# declaring and using variables

Variables are named using `UpperCamelCase` identifiers.  The `:` symbol is used
to declare deeply constant, non-reassignable variables, and `;` is used to declare
mutable, reassignable variables.

```
# declaring and setting a non-reassignable variable that holds a big integer
Y: int = 5
# also equivalent: `Y := 5` or `Y := int(5)`.

# using the variable:
print(Y * 30)

Y = 123     # COMPILER ERROR, Y is non-reassignable.
Y += 3      # COMPILER ERROR, Y is deeply constant.
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

## temporarily locking non-reassignable variables

TODO: see if there's a better syntax for this.

You can also make a variable non-reassignable and deeply constant
for the remainder of the current block by using `;:` after the variable name.

```
X; int = 4  # defined as mutable and reassignable

if SomeCondition
    X;: = 7 # locks X after assigning it.
            # For the remainder of this indented block, you can use X but not reassign it
            # you also can't use mutable, i.e., non-const, methods on X.
else
    X;:     # lock X to whatever value it was for this block.
            # You can still use X but not reassign/mutate it.

print(X)    # will either be 7 (if SomeCondition was true) or 4 (if !SomeCondition)
X += 5      # can modify X back in this block; there are no constraints here.
```

## nested/object types

You can declare an object type inline with nested fields.  The nested fields defined
with `:` are immutable, and `;` are mutable.

```
Vector; (X: dbl, Y: dbl, Z: dbl) = (X: 4, Y: 3, Z: 1.5)
Vector X += 4   # COMPILER ERROR, field `X` of object is immutable

# note however, as defined, Vector is reassignable since it was defined with `;`:
Vector = (X: 1, Y: 7.2)
# note, missing fields will be default-initialized.
Vector Z == 0   # should be True.

# to make an object variable non-reassignable, use := when defining:
Vector2 := (X: 3.75, Y: 3.25)
# or you can use `:` with an explicit type specifier and then `=`:
Vector2: (X: dbl, Y: dbl) = (X: 3.75, Y: 3.25)
# then these operations are invalid:
Vector2 X += 3          # COMPILER ERROR, object is immutable
Vector2 = (X: 1, Y: 2)  # COMPILER ERROR, variable is non-reassignable
```

You can define a type/interface for objects you use multiple times.

```
# a plain-old-data class with 3 non-reassignable fields, X, Y, Z:
vector3 := (X: dbl, Y: dbl, Z: dbl)

# you can use `vector2` now like any other type, e.g.:
Vector3 := vector3(X: 5, Y: 10)
```

## nested reassignable fields, and how to deeply lock

We can also allow type definitions with mutable fields, e.g. `(X; int, Y; dbl)`.
Depending on how the variable is defined, however, you may not be able to change
the fields once they are set.  If you define the variable with `;`, then you
can reassign the variable or modify the mutable fields.  But if you define the
variable with `:`, the object is deeply constant, regardless of the field definitions.

```
# vector2 has two reassignable fields, X and Y:
vector2 := (X; dbl, Y; dbl)

# when defined with `;`, the object is mutable and reassignable.
MutableVec2; vector2 = (X: 3, Y: 4)
MutableVec2 = vector2(X: 6, Y: 3)   # OK
MutableVec2 X += 4                  # OK
MutableVec2 Y -= 1                  # OK

# when defined with `:`, the object is deeply constant, so its fields cannot be changed:
ImmutableVec2: vector2 = (X: 5, Y: 3)    # note you can use : when defining.
ImmutableVec2 = vector2(X: 6, Y: 3)  # COMPILE ERROR, ImmutableVec2 is non-reassignable
ImmutableVec2 X += 4                 # COMPILE ERROR, ImmutableVec2 is deeply constant
ImmutableVec2 Y -= 1                 # COMPILE ERROR, ImmutableVec2 is deeply constant
```

When used as a map key, objects with nested fields become deeply constant,
regardless of whether the internal fields were defined with `;` or `:`.
I.e., the object is defined as if with a `:`.
This is because we need key stability inside a map; we're not allowed
to change the key or it could change places inside the map and collide
with an existing key.

## hiding variables for the remainder of the block

TODO.  descopes/destructs if the variable was declared in that block.

# declaring, defining, and using a function

Functions are named using `lowerCamelCase` identifiers.  All functions
must return something, and `null` is a valid return type.  (There is no "void" type.)
TODO: Probably it's ok to avoid writing in the `null` return type; if no
return type is specified, `null` is assumed, unless we writing an inline function
where we can infer the return argument value immediately.

```
# declaring a function with no arguments that returns a big integer
v(): int

# setting/defining/initializing the function usually requires an indent+1 block following:
v(): int
    return 600

# but in simple cases like this you can also define inline:
v() := 600

# function with X,Y double-precision float arguments that returns nothing
v(X: dbl, Y: dbl): null
    print("X = ${X}, Y = ${Y}, atan(Y, X) = ${math atan(X, Y)}")
    # Note this could also be defined more concisely using $$,
    # which also prints the expression inside the parentheses with an equal sign and its value,
    # although this will print `math atan(X, Y) = ...`, e.g.:
    # print("$${X}, $${Y}, $${math atan(X, Y)}")

# function that takes a function as an argument and returns a function
# TODO: input function must be scoped to survive however long `wow` can be called;
# how do we want to do closure??
wow(Input fn(): string): fn(): int
    return fn(): int
        return Input fn() size()
```

## constant versus mutable arguments

Functions can be defined with mutable or immutable arguments, but that does
not change the function signature (see section on function overloads).
The important difference is that arguments defined with `;` must be copied
in from the outside (unless the external variable is already a temporary),
whereas arguments defined with `:` can be referenced without a copy.  This
is because arguments defined with `;` can be modified inside the function,
but they should not modify any variables outside of the function, even if
they are passed in.  Examples:

```
# this function makes a copy of whatever string is passed in:
copiedArgumentFunction(CopyMe; string): string
    CopyMe += "!!??"    # OK since CopyMe is defined as mutable via `;`.
    print(CopyMe)
    return CopyMe

# this function just references whatever string is passed in (no copy):
reffedArgumentFunction(Ref: string): string
    print(Ref)
    return Ref + "??!!"

MyValue: string = "immutable"
copiedArgumentFunction(CopyMe: MyValue) # prints "immutable!!??"
print(MyValue)                          # prints "immutable"
reffedArgumentFunction(Ref: MyValue)    # prints "immutable??!!"
print(MyValue)                          # prints "immutable"

Mutable; string = "mutable"
copiedArgumentFunction(CopyMe: Mutable) # prints "mutable!!??"
print(Mutable)                          # prints "mutable"
reffedArgumentFunction(Ref: Mutable)    # prints "mutable??!!"
print(Mutable)                          # prints "mutable"
```

The reason that the `;` or `:` argument definition doesn't change the function
signature is because in either case, the variables passed in from the outside
are not affected by the internal parts of the function.  That is, the function
cannot modify the external variables at all.

TODO: figure out how to pass in an object pointer so that you can modify
an object variable from the outside.  we could try to force these all
to be object methods (e.g., on a class), but people might need it.
Maybe define a method-type function: `someObjectType;;myMethod(args...): returnType`
which allows you to operate on an instance of `someObjectType` inside.
Or maybe allow it based on the return type.

## calling a function

You can call functions with arguments in any order.  Arguments must be specified
with the named identifiers in the function definition.  The only exception is
if the argument is "unnamed" (i.e., it has the same name as the type), then you
don't need to specify its name.  We'll discuss that more in the next section.

```
# definition:
v(X: dbl, Y: dbl): null

# example calls:
v(X: 5.4, Y: 3)
v(Y: 3, Y: 5.4)

# if you already have variables X and Y, you don't need to re-specify their names:
X := 5.4
Y := 3
v(X, Y)     # equivalent to `v(X: X, Y: Y)` but the redundancy is not idiomatic.
v(Y, X)     # equivalent
```

TODO: Check if the syntax works out
We also allow calling functions without parentheses for a single argument, like this:

```
print(String): null     # definition, using an "unnamed" string, e.g., `String: string`.

# example calls:
print("Hello, world!")  # with parentheses, and without explicitly naming the variable
print "Hello, world!"   # without
```

To specify multiple arguments, one must use parentheses to avoid ambiguity.
However, if you are specifying a single, named argument, you can avoid using parentheses.

```
v(X: dbl, Int): null    # definition

v 100       # executes v(X: 0.0, Int: 100)
v X: 5      # executes v(X: 5, Int: 0)

# COMPILE ERROR: need parentheses here, since commas bind less strongly than function-spaces:
v 100, X: 10
```

A function can have a function as an argument, and there are a few different ways to call
it in that case.
```
# finds the integer input that produces "hello, world!" from the passed-in function, or -1
# if it can't find it.
detect(greet(Int): string): int
    for Check: int < 100
        if greet(Check) == "hello, world!"
            return Check
    return -1

# if your function is named the same as the function argument, you can use it directly:
greet(Int): string
    return "hay"
detect(greet)       # returns -1

# if your function is not named the same, you can do argument renaming;
# internally this does not create a new function:
sayHi(Int): string
    return "hello, world" + "!" * Int
detect(greet: sayHi)    # returns 1

# you can also create a lambda function named correctly inline -- the function
# will not be available outside, after this call (it's scoped to the function arguments).
detect(greet(Int): string
    return "hello, world!!!!" substring(Length: Int)
)   # returns 13
```

## unnamed arguments in functions

For functions with a single argument where the variable name doesn't matter,
you can use "unnamed" variables.  For primitive types, the "unnamed" identifier
is just the `UpperCamelCase` version of the `lowerCamelCase` type.

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

Note that you can define the function arguments as mutable (with `;`) or
immutable (with `:`) but that does **not** change the type of function.
Whether the variables can be mutated inside the function does not matter
to the interface between function and caller.

```
# Note, these are not real overloads, and defining both would throw a COMPILE ERROR

greetings(Say: string): null
    print "${Say}, world!"

greetings(Say; string): null
    Say += " wow"
    print "${Say}, world..."

# COMPILE ERROR, function overloads of `greetings` only differ in mutability of arguments;
# argument names must be unique in this case.
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
    GoldenRatio: dbl = (1.0 + math sqrt(5)) * 0.5
    OtherRatio: dbl = (1.0 - math sqrt(5)) * 0.5
    return (GoldenRatio^Times - OtherRatio^Times) / math sqrt(5)

# COMPILE ERROR: function overloads of `fibonacci` must have unique argument names, not argument types.
```

## function templates/generics

You can create template functions which can work for a variety of types
using the syntax `@(type1, type2, ...)` or `@type` (for just one generic type) 
after a function name.

```
# declaration equivalent to `logger @(type) (Type): type`:
logger @type (Type): type
    print("got ${Type}")
    return Type

vector3 := (X: dbl, Y: dbl, Z: dbl)
Vector3 := vector3(Y: 5)
Result := logger(Vector3)   # prints "got vector3(X: 0, Y: 5, Z: 0)".
Vector3 == Result           # equals True

# implicit type request:
OtherResult := logger(5)    # prints "got 5" and returns the integer 5.

# explicit generic request:
DblResult := logger @dbl (5)    # prints "got 5.0" and returns 5.0

# explicit type request:
AnotherDblResult := logger(dbl(4))  # prints "got 4.0" and returns 4.0
# can also use `logger dbl(4)` above or `logger dbl 4` with function spaces.
```

# declaring and using a class

A class is defined with the `class` keyword and a `lowerCamelCase` identifier.
Class definitions must be constant/non-reassignable, so they are declared using
the `:` symbol.

```
exampleClass := class() {
    # class instance variables can be defined here:
    X; int

    # class methods can be defined as well:
    doSomething(Int): int
        return X + Int

    # TODO: revisit with immutable fields (; vs. :)
    # classes must be resettable to a blank state, or to whatever is specified
    # as the starting value based on a `reset` function:
    reset(X: int): null
        This X = X

    # or short-hand: `reset(This X: int)` will automatically set `This X` to the passed in `X`.
}

Example; exampleClass = (X: 5)  # also equivalent, `Example ;= exampleClass(X: 5)`
print(Example doSomething(7))   # should print 12
Example = exampleClass(X: 7)    # note: variable can be reassigned.
Example X -= 3                  # internal fields can be reassigned as well.

# note that if you define an instance of the class as immutable, you can only operate
# on the class with functions that do not mutate it.
ConstVar := exampleClass(X: 2)
ConstVar X += 3                 # COMPILER ERROR! `ConstVar` is deeply constant.
ConstVar = exampleClass(X: 4)   # COMPILER ERROR! variable is non-reassignable.
```

## parent-child classes and method overloads

You can define parent-child class relationships like this.

```
animal := class() {
    reset(This Name: string): null

    # define two methods on `animal`: `speak` and `go`.
    # these are "abstract" methods, i.e., not implemented by this base class.
    speak(): null
    goes(): string

    # this method is defined, so it's implemented by the base class.
    # derived classes can still change it, though.
    escape(): null
        print "${Name} ${goes()} away!!"
}

snake := class(animal) {
    # if no `reset` functions are defined,
    # child classes will inherit their parent `reset()` methods.

    speak(): null
        print "hisss!"
    goes(): string
        return "slithers"
}

Snake := snake(Name: "Fred")
Snake escape()  # prints "Fred slithers away!!"

cat := class(animal) {
    # here we define a `reset` method, so the parent `reset` methods
    # become hidden to users of this child class:
    reset(): null
        # can refer to parent methods using class name:
        animal reset(Name: "Cat-don't-care-what-you-name-it")

    speak(): null
        print "hisss!"
    goes(): string
        return "saunters"

    escape(): null
        print "CAT ESCAPES DARINGLY!"
}

Cat := cat()
Cat escape()    # prints "CAT ESCAPES DARINGLY!"
```

All abstract base classes also provide ways to instantiate using lambda functions.
All abstract methods must be defined for the instance to be created, and if a
`reset` method is defined on the parent, any arguments passed into the first reset
(i.e., which is the default constructor) should be defined for the lambda class.

```
# TODO: lambda functions defined using `functionName(args...) := returnValueStatement`
# if that is ok with syntax/grammar.
WeirdAnimal := animal(
    Name: "Waberoo"
    speak(): null
        print "Meorooo"
    goes(): string
        return "meanders"
    escape(): null
        animal escape()
        print "${Name} ${goes()} back..."
        animal escape()
)

WeirdAnimal escape()    # prints "Waberoo ... meanders ... meanders back ... meanders away!!"
```

## template methods

You can define methods on your class that work for a variety of types.

```
someExample := class() {
    Value: int
    reset(Int): null
        This Value = Int
    to @(type) (): type
        return type(Value)
}

SomeExample := someExample(5)

# you can use type inference here based on variable taking the return value:
ToString: string = SomeExample to()

# or you can explicitly ask for the type
ToDbl := SomeExample to @(dbl) ()

# or you can explicitly ask like this:
To64 := i64(SomeExample to())
```

## generic/template classes

To create a generic class, you put the expression `@(types...)` after the
`class` keyword.  You can use these new types for any class inheritance
from a parent which is a generic/template class.

```
# create a class with two generic types, `key` and `value`:
genericClass := class @(key, value) () {
    reset(This Key: key, This Value: value): null
}
# if this class is just POD, you can use the equivalent type:
# genericClass := @(key, value) (Key: key, Value: value)
# TODO: allow specifying generics inline, e.g. `genericClass := (Key: @key, Value: @value)`,
# or `genericClass := (@Key, @Value)` for short.

# creating an instance using type inference:
ClassInstance := genericClass(Key: 5, Value: "hello")
 
# creating an instance with template/generic types specified:
OtherInstance := genericClass @(key: dbl, value: string) (Key: 3, Value: 4)
# note the passed-in values will be converted into the correct type.
```

# modules

TODO: find a better syntax for importing a module.  `math sqrt` isn't good


# standard container classes (and helpers)

## arrays

An array contains a list of elements in contiguous memory.  You can
define an array explicitly using the notation `array@elementType`
for the type `elementType`, or implicitly with the subscript operator, `_`
(AKA "key" or "indexing" operator), using the notation `elementType_`.
E.g. `MyArray: int_` or `MyArray: array@int` for an immutable integer array.
The mutable versions of course use `;` instead of `:`.

Side note: as we will see, the subscript operator is usually a binary operator, i.e.,
requiring two operands, `A _ B`, read "A subscript B".  We make an exception for the
array type not to require a second operand -- in fact, adding one would create a
different type, the map type.

The unnamed version of an array `elementType_` is `ElementType~`,
which you can read as `ElementType`s (plural) or `ElementType` + `S` (for stack
AKA array).  Example usage and declarations:

```
# this is an immutable array:
MyArray: dbl_ = [1.2, 3, 4.5]       # converts all to dbl
MyArray append(5)   # COMPILE ERROR: MyArray is immutable
MyArray_1 += 5      # COMPILE ERROR: MyArray is immutable

# mutable integer array:
Int~;           # declaring a mutable, "unnamed" integer array, e.g. `Int~; int_`
Int~ append(5)  # now Int~ == [5]
Int~_3 += 30    # now Int~ == [5, 0, 0, 30]
Int~_4 = 300    # now Int~ == [5, 0, 0, 30, 300]
Int~_2 -= 5     # now Int~ == [5, 0, -5, 30, 300]

# mutable string array:
StringArray; string_ = ["hi", "there"]
print(StringArray pop())    # prints "there".  now StringArray == ["hi"]
```

The default implementation of `array` might be internally a contiguous deque,
so that we can pop or insert into the beginning at O(1).  We might reserve
`stack` for a contiguous list that grows in one direction only.

```
# some relevant pieces of the class definition
array := class @type () {
    ...
    # always returns a non-null type, resizing the array to
    # add default-initialized values if necessary:
    # TODO: maybe switch to getters and setters:
    # `This_(Index;): value` and `This_(Index;, Value;): value`
    # but then we'd have to figure out const ref vs. mutable on the return side as well.
    # TODO: figure out const-ref and mutable with function return types, maybe via pointers.
    This_(Index; index): gate@type

    size(): index

    append(Type;): null

    pop(Index: index = -1): type

    ...
}
```

## hash maps

A hash map can look up elements by key in O(1) time.  You can use the explicit
way to define a map, e.g., `VariableName: map@(key: keyType, value: valueType)`,
or you can use the implicit method with the subscript operator (`_`),
`VariableName: valueType_keyType`.  You can read the operator `_` as "keyed by",
e.g., `valueType_keyType` as "`valueType` keyed by `keyType`".  For example,
for a map from integers to strings, you can use: `MyMap: string_int`.

For simple types, one can create the "unnamed" version of a map using the
"capitalization" of the `_` operator, `~`.  E.g., `String~Int` is the default
variable name for the `string_int` map type (from integers to strings).
Like `_`, `~` is read as "keyed by" in this case, so that `String~Int` is
"String keyed by Int".

```
# some relevant pieces of the class definition
map := class @(key, value) () {
    # always returns a non-null type, adding
    # a default-initialized value if necessary:
    # TODO: maybe switch to getters and setters:
    # `This_(Key): value` and `This_(Key, Value;): value`
    This_(Key): gate@value
        ...
    size(): index
        ...
    set(Key;, Value;): null
        ...
    pop(Key): value
        ...
    ...
}
```

Maps require a key type whose instances can hash to an integer or string-like value.
E.g., `dbl` and `flt` cannot be used, nor can types which include those (e.g., `array @dbl`).

```
DblDatabase; dbl_int        # OK, int is an OK key type
DblDblDatabase; dbl_dbl     # COMPILE ERROR, dbl is an invalid key type.
```

However, we allow casting from these prohibited types to allowed key types.  For example:

```
NameDatabase; string_int
NameDatabase_123 = "John"
NameDatabase_124 = "Jane"
print(NameDatabase_123.4)   # prints "John" with 60% probability, "Jane" with 40%.

# note that the definition of the key is an immutable array; it's a compile error if the
# mutable version of the array is used:
StackDatabase; string_(int_)    # parentheses are grammatically unnecessary, subscripts go right to left
StackDatabase_[1,2,3] = "stack123"
StackDatabase_[1,2,4] = "stack124"
print(StackDatabase_[1.0, 2.0, 3.1])    # prints "stack123" with 90% probability, "stack124" with 10%
# things get more complicated, of course, if all array elements are non-integer.
# the array is cast to the key type (integer array) first.
StackDatabase_[2.2, 3.5, 4.8] = "odd"
# result could be stored in [2, 3, 4], [2, 3, 5], [2, 4, 4], [2, 4, 5],
#                           [3, 3, 4], [3, 3, 5], [3, 4, 4], [3, 4, 5]
# but the key is decided first, then the map is added to.
```

## sets

A set contains some elements, and makes checking for the existence of an element within
fast, i.e., O(1).  Like with map keys, the set's element type must satisfy certain properties
(e.g., integer/string-like).  The syntax to define a set is `VariableName: set@elementType`
to be explicit or `VariableName: _elementType` using the subscript operator `_` on the
opposite side of the array type (i.e., the array looks like `arrayElementType_`).
The unnamed version of a set `_elementType` is `~ElementType`.

```
set := class @type () {
    # returns true if the passed-in element is present in the set.
    This_(Type): bool
        ...
    size(): index
        ...
    # TODO: find a good way to indicate a method allows mutations on This.
    # e.g., `This;; += (...)` and `This;;pop(): type`, or maybe `;;pop(): type` for short.
    # Adds an element to the set
    This += (Type;): null
        ...

    pop(): type
        ...
    ...
}
```

## iterator

```
iterator := class @type () {
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
}
```

# standard flow constructs

TODO -- description, plus `consider+case` and `if/else/elif`
```
# for-loop with counter that is immutable inside the for-loop's block:
for Value: int < 10
    # Value goes from 0 to 9 by 1;
    # Value is not allowed to be mutated (defined with :).
    # trying to mutate it throws a compiler error.
    print Value
# prints "0" to "9" on separate newlines.

# for-loop whose counter can be modified inside the block.
for Special; int < 5
    print("A: ${Special}")
    ++Special
    print("B: ${Special}")
    ++Special
# prints "A: 0", "B: 1", "A: 3" "B: 4"
```

# pointers/references

TODO:
How do we allow modifying an external instance inside a function?
Do we use pointers?  (e.g., repurpose `@` for a pointer type?)
Do we figure out a different way to reference an existing instance?

E.g., for an array of some object type:
```
vector3 := class() {
    X; dbl
    Y; dbl
    Z; dbl
    norm() := math sqrt(X^2 + Y^2 + Z^2)
    # TODO: not sure about this syntax
    normalize(): ref@this
        Norm := norm()
        assert(Norm > 1e-8)
        X /= Norm
        Y /= Norm
        Z /= Norm
        return This
}

# normalize can be chained:
R1 := vector3(1, 2, 3) normalize()

# but when used inside an array, will the changes affect the array element?
Array; vector3_
Array_0 = {X: 5} 
Array_0 normalize() Z -= 0.5
# e.g., does Array_0 now equal vector3(X: 1, Z: -0.5) ??
```

# grammar/syntax

* `LowerCamelCase`: identifier which starts with a lowercase alphabetical character.
* `UpperCamelCase`: identifier which starts with an uppercase alphabetical character.

```
# TODO: support for labeling token matchers, e.g. "parentClassNames" and "classBlock"
ClassDefinition := sequence([
    keyword("class")
    optional(TemplateArguments)
    list(LowerCamelCase)    # parent class names
    parentheses(Block)
])

# a list encompasses things like (), (TokenMatcher), (TokenMatcher, TokenMatcher), etc.,
# but also lists with newlines if properly tabbed.
list(TokenMatcher) := parentheses(fn(EndParen) := until(
    EndParen
    repeat(CheckExit) := sequence([
        TokenMatcher, CheckExit, CommaOrBlockNewline
    ])
))

ForLoop := sequence([
    keyword("for")
    TODO
])

TODO: support internationalization.  do we really require Upper/lower+CamelCase for variables/functions?
or is the syntax unambiguous enough to not need them?
