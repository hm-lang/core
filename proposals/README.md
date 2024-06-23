# why

hm-lang tries to achieve **consistency** above all else.  
**Convenience**, **clarity**, **concision**, and **simplicity** come next.

## consistency

In most languages, primitive types have different casing than class types that are
created by end-user developers.  This is usually an inconsistency by convention,
e.g., `PascalCase` for class names and `snake_case` for primitive types in C++,
but Rust requires this inconsistency by fiat.
In hm-lang, all types are `lowerCamelCase`, like functions.  Variables and identifiers
like `True` or `False` are `UpperCamelCase`.  hm-lang doesn't recommend distinguishing
between constant identifiers and non-constant identifiers with casing,
e.g., like `UPPER_SNAKE_CASE` in other languages; you can rely on the compiler
to stop you if you try to change a constant variable.

Another change is that hm-lang uses `:` (or `;`) for declarations and `=` for reassignment,
so that declaring a variable and specifying a variable will work the same inside and outside
function arguments.  For example, declaring a function that takes an integer named `X`
`myFunction(X: int): null` and declaring an integer variable named `X` uses the same syntax:
`X: int`.  Similarly, calling a function with arguments specified as `myFunction(X: 5)` and
defining a variable works the same outside of a function: `X: 5`.

In some languages, e.g., JavaScript, objects are passed by reference and primitives
are passed by value when calling a function with these arguments.  In hm-lang,
arguments are passed by reference by default, for consistency.
See [passing by reference and by value](#pass-by-reference-or-pass-by-value). 
However, to avoid most surprises, arguments are passed by *readonly* reference.
See [passing by reference gotchas](#passing-by-reference-gotchas)) for the edge cases.

In hm-lang, determining the number of elements in a container uses the same
method name for all container types; `count(Container)` or `Container count()`,
which works for `Array`, `Store` (map/dict), `Set`, etc.  In some languages, e.g., JavaScript,
arrays use a property (`Array.length`) and maps use a different property (`Map.size`).

## convenience

hm-lang also prioritizes convenience; class methods can be called like a function with
the instance as an argument or as a method on the instance, e.g., `myMethod(MyClass)`
or `Class myMethod()`.  This extends to functions with definitions like
`myTwoInstanceFunction(MyClassA, MyClassB, Width: int, Height: int)` which
can be called as `(MyClassA, MyClassB) myTwoInstanceFunction(Width: 5, Height: 10)`,
or by calling it as a method on either one of the instances, e.g.,
`MyClassA myTwoInstanceFunction(MyClassB, Width: 5, Height: 10)`, without needing to
define, e.g., `MyClassB myTwoInstanceFunction(MyClassA, Width: 5, Height: 10)` as well.

For convenience, `Array[3] = 5` will work even if `Array` is not already at least size 4;
hm-lang will resize the array if necessary, populating it with default values,
until finally setting the fourth element to 5.  This is also to be consistent with
other container types, e.g., stores, where `Store["Id"] = 50` works conveniently.
In some standard libraries (e.g., C++), `Array[3] = 5` is undefined behavior
if the array is not already at least size 4.

Similarly, when referencing `Array[10]` or `Store["Id"]`, a default will be provided
if necessary, so that e.g. `++Array[10]` and `++Store["Id"]` don't need to be guarded
as `Array[10] = if count(Array) > 10 $(Array[10] + 1) else $(Array count(11), 1)` or
`Store["Id"] = if Store["Id"] != Null $(Store["Id"] + 1) else $(1)`.

## clarity

Functions are called with named arguments always, although names can be omitted in
certain circumstances [when calling a function](#calling-a-function).

Nullable variables should always be annotated by `?`, both in a declaration
`Y?: somePossiblyNullReturningFunction()` and also when used as a function
argument, e.g., `callWithNullable(SomeValue?: Y)`.  This is to avoid surprises
with null, since `callWithNullable(SomeValue?: Null)` is equivalent to
`callWithNullable()`, which can be a different overload.

## concision

When calling a function, we don't need to use `myFunction(X: X)` if we have a local
variable named `X` that shadows the function's argument named `X`.  We can just
call `myFunction(X)` for readonly reference (`X: X`), `otherFunction(Y;)` for a writable
reference (`Y; Y`), or `tmpFunction(Z!)` to pass in as a temporary `Z. Z!`.  While
technically we might want the `:;.` on the left-hand side of the variable we're passing in
(e.g., `otherFunction(;Y)`), we keep it on the right-hand side for consistency of how
functions are defined, and to avoid needing to do something like `tmpFunction(.Z!)`.

When defining a function, variable arguments that use the default name for a type can
elide the type name; e.g., `myFunction(Int): str` will declare a function that takes 
an instance of `int`.  See [default-named arguments](#default-name-arguments-in-functions).
This is also true if namespaces are used, e.g., `myFunction(Named Int): str`.  We can also
avoid the readonly declaration (`:`) since it is the default, but you can use
`myFunction(Int;): str` for a function which can mutate the passed-in integer
or `myFunction(Int.): str` for a function which takes a temporary integer.
This also works for generic classes like `myGeneric[of]` where `of` is a template type;
`myFunction(MyGeneric[int];)` is short for `myFunction(MyGeneric; myGeneric[int])`.

Class methods technically take an argument for `Me/My/I` everywhere, but instead of
writing `myMethod(Me, X: int): str`, we can write `::myMethod(X: int): str`.
This parallels `myClass::myMethod` in C++, but in hm-lang we can analogously use
`myClass;;myMutatingMethod` for a method that can mutate `Me`, i.e.,
`myMutatingMethod(Me;, X: int): str` becomes `;;myMutatingMethod(X: int): str`,
or `myClass..myTemporaryMethod()` for a method on a temporary `Me`, i.e.,
`myTemporaryMethod(Me.)`.  You can substitute `I` or `My` for `Me` anywhere,
including in methods that only declare one of them in the function arguments.
We recommend using `My` for field access (e.g., `My X`) as well as getters/setters
`My x()` or `My x(NewXValue)`, `Me` for returning a copy/clone (e.g., `return Me`),
and `I` for methods that start with a verb, e.g., `I draw()`.

Class getters/setters do not use `::getX(): dbl` or `;;setX(Dbl): null`, but rather
just `::x(): dbl` and `;;x(Dbl;.): null` for a private variable `X; dbl`.  This is one
of the benefits of using `lowerCamelCase` for functions/methods and `UpperCamelCase`
for variables; we can easily distinguish intent without additional verbs.

hm-lang uses result-passing instead of exception-throwing in order to make it clear
when errors can occur.  The `hm[ok, uh]` class handles this, with `ok` being the
type of a valid result, and `uh` being the type of an error result.  You can specify
the types via `hm[ok: int, uh: str]` for `ok` being `int` and `uh` being a `str`.
If the `ok` and `uh` types are distinct, you don't need to wrap a return value in
`ok(ValidResult)` and `uh(ErrorResult)`; you can just return `ValidResult` or `ErrorResult`.

## simplicity

We don't require a different function name for each method to convert a result class
into a new one, e.g., to transform the `ok` result or the `uh` error.  We simply
use `map` for everything, with function overloading helping us out.

We also don't use a different concept for interfaces and inheritance.
The equivalent of an interface in hm-lang is simply an abstract class.  This way
we don't need two different keywords to `extend` or `implement` a class or interface,
we simply always `extend` the class.

## safety

hm-lang supports "safe" versions of functions where it's possible that we'd run out of
memory or otherwise throw.  By default, `Array[100] = 123` will increase the size
of the array if necessary, and this could potentially throw in a memory-constrained
environment (or if the index was large).  If you need to check for these situations,
there is a safe API, e.g., `Result: Array at(100, Put: 123)` and `Result` can then
be checked for `isUh()`, etc.  For convenience for cases where you don't care about
memory, these safe functions are a bit more verbose than the unchecked functions.


# general syntax

* `print(...)` to echo some values (in ...) to stdout, `error(...)` to echo to stderr
* `lowerCamelCase` identifiers like `x` are function/type-like, see [here](#variable-and-function-names)
* `UpperCamelCase` identifiers like `X` are instance-like, see [here](#variable-and-function-names)
* use `#` for [comments](#comments)
* outside of arguments, use `:` for readonly declarations and `;` for writable declarations
* for an argument, `:` is a readonly reference, `;` is a writable reference, and `.` is a temporary
    (i.e., passed by value), see [pass-by-reference or pass-by-value](#pass-by-reference-or-pass-by-value)
* use `:` to declare readonly things, `;` to declare writable things.
    * use `A: x` to declare `A` as an instance of type `x`, see [variables](#variables)
    * use `fn(): x` to declare `fn` as returning an instance of type `x`, see [functions](#functions)
    * use `a: y` to declare `a` as a constructor that builds instances of type `y`
    * while declaring *and defining* something, you can avoid the type if you want the compiler to infer it,
        e.g., `A: someExpression()`
* when not declaring things, `:` is not used; e.g., `if` statements do not require a trailing `:` like python
* `()` for organization and function calls
    * `(W: str = "hello", X: dbl, Y; dbl, Z. dbl)` to declare an argument object type, `W` is an optional field
        passed by readonly reference, `X` is a readonly reference, `Y` is a writable reference,
        and `Z` is passed by value.  See [argument objects](#argument-objects) for more details.
    * `(SomeInstance x(), SomeInstance Y;, W: "hi", Z. 1.23)` to instantiate an argument object instance
        with `X` and `W` as readonly references, `Y` as mutable reference, and `Z` as a temporary.
    * `"My String Interpolation is $(missing(), X)"` to add `X` to the string.
        Note that only the last element in the `$()` is added, but `missing()` will still be evaluated.
* `[]` are for containers
    * `Store[id]: value` to declare a store (i.e., map/dict), or `Store: value[id]`
    * `Set[element]:` or `Set: [element]` to declare a set with `element` type instances as elements
    * `Array[]: element` or `Array: element[]` to declare an array with `element` type instances as elements
    * `"My String interpolation is $[X, Y]"` to add `[*value-of-X*, *value-of-Y*]` to the string.
    * For generic/template classes, e.g., classes like `array[Count, of]` for a fixed array of size
        `Count` with elements of type `of`, or `store[id: str, value: int]` to create a map/dictionary
        of strings mapped to integers.  See [generic/template classes](#generictemplate-classes).
    * For generic/template functions with type constraints, e.g., `myFunction[of: nonNull](X: of, Y: int): of`
        where `of` is the generic type.  See [generic/template functions](#generictemplate-functions) for more.
* `{}` for objects/types
    * `{X: dbl, Y: dbl}` to declare a class with two double-precision fields, `X` and `Y`
    * `{X: 1.2, Y: 3.4}` to instantiate a plain-old-data class with two double-precision fields, `X` and `Y`
    * `{Greeting: str, Times: int} = destructureMe()` to do destructuring of a return value
    * `"My String Interpolation is ${X, Y: Z}"` to add `{X: *value-of-X*, Y: *value-of-Z*}` to the string.
    * `A {x(), Y}` to call `A x()` then `A Y` via [sequence building](#sequence-building)
        and return them in an object with fields `X` and `Y`, i.e., `{X: A x(), Y: A Y}`.
    * `A {L1: x(), L2: Y}` to do sequence building with renamed fields, i.e.,
        `{L1: A x(), L2: A Y}`.
* `~` to declare a template type within a function, e.g., `myGenericFunction(Value: ~u): u` to declare
    a function that takes a generic type `u` and returns it.  For more details, see
    [generic/template functions](#generictemplate-functions).
* `$` for inline block and lambda arguments
    * [inline blocks](#block-parentheses-and-commas) include:
        * `$(...)` to effectively indent `...`: `if Condition $(doThing()) else $(doOtherThing())`
        * `$[...]` as shorthand for a new block defining `[...]`, e.g., for a return value:
            `Array: if SomeCondition $[1, 2, 3] else $[4, 5]`
        * `${...}` as shorthand for a new block defining `{...}`, e.g., for a return value:
            `Result: if X > Y ${Max: X, Min: Y} else ${Min: X, Max: Y}`
    * `MyArray map($Int * 2 + 1)` to create a [lambda function](#functions-as-arguments)
        which will iterate over e.g., `MyArray: [1, 2, 3, 4]` as `[3, 5, 7, 9]`.
        * Lambda arguments need to specify which scope they attach to, by increasing
            the number of `$` to escape parentheses, e.g., `MyArray map(str($$Int) * 2)`
            to get `["11", "55", "1010"]` for `MyArray: [1, 5, 10]`.
* all arguments are specified by name so order doesn't matter, although you can have default-named arguments
  for the given type which will grab an argument with that type (e.g., `Int` for an `int` type).
    * `(X: dbl, Int)` can be called with `(1234, X: 5.67)` or even `(Y, X: 5.67)` if `Y` is an `int`
* variables that are already named after the correct argument can be used without `:`
    * `(X: dbl, Y: int)` can be called with `(X, Y)` if `X` and `Y` are already defined in the scope,
        i.e., eliding duplicate entries like `(X: X, Y: Y)`.

```
# declaring a variable:
ReadonlyVar: int
MutableVar; int

# declaring + defining a variable:
MutableVar; 321

# you can also give it an explicit type:
ReadonlyVar: int(123)

# you can also define a variable using an indented block;
# the last line will be used to initialize the variable.
# here we use an implicit type (whatever `SomeHelperValue + 4` is).
MyVar:
    # this helper variable will be descoped after calculating `MyVar`.
    SomeHelperValue: someComputation(3)
    SomeHelperValue + 4

# you can also give it an explicit type:
OtherVar; explicitType
    "asdf" + "jkl;"
```

```
# declaring a readonly array
MyArray[]: elementType      # also ok: `MyArray: elementType[]`

# defining a writable array:
ArrayVar; [1, 2, 3, 4]      # also ok: `ArrayVar[]; int = [...]`
                            # also ok: `ArrayVar; int[] = [...]`
ArrayVar[5] = 5     # ArrayVar == [1, 2, 3, 4, 0, 5]
++ArrayVar[6]       # ArrayVar == [1, 2, 3, 4, 0, 5, 1]
ArrayVar[0] += 100  # ArrayVar == [101, 2, 3, 4, 0, 5, 1]
ArrayVar[1]!        # returns 2, zeroes out ArrayVar[1]:
                    # ArrayVar == [101, 0, 3, 4, 0, 5, 1]
```

```
# declaring a readonly store
MyStore[idType]: valueType   # also ok: `MyStore: valueType[idType]`

# defining a writable store:
VotesStore[str]; int = ["Cake": 5, "Donuts": 10, "Cupcakes": 3]
# We can also infer types implicitly via `VotesStore; ["Cake": 5, ...]`?
VotesStore["Cake"]        # 5
++VotesStore["Donuts"]    # 11
++VotesStore["Ice Cream"] # inserts "Ice Cream" with default value, then increments
VotesStore["Cupcakes"]!   # deletes from the store (but returns `3`)
VotesStore::["Cupcakes"]  # Null
# now VotesStore == ["Cake": 5, "Donuts": 11, "Ice Cream": 1]
```

```
# declaring a readonly set
MySet[elementType]:     # also ok: `MySet: [elementType]`

# defining a writable set:
SomeSet[str]; ["friends", "family", "fatigue"]
# Also ok: we can do `SomeSet; [str]["friends", ...]`
SomeSet::["friends"]    # `True`
SomeSet::["enemies"]    # Null (falsy)
SomeSet["fatigue"]!     # removes "fatigue", returns `True` since it was present.
                        # SomeSet == [str]["friends", "family"].
SomeSet["spools"]       # adds "spools", returns Null (wasn't in the set)
                        # SomeSet == [str]["friends", "family", "spools"]
```

```
# declaring a "void" function:
doSomething(With: int, X; int, Y; int): null

# defining a void function
doSomething(With: int, X; int, Y; int): null
    # because `X` and `Y` are defined with `;`, they are writable
    # in this scope and their changes will persist back into the
    # caller's scope.
    X = With + 4
    Y = With - 4

# calling a function with temporaries:
doSomething(With: 5, X; 12, Y; 340)

# calling a function with variables matching the argument names:
With: 1000
X; 1
Y; 2
# Note that readonly arguments (`:`) are the default,
# so you need to specify `;` for writable arguments.
doSomething(With, X;, Y;)

# calling a function with argument renaming:
Mean: 1000
MutatedX; 1
MutatedY; 2
doSomething(With: Mean, X; MutatedX, Y; MutatedY)
```

```
# declaring a function that returns other values:
doSomething(X: int, Y: int): {W: int, Z: int}

# defining a function that returns other values
doSomething(X: int, Y: int): {W: int, Z: int}
    # option A:
    Z = \\math atan(X, Y)
    W = 123
    # option B:
    {Z: \\math atan(X, Y), W: 123}
```

```
# declaring a simple class
vector3: {X: dbl, Y: dbl, Z: dbl}

# declaring a "complicated" class
myClass: {
    # methods which mutate the class use a `;;` prefix
    ;;renew(My X: int): Null

    # methods which keep the class readonly use a `::` prefix
    ::doSomething(Y: int): int
        My X * Y
    # inline: `::doSomething(Y: int): int(My X * Y)`
}
```

```
# combining two classes
aOrB: oneOf[a, b]
aAndB: union[a, b]
allOf[...args]: union[...args]  # in case symmetry between `oneOf` and `allOf` makes more sense.
```

```
# defining a function with some lambda functions as arguments
doSomething(you(): str, greet(Name: str): str): str
    greet(Name: you())

# calling a function with some functions as arguments
myName(): str
    "World"
# inline, `myName(): "World"`
doSomething(you: myName, greet(Name: str): str
    "Hello, $(Name)"
)
```

```
# defining a function that returns a lambda function
makeCounter(Counter; int): fn(): int
    fn(): ++Counter
Counter; 123
counter: makeCounter(Counter;)
print(counter())    # 124
```

```
# defining a function that takes two type constructors as arguments,
# one of them being named, and returns a type constructor:
doSomething(~x, namedNew: ~y): oneOf[new[x], new[y]]
    # if random(dbl) < 0.5 $(x) else $(namedNew)

someType: doSomething(int, namedNew: dbl) # int or dbl with 50-50 probability
```

Note you could also return `new[oneOf[x, y]]` in the above example,
but that subtly changes the return value to a type that can be either `x` or `y`
and can switch from one to the other.  `oneOf[new[x], new[y]]` will be either
`x` or `y`, but does not allow switching.

## variable and function names

Identifiers in hm-lang are very important.  The capitalization (or lack thereof)
of the first letter indicates whether the identifier is a variable or a function.
Since we think of functions as verb-like, they are `lowerCamelCase` identifiers, e.g.,
`makeToast` or `runMarathon`.  On the other hand, variables are names, and we think
of them as proper nouns (like names), e.g., `Sam` or `MaxArrayCount`, so they are
`UpperCamelCase` identifiers.  Class names are `lowerCamelCase`, since they
act more functions than variables; e.g., you can convert one class instance into
another class's instance, like `int(MyNumberString)` which converts `MyNumberString`
(presumably a `string` type) into a big integer.

There are a few reserved keywords, like `if`, `elif`, `else`, `with`, `return`,
`what`, `in`,
which are function-like but may consume the rest of the statement.
E.g., `return X + 5` will return the value `(X + 5)` from the enclosing function.
There are some reserved namespaces like `Old`, `New`, `Other`, `First`, `Second`,
`Unused`,
and variables cannot be defined with these names.  Variables can be defined
using these namespaces, e.g., as `Old Int`, `New X`, `Other ClassType`, or `Unused Z`.
In particular, `First` and `Second`
are reserved for binary operations like `&&` and `*`.
See [namespaces](#namespaces) for more details.
Other reserved keywords: `new` for returning a class constructor, e.g.,
`myFunction(): new[oneOf[int, dbl]]` for returning either an `int` or `dbl` constructor.
There are some reserved variable names, like `I`, `Me`, and `My`, which can only
be used as a reference to the current class instance, and `You` and `Your` which
can only be used as a reference to a different instance of the same type.
(The corresponding types `i`, `me`, `my`, `you`, and `your` are reserved for the same reasons.)

Most ASCII symbols are not allowed inside identifiers, e.g., `*`, `/`, `&`, etc., but
underscores (`_`) have some special handling.  They are ignored in numbers,
e.g., `1_000_000` is the same as `1000000`, but highly recommended for large numbers.
Underscores in identifiers will automatically capitalize the next letter, so
`my_function` is the same as `myFunction`, and `_count` is the same as `Count`.
Numbers are ignored, so `x_1` is the same as `x1`.  Trailing underscores are not allowed;
if you want to annotate a variable as unused, use the `Unused` namespace.

## blocks

### tabs vs. spaces

Blocks of code are made out of lines at the same indent level; an indent is four spaces.
No more than 7 levels of indent are allowed, e.g., lines at 0, 4, 8, 12, 16, 20, 24 spaces.
If you need more indents, refactor your code.

### line continuations

Lines can be continued at a +2 indent from the originating line, though there are some
special rules here when parentheses are involved.  If an open bracket is encountered at
the end of the line, and the next lines are only at +1 indent, then we assume a container
is being constructed.  Note that operators besides parentheses *are ignored* for determining
the indent, so typical practice is to tab to the operator then tab to the number/symbol
you need for continuing a line.

```
SomeVariable: someVeryLongFunctionNameBecauseItIsGoodToBeSpecific(10)
    +   3             # indent at +2 ensures that 3 is added into SomeVariable.
    -   OtherVariable # don't keep adding more indents, keep at +2 from original.

ArrayVariable: [
    # Array elements are at indent +1 from an open parenthesis, trailing commas optional:
    1
    2
    3
    4
    5
]

StoreVariable: [
    SomeValue: 100      # equivalent to "SomeValue": 100
    OtherValue: "hi"    # equivalent to "OtherValue": "hi"
]

# This is different than the `StoreVariable` because it essentially
# is an instance of a `{SomeValue: int, OtherValue: str}` type,
# which cannot have new fields added, even if it was mutable.
ObjectVariable: {
    SomeValue: 100
    OtherValue: "hi"
}
```

Note that the close parenthesis must be at the same indent as the line of the open parenthesis.
The starting indent of the line is what matters, so a close parenthesis can be on the same
line as an open parenthesis.

```
SomeValue: (
        (20 + 45)
    *   Continuing + The + Line + AtPlus2Indent - (
                Nested * Parentheses / Are + Ok
            -   Too
        )
)

AnotherLineContinuationVariable: CanOptionallyStartUpHere
    +   OkToNotHaveAPreviousLineStartingAtPlusTwoIndent * (
                KeepGoingIfYouLike
            -   HoweverLong
        ) + (70 - 30) * 3
```

Note that line continuations must be at least +2 indent, but can be more if desired.
Unless there are parentheses involved, all indents for subsequent line continuations
should be the same.

```
ExamplePlusThreeIndent; someType
...
ExamplePlusThreeIndent
    =       Hello
        +   World
        -   Continuing
```

Arguments supplied to functions are similar to stores and only require +1 indent.

```
if someFunctionCall(
    X
    Y: 3 + sin(Z)
)
    doSomething()

declaringAFunctionWithMultilineArguments(
    Times: int
    Greeting: string
    Name: string("World")   # argument with a default
): string
    # "return" is optional for the last line of the block:
    "$(Greeting), $(Name)! " * Times
```

You can call or declare functions with arguments at +2 indent, but then you must use commas at the end
of each argument line because +2 indent is the same as a line continuation.  The last argument's comma
is optional but recommended.

```
declaringAFunctionWithPlusTwoIndentArguments(
        Times: int,
        Greeting: string,
        Name: string = "World",
): string
    return "$(Greeting), $(Name)! " * Times

SomeLineContinuationExampleVariable:
        OptionalExpressionExplicitlyAtPlusTwoIndent
    +   5 - someFunction(
                AnotherOptionalExpression
            +   NextVariable
            -   CanKeepGoing
            /   Indefinitely,
                R: 123.4,
        )
```

### block parentheses and commas

You can use `$(` ... `)` to define a block inline.  The parentheses block "operator" is grammatically
the same as a standard block, i.e., going to a new line and indenting to +1.
This is useful for short `if` statements, e.g., `if SomeCondition $(doSomething())`.
Similarly, you can return arrays or objects in blocks via `$[...]` or `${...}`, respectively.

Similarly, note that commas are essentially equivalent to a new line and tabbing to the
same indent (indent +0).  This allows you to have multiple statements on one line, in any block,
by using commas.  E.g.,

```
# standard version:
if SomeCondition
    print("toggling shutoff")
    shutdown()

# comma version:
if SomeCondition
    # WARNING: NOT RECOMMENDED, since it's easy to accidentally skip reading
    # the statements that aren't first:
    print("toggling shutoff"), shutdown()

# block parentheses version
if SomeCondition $( print("toggling shutoff"), shutdown() )
```

If the block parentheses encapsulate content over multiple lines, note that
the additional lines need to be tabbed to +1 indent to match the +1 indent given by `$(`.
Multiline block parentheses are useful if you want to clearly delineate where your blocks
begin and end, which helps some editors navigate more quickly to the beginning/end of the block.

```
# multiline block parentheses via an optional `$(`
if SomeCondition $(
    print("toggling shutdown")
    print("waiting one more tick")
    print("almost..."), print("it's a bit weird to use comma statements")
    shutdown()
)
```

## comments

Comments come in three types: (1) end of line (EOL) comments, (2) mid-line comments,
and (3) multiline comments.  End of line comments are the hash `#` followed by any
character that is not `(`.  All characters after an EOL comment are ignored; the
compiler will start parsing on the next line.  A mid-line comment begins with `#(`
followed by any character that is not a `#`, and ends with `)#` *on the same line*.
All characters within the parentheses are ignored by the compiler.  Multiline comments
begin with `#(#` and end with the corresponding `#)#` *at the same tab indent*.
This means you can have nested multiline comments, as long as
the nested symbols are at a new tab stop, and they can even be broken (e.g., an unmatched
closing operator `#)#` as long as it is indented from the symbols which started the
multiline comment), although this is not recommended.  To qualify as a multiline comment
(either to open or close the comment), nothing else is allowed on the line before or after
(besides spaces), otherwise a compiler error is thrown.  All characters on all lines in between
the multiline comment symbols (e.g., `#(#` to `#)#`) are ignored.

Note that `#@` is an end-of-line comment reserved for the compiler, so if you use
them they may be deleted/updated in unexpected ways.

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
* `u32` : unsigned integer which can hold values from 0 to `2^32 - 1`, inclusive
* `u64` : unsigned integer which can hold values from 0 to `2^64 - 1`, inclusive
* `uXYZ` : unsigned integer which can hold values from 0 to `2^XYZ - 1`, inclusive,
    where `XYZ` is 128 to 512 in steps of 64, and generically we can use
    `unsigned[Bits: count]: what Bits $(8 $(u8), 16 $(u16), 32 $(u32), ...)`
* `count` : `s64` under the hood, intended to be >= 0 to indicate the amount of something.
* `index` : signed integer, `s64` under the hood.  for indexing arrays starting at 0.
* `ordinal` : signed integer, `s64` under the hood.  for indexing arrays starting at 1.

and similarly for `i8` to `i512`, using two's complement.  For example,
`i8` runs from -128 to 127, inclusive, and `u8(i8(-1))` equals `255`.
The corresponding generic is `signed[Bits: count]`.  We also define the
symmetric integers `s8` to `s512` using two's complement, but disallowing
the lowest negative value of the corresponding `i8` to `i512`, e.g.,
-128 for `s8`.  This allows you to fit in a null type with no extra storage,
e.g., `oneOf[s8, null]` is exactly 8 bits, since it uses -128 for null.
(See [nullable classes](#nullable-classes) for more information.)
Symmetric integers are useful when you want to ensure that `-Symmetric`
is actually the opposite sign of `Symmetric`; `-i8(-128)` is still `i8(-128)`.
The corresponding generic for symmetric integers is `symmetric[Bits: count]`.

Note that the `ordinal` type behaves exactly like a number but can be used
to index arrays starting at 1.  E.g., `Array[ordinal(1)]` corresponds to `Array[index(0)]`
(which is equivalent to other numeric but non-index types, e.g., `Array[0]`).
There is an automatic delta by +-1 when converting from `index` to `ordinal`
or vice versa, e.g., `ordinal(index(1)) == 2` and `index(ordinal(1)) == 0`.
Note however, that there's a bit of asymmetry here; non-index, numeric types
like `u64`, `count`, or `i32` will convert to `index` or `ordinal` without any delta.
It's only when converting between `index` and `ordinal` that a delta occurs.

## casting between types

hm-lang attempts to make casting between types convenient and simple.
However, there are times where it's better to be explicit about the program's intention.
For example, if you are converting between two number types, but the number is *not*
representable in the other type, the run-time will return an error.  Therefore you'll
need to be explicit about rounding when casting floating point numbers to integers,
unless you are sure that the floating point number is an integer.  Even if the float
is an integer, the maximum floating point integer is larger than most fixed-width integer
types (e.g., `u32` or `i64`), so errors can be returned in that case.  The big-integer type
`int` will not have this latter issue, but may return errors depending on memory constraints.
Notice we use `assert` to shortcircuit function evaluation and return an error result
(like throwing).  See [errors and asserts](#errors-and-asserts) for more details.

```
# Going from a floating point number to an integer should be done carefully...
X: dbl(5.43)
SafeCast: X int()                   # SafeCast is a result type (`hm[ok: int, NumberConversion uh]`)
# also OK: `SafeCast: int(X)`.
Q: X int() assert()                 # returns an error since `X` is not representable as an integer
Y: X round(Down) int() assert()     # Y = 5.  equivalent to `X floor()`
Z: X round(Up) int() assert()       # Z = 6.  equivalent to `X ceil()`.
R: X round() int() assert()         # R = 5.  rounds to closest integer, breaking ties at half
                                    #         to the integer larger in magnitude.

# Note, representable issues arise for conversions even between different integer types.
A: u32(1234)
Q: A u8() assert()                  # RUN-TIME ERROR, `A` is not representable as a `u8`.
B: u8(A & 255) assert()             # OK, communicates intent and puts `A` into the correct range.
```

Casting to a complex type, e.g., `oneOf[int, str](SomeValue)` will pass through `SomeValue`
if it is an `int` or a `str`, otherwise try `int(SomeValue)` if that is allowed, and finally
`str(SomeValue)` if that is allowed.  If none of the above are allowed, the compiler will
throw an error.  Note that nullable types absorb errors in this way (and become null), so
`oneOf[int, null](SomeSafeCast)` will be null if the cast was invalid, or an `int` if the
cast was successful.

To define a conversion from one class to another, you can define a global function
or a class method, like this:

```
scaled8: {
    # the actual value held by a `scaled8` is `My ScaledValue / my Scale`.
    @private
    ScaledValue: u8

    # static/class-level variable:
    @private
    my Scale: 32

    my new(Flt): hm[ok: me, uh: oneOf[Negative, TooBig]]
        ScaledValue: round(Flt * my Scale)
        if ScaledValue < 0
            return Negative
        if ScaledValue > u8 max() flt()
            return TooBig
        scaled8(ScaledValue u8() orPanic())

    # if there are no representability issues, you can create
    # a direct method to convert to `flt`;
    # this can be called like `flt(Scaled8)` or `Scaled8 flt()`.
    ::flt(): flt
        # `u8` types have a `flt` method.
        My ScaledValue flt() / my Scale flt()

    # if you have representability issues, you can return a result instead.
    ::int(): hm[ok: int, NumberConversion uh]
        if My ScaledValue % my Scale != 0
            NumberConversion NotAnInteger
        else
            My ScaledValue // my Scale
}

# global function; can also be called like `Scaled8 dbl()`.
dbl(Scaled8): dbl
    # note that we can access private variables of the class *in this file/module*
    # but if we weren't in this file we wouldn't have this access.
    Scaled8 ScaledValue dbl() / scaled8 Scale dbl()

# global function which returns a result, can be called like `Scaled8 u16()`
u[Bits: count](Scaled8): hm[ok: u[Bits], NumberConversion uh]
    if My ScaledValue % my Scale != 0
        NumberConversion NotAnInteger
    else
        My ScaledValue // my Scale
```

## types of types

Every variable has a reflexive type which describes the object/primitive that is held
in the variable, which can be accessed via the `lowerCamelCase` version of the
`UpperCamelCase` variable name.  For more methods of determining/transforming
internal types, see
[declaring more complicated types via `oneOf`](#declaring-more-complicated-types-via-oneOf).

```
# implementation note: `int` should come first so it gets tried first;
# `dbl` will eat up many values that are integers, including `4`.
X; oneOf[int, dbl] = 4
...
if x == int
    print("X is an integer")
elif x == dbl
    print("X is a double")
```

You can create a new instance of the same type using the `lowerCamelCase` type:

```
xType: x
Y: xType(1234)

# alternatively, you can use directly:
Y: x(1234)
```

Some more examples:

```
vector3: {X; dbl, Y; dbl, Z; dbl}

MyVector3: vector3(X: 1.2, Y: -1.4, Z: 1.6)

print(myVector3)                # prints `vector3`
print(vector3 == myVector3)     # this prints true
```

```
# we'll define `someType` as vector3, but it could become
# different than vector3 in the future.
someType; any(myVector3)

... # other logic that could change `someType`

# creates a dynamical instance; could be vector3 or something else.
SomeThing: someType(X: 5, Y: 6, Z: -7)
```

TODO: types of functions, shouldn't really have `new`.
TODO: we should discuss the things functions do have, like `myFunction Inputs`
and `myFunction Outputs`.

## type overloads (generics only)

Similar to defining a function overload, we can define type overloads for generic types.
For example, the generic result class in hm-lang is `hm[ok, uh: nonNull]`, which
encapsulates an ok value (`ok`) or a non-nullable error (`uh`).  For your custom class you
may not want to specify `hm[ok: myOkType, uh: myClassUh]` all the time for your custom
error type `myClassUh`, so you can define `hm[of]: hm[ok: of, uh: myClassUh]` and
use e.g. `hm[int]` to return an integer or an error of type `myClassUh`.  Shadowing variables is
invalid in hm-lang, but overloads are valid.  Note however that we disallow redefining
an overload, as that would be the equivalent of shadowing.

# operators and precedence

Almost all operations should have result-like syntax.  e.g., `A * B` can overflow (or run out of memory for `int`).
same for `A + B` and `A - B`.  `A // B` is safe.  However, instead of making hm-lang always assert,
we will use `multiply(~First A, Second A): hm[ok: a, NumberConversion uh]` and then have
`A1 * A2` always give an `a` result by panicking if we run out of memory.  i.e.,
```
number::*(You): me
    Result: multiply(Me, You)
    Result orPanic()
```
Primitive types will do overflow like in other languages without panicking, but you can use, e.g.,
`multiply(One U32, Another U32)` to return an error if it overflows.

TODO: add : , ; ?? postfix/prefix ?
TODO: add ... for dereferencing.  maybe we also allow it for spreading out an object into function arguments,
e.g., `myFunction(A: 3, B: 2, ...MyObject)` will call `myFunction(A: 3, B: 4, C: 5)` if `MyObject == {B: 4, C: 5}`.

| Precedence| Operator  | Name                      | Type/Usage        | Associativity |
|:---------:|:---------:|:--------------------------|:-----------------:|:-------------:|
|   0       |   `()`    | parentheses               | grouping: `(A)`   | ??            |
|           |   `[]`    | parentheses               | grouping: `[A]`   |               |
|           |   `{}`    | parentheses               | grouping: `{A}`   |               |
|           | `\\x/y/z` | library module import     | special: `\\a/b`  |               |
|           | `\/x/y/z` | relative module import    | special: `\/a/b`  |               |
|   1       |  ` ()`    | function call             | on fn: `a(B)`     | LTR           |
|   2       |   `::`    | impure read scope         | binary: `A::B`    | LTR           |
|           |   `;;`    | impure read/write scope   | binary: `A;;B`    |               |
|           |   ` `     | implicit member access    | binary: `A B`     |               |
|           |   ` []`   | parenthetical subscript   | binary: `A[B]`    |               |
|           |   `!`     | postfix moot = move+renew | unary:  `A!`      |               |
|           |   `?`     | postfix nullable          | unary:  `A?`/`a?` |               |
|   3       |   `^`     | superscript/power         | binary: `A^B`     | RTL           |
|           |   `**`    | also superscript/power    | binary: `A**B`    |               |
|           |   `--`    | unary decrement           | unary:  `--A`     |               |
|           |   `++`    | unary increment           | unary:  `++A`     |               |
|           |   `~`     | template/generic scope    | unary:  `~b`      |               |
|   4       |   `<>`    | bitwise flip              | unary:  `<>A`     | RTL           |
|           |   `-`     | unary minus               | unary:  `-A`      |               |
|           |   `+`     | unary plus                | unary:  `+A`      |               |
|           |   `!`     | prefix boolean not        | unary:  `!A`      |               |
|           |  `not`    | prefix boolean not        | unary:  `not A`   |               |
|   5       |   `>>`    | bitwise right shift       | binary: `A>>B`    | LTR           |
|           |   `<<`    | bitwise left shift        | binary: `A<<B`    |               |
|   6       |   `*`     | multiply                  | binary: `A*B`     | LTR           |
|           |   `/`     | divide                    | binary: `A/B`     |               |
|           |   `%`     | modulus                   | binary: `A%B`     |               |
|           |   `//`    | integer divide            | binary: `A//B`    |               |
|           |   `%%`    | remainder after //        | binary: `A%%B`    |               |
|   7       |   `+`     | add                       | binary: `A+B`     | LTR           |
|           |   `-`     | subtract                  | binary: `A-B`     |               |
|           |   `&`     | bitwise AND               | binary: `A&B`     |               |
|           |   `\|`    | bitwise OR                | binary: `A\|B`    |               |
|           |   `><`    | bitwise XOR               | binary: `A><B`    |               |
|           |   `??`    | nullish OR                | binary: `A??B`    |               |
|   8       |   `==`    | equality                  | binary: `A==B`    | LTR           |
|           |   `!=`    | inequality                | binary: `A!=B`    |               |
|   9       |  `and`    | logical AND               | binary: `A and B` | LTR           |
|           |   `or`    | logical OR                | binary: `A or B`  |               |
|           |  `xor`    | logical XOR               | binary: `A xor B` |               |
|   10      | `$(   )`  | block parentheses         | grouping: `$(A)`  | ??            |
|   11      |   `=`     | assignment                | binary: `A = B`   | LTR           |
|           |  `???=`   | compound assignment       | binary: `A += B`  |               |
|           |   `<->`   | swap                      | binary: `A <-> B` |               |
|   12      |   `,`     | comma                     | binary/postfix    | LTR           |


TODO: discussion on `~`

## function calls

Function calls are assumed whenever a function identifier (i.e., `lowerCamelCase`)
occurs before a parenthetical expression.  E.g., `print(X)` where `X` is a variable name or other
primitive constant (like `5`), or `anyFunctionName(Any + Expression / Here)`.
In case a function returns another function, you can also chain like this:
`getFunction(X)(Y, Z)` to call the returned function with `(Y, Z)`.

Note that methods defined on classes can be prefaced by member access (e.g., ` `, `::`, or `;;`),
and that path will still be used for function specification, even though member access has
less precedence.  So for example, if some instance `MyClassInstance` has a field `MyField`
which itself has a method `print`, then `MyClassInstance::MyField::print("Carrots")` 
(or `MyClassInstance MyField print("Carrots")`) will use the `print` function specified
on `MyField` rather than the global `print` function, even though the function call has
higher precedence than member access.  E.g., the compiler sees
`MyClassInstance::MyField::(print("Carrots"))`, but knows to use
`MyClassInstance::MyField::print` when running the internal `(print("Carrots"))`.

It is recommended to use parentheses where possible, to help people see the flow more easily.
E.g., `someFunction(SomeInstance SomeField someMethod()) FinalField` looks pretty complicated.
This would compile as `(someFunction(SomeInstance)::SomeField::someMethod())::FinalField`,
and including these parentheses would help others follow the flow.  Even better would be to
add descriptive variables as intermediate steps.

Deep dive!  Function calls are higher priority than member access because `someMethod() FinalField`
would compile as `someMethod( ()::FinalField )` otherwise, which would be a weird compile error.  

We don't allow for implicitly currying functions in hm-lang,
but you can explicitly curry like this:

```
someFunction(X: int, Y; dbl, Z. str):
    print("something cool with $(X), $(Y), and $(Z)")

curriedFunction(Z. str): someFunction(X: 5, Y; 2.4, Z.)

# or you can make it almost implicit like this:
curriedFunction: someFunction(X: 5, Y; 2.4, $$Z.)
```

## namespaces

Namespaces are used to avoid conflicts between two variable names that should be called
the same, i.e., for function convenience.  hm-lang doesn't support shadowing, so something
like this would break:

```
myFunction(X: int): int
    # define a nested function:
    # COMPILE ERROR
    doStuff(X: int): null
        # is this the `X` that's passed in from `myFunction`? or from `doStuff`?
        # most languages will shadow so that `X` is now `doStuff`'s argument,
        # but hm-lang does not allow shadowing.
        print(X)
    doStuff(X)
    doStuff(X: X // 2)
    doStuff(X: X // 4)
    X // 8
```

The way to get around this is to use a namespace for one or both conflicts.
Namespaces look like objects with field names, e.g., `MyNamespace MyVariableName`,
but where `MyNamespace` isn't an existing variable name.  hm-lang will infer that
it's a namespace if it's in the arguments of a function declaration.

```
myFunction(X: int): int
    # nested function is OK due to namespace:
    doStuff(Other X: int): null
        print(Other X)
    doStuff(X)
    doStuff(X: X // 2)
    doStuff(X: X // 4)
    X // 8
```

While shadowing is not allowed, you can use `@hide` to ensure you don't use the
other value accidentally.

```
myFunction(X: int): int
    # nested function is OK due to namespace:
    doStuff(Other X: int): null
        @hide X
        ...
        print(X)    # COMPILE ERROR, `X` was hidden from this scope.
        ...
    doStuff(X)
    doStuff(X: X // 2)
    doStuff(X: X // 4)
    X // 8
```

Similarly, you can define new variables with namespaces, in case you need a new variable
in the current space.  This might be useful in a class method like this:

```
myClass: {
    X; dbl

    # this is a re-implementation of `;;x(X. dbl): dbl`
    # but is a situation where you might like to use namespaces.
    ;;doSomething(New X. dbl): dbl
        # defines a variable `X` in the namespace `Old`:
        Old X: My X
        My X = New X
        Old X
```

One of the most convenient uses for namespaces is the ability to use elide argument
names when calling functions.  E.g., if you have a function which takes a variable named `X`,
but you already have a different one in scope, you can create a new variable with a namespace
`ExampleNamespace X: MyNewXValue` and then pass it into the function as
`myFunction(ExampleNamespace X)` instead of `myFunction(X: ExampleNamespaceX)`.
This also works with default-named variables.

```
someFunction(Input Index): null
    # `Input Index` is a default-named variable of type `index`, but we refer to it
    # within this scope using `Input Index`.
    even(Index): bool
        Index % 2 == 0
    # you can define other namespaces inline as well, like `Another` here:
    for Another Index: index < Input Index
        if even(Another Index)
            print(Another Index)
        
X: index = 100
someFunction(X)     # note that we don't need to call as `someFunction(Index: X)` or `someFunction(Input Index: X)`.
```

You can use the same namespace for multiple variables, e.g., `Input Rune` and `Input String`,
as long as the variable names don't overlap.  Like the member access operators below, the
namespace operator binds left to right.

There are some reserved namespaces which cannot be used as variable names, e.g., `Old`, `New`,
`Other`, `Unused`.

### full list of reserved namespaces

* `New`
* `Old`
* `First` - for the first operand in a binary operation (where order matters)
* `Second` - for the second operand in a binary operation (where order matters)
* `Unused` - for variables that aren't used in this block


## member access operators `::`, `;;`, and ` `

We use `::`, `;;`, and ` ` (member access) for accessing variables or functions that belong to
another object.  The `::` operator ensures that the RHS operand is read only, not write,
so that both LHS and RHS variables remain constant.  Oppositely, the `;;` scope operator passes
the RHS operand as writable, and therefore cannot be used if the LHS variable is readonly.
The implicit member access operator ` ` is equivalent to `::` when the LHS is a readonly variable
and `;;` when the LHS is a writable variable.  When declaring class methods, `::` and `;;` can be
unary prefixes to indicate readonly/writable-instance class methods.  They are shorthand for adding a
readonly/writable `I/Me/My` as an argument.

```
exampleClass: {
    # this `;;` prefix is shorthand for `renew(Me;, ...): null`:
    ;;renew(My X: int, My Y: dbl): null
        print("X $(X) Y $(Y)")

    # this `::` prefix is shorthand for `multiply(My: my, ...): dbl`:
    ::multiply(Z: dbl): dbl
        My X * My Y * Z
}
```


```
someClass: {X: dbl, Y: dbl, I; str[]}
SomeClass; someClass(X: 1, Y: 2.3, I: ["hello", "world"])
print(SomeClass::I)     # equivalent to `print(SomeClass I)`.  prints ["hello", "world"]
print(SomeClass::I[1])  # prints "world"
print(SomeClass I[1])   # also prints "world", using ` ` (member access)
SomeClass;;I[4] = "love"    # the fifth element is love.
SomeClass::I[7] = "oops"    # COMPILE ERROR, `::` means the array should be readonly.
SomeClass;;I[7] = "no problem"

NestedClass; someClass[] = []
NestedClass[1] X = 1.234        # creates a default [0] and [1], sets [1]'s X to 1.234
NestedClass[3] I[4] = "oops"    # creates a default [2] and [3], sets [3]'s I to ["", "", "", "", "oops"]
```

For class methods, `;;` (`::`) selects the overload with a writable (readonly) class
instance, respectively.  For example, the `array` class has overloads for sorting, (1) which
does not change the instance but returns a sorted copy of the array (`::sort(): me`), and
(2) one which sorts in place (`;;sort(): null`).  The ` ` (member access) operator will use
`I:` if the LHS is a readonly variable or `I;` if the LHS is writable.  Some examples in code:

```
# there are better ways to get a median, but just to showcase member access:
getMedianSlow(Array[]: int): hm[ok: int, uh: string]
    if Array count() == 0
        return uh("no elements in array, can't get median.")
    # make a copy of the array, but no longer allow access to it (via `@hide`):
    Sorted Array: @hide Array sort()   # same as `Array::sort()` since `Array` is readonly.
    ok(Sorted Array[Sorted Array count() // 2])

# sorts the array and returns the median.
getMedianSlow(Array[]; int): hm[ok: int, uh: string]
    if Array count() == 0
        return uh("no elements in array, can't get median.")
    Array sort()    # same as `Array;;sort()` since `Array` is writable.
    ok(Array[Array count() // 2])
```

Note that if the LHS is readonly, you will not be able to use a `;;` method.
To sum up, if the LHS is writable, you can use `;;` or `::`, and ` ` (member access) will
effectively be `;;`.  If the LHS is readonly, you can only use `::` and ` `, which are equivalent.

Subscripts have the same binding strength as member access operators since they are conceptually
similar operations.  This allows for operations like `++A[3]` meaning `++(A[3])` and
`--A B C[3]` equivalent to `--(((A;;B);;C)[3])`.  Member access binds stronger than exponentation
so that operations like `A B[C]^3` mean `((A::B)[C])^3`.

Note that `something() NestedField` becomes `(something())::NestedField` due to
the function call having higher precedence.  (You can also use destructuring if you want
to keep a variable for multiple uses: `{NestedField}: something()`.)

## prefix and postfix question marks `?`

The `?` operator binds strongly, but less so than member access, so `x a?` is equivalent
to `oneOf[x a, null]` and not `oneOf[x a, null]`.  This is for nested classes, e.g.,
`x: {a: int}`, so that we don't need to use `(x a)?` to represent `x oneOf[a, null]`.
Generally speaking, if you want your entire variable to be nullable,
it should be defined as `X?: int`.  `X: int?` works in this instance, but if you have
generic classes (like `array[elementType]`), then `X[]: int?` or `X[]?: int` would define
an array of nullable integers.  To declare a nullable array of integers, you'd use
`X?: array[int]`, `X?[]: int`, or `X?: int[]`.

Prefix `?` can be used to short-circuit function evaluation if an argument is null.
for a function like `doSomething(X?: int)`, we can use `doSomething(?X: MyValueForX)`
to indicate that we don't want to call `doSomething` if `MyValueForX` is null;
we'll simply return `Null`.  E.g., `doSomething(?X: MyValueForX)` is equivalent
to `if MyValueForX == Null $(Null) else $(doSomething(X: MyValueForX))`.
In this case `X` is already in scope, it becomes `doSomething(?X)` to elide the
variable name.

## prefix and postfix exclamation points `!`

The operator `!` is always unary (except when combined with equals for not equals,
e.g., `!=`).  It can act as a prefix operator "not", e.g., `!A`, pronounced "not A",
or a postfix operator on a variable, e.g., `Z!`, pronounced "Z mooted" (or "moot Z").  In the first
example, prefix `!` calls the `!(Me): bool` method defined on `A`, which creates a
temporary value of the boolean opposite of `A` without modifying `A`.  In the second
case, it calls a built-in method on `Z`, which moves the current data out of `Z` into
a temporary instance of whatever type `Z` is, and resets `Z` to a blank/default state.
This is a "move and reset" operation, or "moot" for short.  Overloads for prefix `!`
should follow the rule that, after e.g., `Z!`, checking whether `Z` evaluates to false,
i.e., by `!Z`, should return true.

Note, it's easier to think about positive boolean actions sometimes than negatives,
so we allow defining either `!!(Me): bool` or `!(Me): bool` on a class, the former
allowing you to cast a value, e.g., `A`, to its positive boolean form `!!A`, pronounced
"not not A."  Note, you cannot define both `!` and `!!` overloads for a class, since
that would make things like `!!!` ambiguous.

## superscripts/exponentiation

Note that exponentiation -- `^` and `**` which are equivalent --
binds less strongly than function calls and member access.  So something like `A[B]^2` will be
equivalent to `(A[B])^2` and `A B^3` is equivalent to `(A::B)^3`.

## bitshifts `<<` and `>>`

The notation `A << B`, called "bitshift left", means to multiply `A` by `2^B`.  For example, 
`A << 1 == A * 2`, `A << 2 == A * 4`, and `A << 3 == A * 8`.  Conversely, "bitshift right"
`A >> B` means to divide `A` by `2^B`.  Typically, we use bitshifts `<<` and `>>`
only for fixed-width integers, so that `A >> 5 == A // 32`, but there are overloads
for other types that will do the expected full division.  For floats, e.g., 16.0 >> 5 == 0.5.
Note that `A << 0 == A >> 0 == A`, and that negating the second operand is the same
as switching the operation, i.e., `A << B == A >> -B`.

In contrast to C/C++, bitshifts have a higher precedence than multiplication and division because they are
"stronger" operations: `100 << 3 == 800` whereas `100 * 3 == 300`, and the difference widens
as the second operand increases; similarly for division, bitshift right `>>` is "stronger"
than division at reducing the first operand via the second operand.
Thus `7 * 31 >> 3` groups as `7 * (31 >> 3) == 21` (and not as `(7 * 31) >> 3 == 27`),
and `105 // 5 << 2` groups as `105 // (5 << 2) == 5` and not `(105 // 5) << 2 == 84`.

Looking the other direction, bitshifts have lower precedence than exponentiation because
exponents are generally stronger -- as long as the first operand, the base of the exponentiation, is
larger than two.  E.g., `3^4 == 81` is greater than `3 << 4 == 48`.
Thus `2 << 3^2 == 2 << (3^2) == 1024` and not `(2 << 3)^2 == 256`, etc.

## division and remainder operators: `/` `//` `%` `%%`

The standard division operator, `/`, will promote integer operands to a rational return value.
E.g., `dbl(3/4) == 0.75` or `6/4 == rtl(3)/rtl(2)`.

The integer division operator, `//`, will return an integer, rounded towards zero, e.g.,`3//4 == 0`
and `-3//4 == 0`.  Also, `5//4 == 1` and `-5//4 == -1`, and `12 // 3 == 4` as expected.
If any operand is a double, the resulting value will be an integer double, e.g.,
`5.1 // 2 == 2.0`.

The modulus operator, `%`, will put the first operand into the range given by the second operand.
E.g., `5 % 4 == 1`, `123.45 % 1 == 0.45`.  Mathematically, we use the relation
`A % B == A - B * floor(A/B)`.

The remainder operator, `%%`, has the property that `A %% B == A - B * (A // B)`;
i.e., it is the remainder after integer division.  The remainder operator, `%%`,
differs from the modulus, `%`, when the operands have opposing signs.

|  `A`  |  `B`  | `floor(A/B)`  |  `A % B`  | `A // B`  | `A %% B`  |
|:-----:|:-----:|:-------------:|:---------:|:---------:|:---------:|
|   1   |   5   |      0        |     1     |     0     |     1     |
|  -1   |   5   |     -1        |     4     |     0     |    -1     |
|   1   |  -5   |     -1        |    -4     |     0     |     1     |
|  -1   |  -5   |      0        |    -1     |     0     |    -1     |
|  13   |   5   |      2        |     3     |     2     |     3     |
| -13   |   5   |     -3        |     2     |    -2     |    -3     |
|  13   |  -5   |     -3        |    -2     |    -2     |     3     |
| -13   |  -5   |      2        |    -3     |     2     |    -3     |
|  56   |   7   |      8        |     0     |     8     |     0     |
|  56   |  -7   |     -8        |     0     |    -8     |     0     |
|  6.78 |   1   |      6.0      |    0.78   |     6.0   |    0.78   |
| -6.78 |   1   |     -7.0      |    0.22   |    -6.0   |   -0.78   |

## less-than/greater-than operators

The less-than, less-than-or-equal-to, greater-than, and greater-than-or-equal-to
binary operators `<`, `<=`, `>`, and `>=` (respectively) have special return types.
This allows chaining like `W >= X < Y <= Z`, which will evaluate as truthy iff
`W >= X`, `X < Y`, and `Y <= Z`.  Note that these expressions are evaluated
left-to-right and the first inequality to fail will stop any further evaluations
or expressions from executing.

Internally, `X < Y` becomes a class which holds onto a value or reference of `Y`,
so that it can be chained.  Any future right operands take over the spot of `Y`.
Note, hm-lang doesn't have access to this class due to not holding onto references,
so `Q: X < Y > Z` instantiates `Q` as a boolean, not as this internal class.

## and/or/xor operators

The `or` operation `X or Y` has type `oneOf[x, y]` (for `X: x` and `Y: y`).
If `X` evaluates to truthy (i.e., `!!X == True`), then the return value of `X or Y` will be `X`.
Otherwise, the return value will be `Y`.  Note in a conditional, e.g., `if X or Y`, we'll always
cast to boolean implicitly (i.e., `if bool(X or Y)` explicitly).

Similarly, the `and` operation `X and Y` also has type `oneOf[x, y]`.  If `X` is falsey,
then the return value will be `X`.  If `X` is truthy, the return value will be `Y`.
Again, in a conditional, we'll cast `X and Y` to a boolean.

Thus, `and` and `or` act the same as JavaScript `&&` and `||`, for ease of transition.
However, to make things more consistent with the `xor` operator, if your return value
is nullable, `X or Y` will be `oneOf[x, y, null]` and `X and Y` will be `oneOf[y, null]`.
The result will be `Null` if both (either) operands are falsey for `or` (`and`).

```
NonNullOr: X or Y           # NonNullOr: if X $(X) else $(Y)
NonNullAnd: X and Y         # NonNullAnd: if !X $(X) else $(Y)
NullableOr?: X or Y         # NullableOr?: if X $(X) elif Y $(Y) else $(Null)
NullableAnd?: X and Y       # NullableAnd?: if !!X and !!Y $(Null) else $(Y)
```

The exclusive-or operation `X xor Y` has type `oneOf[x, y, null]`, and will return `Null`
if both `X` and `Y` are truthy or if they are both falsy.  If just one of the operands
is truthy, the result will be the truthy operand.  An example implementation:

```
# you can define it as nullable via `xor(~X, ~Y): oneOf[x, y, null]` or like this:
xor(~X, ~Y)?: oneOf[x, y]
    XIsTrue: bool(X)
    YIsTrue: bool(Y)
    if XIsTrue
        if YIsTrue $(Null) else $(X)
    elif YIsTrue
        Y
    else
        Null
```

Note that `xor` will thus return a nullable value, unless you do an assert.

```
NullableXor?: X xor Y
NonNullXor: X xor Y assert()     # will shortcircuit this block if `X xor Y` is null
```

## reassignment operators

Note that `:`, `;`, and `.` can assign values if they're also being declared.
Thus, `=` is only used for reassignment.  Many binary (two operand) operators
such as `*`, `/`, `+`, `-`, etc., also support being paired with reassignment.
As long as `X @op Y` has the same type as `X`, then we can do `X @op = Y` for
shorthand of `X = X @op Y` for any eligible binary operator `@op`.  Examples
include `X -= 5`, `Y &= 0x12`, etc.

Swapping two variables is accomplished by something like `A <-> B`.
Swap uses `<->` since `<=>` is reserved for a future spaceship operator
(encompassing `<`, `<=`, `==`, `=>` and `>` in one).  As a function, swap
would require mutable variables, e.g., `@orderIndependent swap(~X;, Other X;): null`.
If you define `swap` in this way for your custom class, it will be available
for the shorthand notation `<->`.

# variables

Variables are named using `UpperCamelCase` identifiers.  The `:` symbol is used
to declare deeply constant, non-reassignable variables, and `;` is used to declare
writable, reassignable variables.  Note when passed in as arguments to a function,
`:` has a slightly different meaning; a variable with `:` is readonly and not
necessarily deeply constant.  That will be discussed more later.

```
# declaring and setting a non-reassignable variable that holds a big integer
Y: int = 5
# also equivalent: `Y: 5` or `Y: int(5)`.

# using the variable:
print(Y * 30)

Y = 123     # COMPILER ERROR, Y is readonly and thus non-reassignable.
Y += 3      # COMPILER ERROR, Y is readonly and here deeply constant.
```

Mutable/reassignable/non-constant variables can use `VariableName = Expression`
after their first initialization, but they must be declared with a `;` symbol.

```
# declaring a reassignable variable that holds a big integer
X; int

# X is default-initialized to 0 if not specified.
X += 5      # now X == 5 is True.

# you can also define the value inline as well:
W; 7
# also equivalent, if you want to be explicit about the type.
W; int = 7
# also equivalent:
W; int(7)
```

Note that we use `;` and `:` as if it were an annotation on the variable name (rather
than the type) so that we don't have to worry about needlessly complex types like a writable
array of a constant integer.  Constant variables are deeply constant, and writable
variables are modifiable/reassignable, and we only have to think about this
(as programmers using the language) at the level of the variable itself,
not based on the type of the variable.  The underlying type is the same for both
readonly and writable variables (i.e., a writable type), but the variable is only
allowed to mutate the memory if it is declared as a writable variable with `;`.

## nullable variable types

To make it easy to indicate when a variable can be nullable, we reserve the question mark
symbol, `?`, placed after the variable name like `X?: int` or after a simple type like
`X: int?`.  Either example declares a variable `X` that
can be an integer or null.  The default value for an optional type is `Null`.
TODO: do we want to rename `null` to `absent`?  essentially we want the feature for
function calls, e.g., `fn(X?: int): q`
and container types, e.g., `{X?: possiblyNull(), Y: ...}` becomes `{Y: ...}` if `X` is null,
or `Store[Id] = Null` to remove `Id` from the store.
we could use `Store[Id] = Absent` and `Present` for non-absent.
I like `absent` but not `present` unfortunately, as it has other connotations.
maybe `missing` and `found`, or `deny`/`hide`/`veto` and `avow`.
`found`
oh, i really like `lost` (and `found`).  we could use `find[x]` to get the non-null
version of `x`.  maybe `if Set[5] == Found` or `if Set[5] isFound()` or `if Set["asdf"] == Lost`.
`lost` might be the wrong connotation, too, however, since it seems to indicate that it
was present at some point.  maybe keep `null` due to expectations in the industry.

For an optional type with more than one non-null type, we use `Y?: oneOf[someType, anotherType]`
or equivalently, `Y: oneOf[someType, anotherType, null]` (where `null` comes last).
Note that `null` should come last for casts to work correctly (e.g., `oneOf[null, int](1234)`
would cast to null rather than `int(1234)`).  Normally the first value in a `oneOf` is the
default, but if `null` or `Null` is an option, then null is the default.  

In either case, you can use `;` instead of `:` to indicate that the variable is writable.
Note that if you are defining a nullable variable inline, you should
prefix the operator with a `?`, e.g., `X?: nullableResult(...)`.  It is a compiler error
if a declared variable is nullable but `?` is not used, since we want the programmer to be
aware of the fact that the variable could be null, even though the program will take care
of null checks automatically and safely.  The `?` operator is required for any `oneOf` that
could take on a `Null` value, e.g., `oneOf[Null, Bread, Tomato, Mozzarella]`.

One of the cool features of hm-lang is that we don't require the programmer
to check for null on a nullable type before using it.  The executable will automatically
check for null on variables that can be null.  If a function can be null, the executable
will return null if the function is null, or otherwise execute the function.
This is also helpful for method chaining on classes (see more on those below).
If your code calls a method on an instance that is null, a null will be
returned instead (and the method will not be called).

```
# define a class with a method called `someMethod`:
someClass: { ::someMethod(): int }

Nullable?; someClass = Null

Value?: Nullable someMethod()   # `Value` has type `oneOf[int, null]` now,
                                # so it needs to be defined with `?`

# eventually we want to support things like this, where the compiler
# can tell if the type is nullable or not:
if Nullable != Null
    NonNullValue: Nullable someMethod()   # `NonNullValue` here must be `int`.
```

It is not allowed to implicitly cast from a nullable type to a non-nullable type,
e.g., `Value: Nullable someMethod()`.  The compiler will require that we define
`Value` with `?:`, or that we explicitly cast via whatever ending type we desire,
e.g., `Value: int(Nullable someMethod())`.  Note that `whateverType(Null)` is
the same as `whateverType()`, and number types (e.g., `int()` or `flt()`)  default
to 0.

Optional functions are defined in a similar way (cf. section on nullable functions),
with the `?` just after the function name, e.g., `someFunction?(...Args): returnType`.

## nullable classes

We will allow defining a nullable type by taking a type and specifying what value
is null on it.  For example, the signed types `s8` defines null as `-128` like this:

```
s8: i8

s8?: s8 {
    ::is(null): bool
        Me == -128
}
```

Similarly, `f32` and `f64` indicate that `NaN` is null via `::is(null): isNaN(Me)`,
so that you can define e.g. a nullable `f32` in exactly 32 bits.  To get this functionality,
you must declare your variable as type `s8?` or `f32?`, so that the nullable checks
kick in.

Note that any `oneOf` that can be null gets nullable methods.  They are defined globally
since we don't want to make users extend from a base nullable class.

```
# nullish or.
# `Nullable ?? X` to return `X` if `Nullable` is null,
# otherwise the non-null value in `Nullable`.
# TODO: maybe rename to `presentOr`
nonNullOr(First ~A?., Second A.): a
    what First A
        NonNull: $(NonNull)
        Null $(Second A)

# boolean or.
# `Nullable || X` to return `X` if `Nullable` is null or falsy,
# otherwise the non-null truthy value in `Nullable`.
truthyOr(First ~A?., Second A.): a
    what A
        NonNull:
            if NonNull
                NonNull
            else
                Second A
        Null $(Second A)
```

We don't currently intend to support Rust-like matching, e.g.,
`what A $( NonNull: and bool(NonNull) $(NonNull), Any: $(Second A) )`,
but that is a possibility in the future.

## nested/object types

You can declare an object type inline with nested fields.  The nested fields defined
with `:` are readonly, and `;` are writable.

```
Vector; {X: dbl, Y: dbl, Z: dbl} = {X: 4, Y: 3, Z: 1.5}
Vector X += 4   # COMPILER ERROR, field `X` of object is readonly 

# note however, as defined, Vector is reassignable since it was defined with `;`:
Vector = {X: 1, Y: 7.2}
# note, missing fields will be default-initialized.
Vector Z == 0   # should be True.

# to make an object variable readonly, use : when defining:
Vector2: {X: 3.75, Y: 3.25}
# or you can use `:` with an explicit type specifier and then `=`:
Vector2: {X: dbl, Y: dbl} = {X: 3.75, Y: 3.25}
# then these operations are invalid:
Vector2 X += 3          # COMPILER ERROR, variable is readonly, field cannot be modified
Vector2 = {X: 1, Y: 2}  # COMPILER ERROR, variable is readonly, cannot be reassigned
```

You can define a type/interface for objects you use multiple times.

```
# a plain-old-data class with 3 non-reassignable fields, X, Y, Z:
vector3: {X: dbl, Y: dbl, Z: dbl}

# you can use `vector3` now like any other type, e.g.:
Vector3: vector3(X: 5, Y: 10)
```

We also allow type definitions with writable fields, e.g. `{X; int, Y; dbl}`.
Depending on how the variable is defined, however, you may not be able to change
the fields once they are set.  If you define the variable with `;`, then you
can reassign the variable and thus modify the writable fields.  But if you define the
variable with `:`, the object fields are readonly, regardless of the field definitions.
Readonly fields on an object are normally deeply constant, unless the instance is
writable and is reset (either via `renew` or reassignment).  This allows you to
effectively change any internal readonly fields, but only in the constructor.

```
# mixMatch has one writable field and one readonly field:
mixMatch: {Wr; dbl, Ro: dbl}

# when defined with `;`, the object `MutableMix` is writable: mutable and reassignable.
MutableMix; mixMatch = {Wr: 3, Ro: 4}
MutableMix = mixMatch(Wr: 6, Ro: 3) # OK, MutableMix is writable and thus reassignable
MutableMix renew(Wr: 100, Ro: 300)  # OK, will update `Ro` to 300 and `Wr` to 100
MutableMix Wr += 4                  # OK, MutableMix is writable and this field is writable
MutableMix Ro -= 1                  # COMPILE ERROR, MutableMix is writable but this field is readonly
                                    # if you want to modify the `Ro` field, you need to reassign
                                    # the variable completely or call `renew`.

# when defined with `:`, the object is readonly, so its fields cannot be changed:
ReadonlyMix: mixMatch = {Wr: 5, Ro: 3}
ReadonlyMix = mixMatch(Wr: 6, Ro: 4)    # COMPILE ERROR, ReadonlyMix is readonly, thus non-reassignable
ReadonlyMix renew(Wr: 7, Ro: 5)         # COMPILE ERROR, ReadonlyMix is readonly, thus non-renewable
ReadonlyMix Wr += 4                     # COMPILE ERROR, ReadonlyMix is readonly
ReadonlyMix Ro -= 1                     # COMPILE ERROR, ReadonlyMix is readonly

# NOTE that in general, calling a function with variables defined by `;` is a different
# overload than calling with `:`.  Mutable argument variables imply that the arguments will
# be mutated inside the function, and because they are passed by reference, escape the function
# block with changes.  Data classes have overloads with writable arguments, which imply that
# the data class will take over the argument (via moot).  This implies a move (not copy) operation.
MyMixMatch: mixMatch(Wr; 5, Ro; 3)      # `;` is useful for taking arguments via a move.
# see section on writable/readonly arguments for more information.
```

Note that hm-lang takes a different approach than C++ when it comes to constant/readonly fields
inside of classes.  In C++, using `const` on a field type bars reassignment of the class instance.
(`non-static const member ‘const t T’, cannot use default assignment operator`.)
In hm-lang, readonly variables are not always deeply constant.  And in the case of readonly class
instance fields, readonly variables are set based on the constructor and shouldn't be modified
afterwards by other methods... except for the constructor if it's called again (i.e., via
`renew`ing the instance or reassignment).

### automatic deep nesting

We can create deeply nested objects by adding valid identifiers with consecutive `:`.  E.g.,
`{X: Y: 3}` is the same as `{X: {Y: 3}}`.

## temporarily locking writable variables

You can also make a variable readonly
for the remainder of the current block by using `@lock` before the variable name.
Note that you can modify it one last time with the `@lock` annotation, if desired.
Also note that the variable may not be deeply constant, e.g., if lambdas are called
which modify it, but you will not be able to explicitly modify it.

```
X; int = 4  # defined as writable and reassignable

if SomeCondition
    @lock X = 7 # locks X after assigning it to the value of 7.
                # For the remainder of this indented block, you can use X but not reassign it.
                # You also can't use writable, i.e., non-const, methods on X.
else
    @lock X # lock X to whatever value it was for this block.
            # You can still use X but not reassign/mutate it.

print(X)    # will either be 7 (if SomeCondition was true) or 4 (if !SomeCondition)
X += 5      # can modify X back in this block; there are no constraints here.
```

## hiding variables for the remainder of the block

We can hide a variable from the current block by using `@hide` before the variable name.
This doesn't descope the variable, but it does prevent the variable from being used by
new statements/functions.  `@hide` has similar behavior to the `@lock` annotation, in that
you can use the variable one last time with the annotation, if desired.

```
DateString; str("2023-01-01")

# after this line, `DateString` can't be accessed anymore.
Date: date(@hide DateString)

# note in some circumstances you may also want to include `!` to avoid copying the variable,
# if the underlying class makes use of that same type variable internally, e.g.:
Date: date(@hide DateString!)
# see discussion on `moot` for more information.
```

# functions

Functions are named using `lowerCamelCase` identifiers.  The syntax to declare
a function is `lowerCamelCaseName(FunctionArguments...): returnType`, but if
you are also defining the function the `returnType` is optional (but generally
recommended for multiline definitions).  Defining the function can occur inline
with `:` or over multiple lines using an indented block.

```
# declaring a function with no arguments that returns a big integer
v(): int

# setting/defining/initializing the function:
v(): int
    # `return` is optional for the last line in a block.
    # e.g., the following could have been `return 600`.
    600

# inline definition
v(): 600

# inline, but with explicit type
v(): int(600)

# function with X,Y double-precision float arguments that returns nothing
v(X: dbl, Y: dbl): null
    print("X = $(X), Y = $(Y), atan(Y, X) = $(\\math atan(X, Y))")
    # Note this could also be defined more concisely using $$,
    # which also prints the expression inside the parentheses with an equal sign and its value,
    # although this will print `\\math atan(X, Y) = ...`, e.g.:
    # print("$$(X), $$(Y), $$(\\math atan(X, Y))")

# Note that it is also ok to use parentheses around a function definition,
# but you should use the block parentheses notation `$()`.
excite(Times: int): str $(
    "hi!" * Times
)

# You can also define functions inline with the `$(` ... `)` block operator.
oh(Really; dbl): dbl $( Really *= 2.5, return 50 + Really )
```

## calling a function

You can call functions with arguments in any order.  Arguments must be specified
with the named identifiers in the function definition.  The only exception is
if the argument is default-named (i.e., it has the same name as the type), then you
don't need to specify its name.  We'll discuss that more in the
[default-name arguments](#default-name-arguments-in-functions) section.

```
# definition:
v(X: dbl, Y: dbl): null

# example calls:
v(X: 5.4, Y: 3)
v(Y: 3, Y: 5.4)

# if you already have variables X and Y, you don't need to re-specify their names:
X: 5.4
Y: 3
v(X, Y)     # equivalent to `v(X: X, Y: Y)` but the redundancy is not idiomatic.
v(Y, X)     # equivalent
```

### argument objects

TODO: this strays from the initial hm-lang idea of only function arguments being references.
can we return to this ideal or are these absolutely needed?  we probably need pointers
in some sense, but it would be nice to have some guarantees on things like this.  we
probably need a borrow checker (like Rust):

```
Result?; someNullableResult()
if Result is NonNull:
    print(NonNull)
    Result = someOtherFunctionPossiblyNull()
    # this could be undefined behavior if `NonNull` is a reference to the
    # nonnull part of `Result` but `Result` became null with `someOtherFunctionPossiblyNull()`
    print(NonNull)
```

In hm-lang, parentheses can be used to define argument objects, both as types
and instances.  As a type, `(X: dbl, Y; int, Z. str)` differs from the object
type `{X: dbl, Y; int, Z; str}`, for more than just the reason that `.` is invalid
in an object type.  When instantiated, argument objects with `;` and `:` fields
contain references to variables; objects get their own copies.  For convenience,
we'll use *arguments type* for an argument object type and *arguments instance* for
an argument object instance.

TODO: do `[]` need to be argument objects as well for the intent of calling
container methods like `Store[5, (Value;): ++Value]`?

Because they contain references, arguments instances cannot outlive the lifetime
of the variables they contain.

```
a: (X: dbl, Y; int, Z. str)

# This is OK:
X: 3.0
Y; 123
A: (X, Y;, Z. "hello")    # `Z` is passed by value, so it's not a reference.
A Y *= 37    # OK

# This is not OK:
returnA(Q: int): a
    # X and Y are defined locally here, and will be descoped at the
    # end of this function call.
    X: Q dbl() okOr(NaN) * 4.567
    Y; Q * 3
    # So we can't pass X, Y as references here.  Z is fine.
    (X, Y;, Z. "world")
```

Note that we can return arguments instances from functions, but they must be
defined with variables whose lifetimes outlive the input arguments instance.
For example:

```
X: 4.56
returnA(Q; int): (X: dbl, Y; int, Z. str)       # inline arguments type
    Q *= 37
    # X has a lifetime that outlives this function.
    # Y has the lifetime of the passed-in variable, which exceeds the return type.
    # Z is passed by value, so no lifetime concerns.
    (X, Y; Q, Z. "sky")
```

Argument objects are helpful if you want to have arguments that should be
references, but need nesting to be the most clear.  For example:

```
# function declaration
copy(From: (Pixels, Rectangle.), To: (Pixels;, Rectangle.): null

# function usage
Source Pixels: pixels() { #( build image )# }
Destination Pixels; pixels()
Size Rectangle: rectangle(Width: 10, Height: 7)

copy(
    From: (
        Source Pixels
        Size Rectangle + Vector2(X: 3, Y: 4)
    )
    To: (
        Destination Pixels;
        Size Rectangle + Vector2(X: 9, Y: 8)
    )
)

```

We can create deeply nested argument objects by adding valid identifiers with consecutive `:`/`;`/`.`.
E.g., `(X: Y: 3)` is the same as `(X: (Y: 3))`.  This can be useful for a function signature
like `run(After: duration, fn(): ~t): t`.  `duration` is a built-in type that can be built
out of units of time like `Seconds`, `Minutes`, `Hours`, etc., so we can do something like
`run(After: Seconds: 3, (): print("hello world!"))`, which will automatically pass
`(Seconds: 3)` into the `duration` constructor.  Of course, if you need multiple units of time,
you'd use `run(After: (Seconds: 6, Minutes: 1), (): print("hello world!"))` or to be explicit
you'd use `run(After: duration(Seconds: 6, Minutes: 1), (): print("hello world!"))`.


### default-name arguments in functions

For functions with one argument (per type) where the variable name doesn't matter,
you can use default-named variables.  For standard ASCII identifiers, the default-name identifier
is just the `UpperCamelCase` version of the `lowerCamelCase` type.

```
# this function declaration is equivalent to `f(Int: int): int`:
f(Int): int
    Int + 5

Z: 3
f(Z)        # ok
f(int(4.3)) # ok
f(5)        # ok
f(Int: 7)   # ok but overly verbose
```

If passing functions as an argument where the function name doesn't matter,
the default-named argument for a function is `fn`.  When *defining* a function
with a function argument, the default name must be used (`fn`); when declaring
the function, or when creating a lambda function for use inside the function,
the default name can be omitted.

```
# declaring a function doesn't require the default name:
q((): bool): null

# when defining the function, the default name `fn` must be used,
# so that it can be used inside the function block.
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

# or you can create a default-named function yourself:
q(fn(): bool
    random() > 0.5
)   # will print one of the above due to randomness.
# equivalent to `q((): random() > 0.5)`

# when defining a lambda (not declaring it), you can omit the name:
q((): True)

# or you can do multiline with a name-omitted lambda:
X; bool
q(():
    return X
)
# equivalent to `q((): X)`
# also equivalent to `q((): $(X))`
```

You only really need to use `$(X)` if you're also doing a side computation,
e.g., `$(SomeOtherCalculation, X)`.

### the name of a called function in an argument object

Calling a function with one argument being defined by a nested function will use
the nested function's name as the variable name.  E.g., if a function is called
`value`, then executing `whatIsThis(value())` will try to call the `whatIsThis(Value)`
overload.  If there is no such overload, it will fall back on `whatIsThis(Type)` where
`Type` is the return value of the `value()` function.

```
value(): int
    return 1234 + 5

whatIsThis(Value: int): null
    print(Value)

whatIsThis(Value: 10)   # prints 10
whatIsThis(value())     # prints 1239
```

You can still use `value()` as an argument for a default-named `Int` argument,
or some other named argument by renaming.

```
takesDefault(Int): string
    string(Int)

takesDefault(value())   # OK.  we try `Value: value()`
                        # and then the type of `value()` next

otherFunction(NotValue: int): string
    return "!" * NotValue

otherFunction(value())              # ERROR! no overload for `Value` or for `Int`.
otherFunction(NotValue: value())    # OK
```

This works the same for plain-old-data objects, e.g., `{value()}` corresponds to
`{Value: value()}`.  In case class methods are being called, the class name
and the class instance variable name are ignored, e.g., `{MyClassInstance myFunction()}`
is short-hand for `{MyFunction: MyClassInstance myFunction()}`.

### functions as arguments

A function can have a function as an argument, and there are a few different ways to call
it in that case.  This is usually a good use-case for lambda functions, which define
an inline function to pass into the other function.

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

detect(greet: ["hi", "hey", hello"][$$Int % 3] + ", world!") # returns 2
```

Note that the last example builds a lambda function using lambda arguments;
`$$Int` attaches an `Int` argument to the `greet` lambda.  We need two `$`
because `Array[$Int]` would call `["hi", ...][fn(Int): null]`.

You can define a lambda function with multiple arguments using multiple lambda
arguments.  The lambda argument names should match the arguments that the
input function declares.

```
runAsdf(fn(J: int, K: str, L: dbl): null): null
    print(fn(J: 5, K: "hay", L: 3.14))

runAsdf($K * $J + str($$L))   # prints "hayhayhayhayhay3.14"
```

Note that `$K`, `$J`, and `$$L` attach to the same lambda because `$$L` is
inside some parentheses.

### types as arguments

Generally speaking you can use generic/template programming for this case,
which infers the types based on instances of the type.

```
# generic function taking an instance of `x` and returning one.
doSomething(~X): x
    return X * 2

doSomething(123)    # returns 246
doSomething(0.75)   # returns 1.5
```
See [generic/template functions](#generictemplate-functions) for more details
on the syntax.

However, there are use cases where we might actually want to pass in
the type of something.  This makes the most sense as a generic type,
and can be done like this:

```
doSomething(~x): x
    return x(123)

print(doSomething(dbl)) # returns 123.0
print(doSomething(u8))  # returns u8(123)
```

### returning a type constructor

For returning a class/constructor, we need to use the syntax `new[x]` (or
`new[any]` if it's any type), because `doSomething(): x` returns
an instance `X` of class `x`.  Since returning an instance is by far the
most common case, we optimize for that rather than for returning a class.

```
# it's preferable to return a more specific value here, like
# `new[oneOf[int, dbl, string]]`, but `new[any]` works as well.
randomClass(): new[any]
    if random(dbl) < 0.5
        int
    elif random(dbl) < 0.5
        dbl
    else
        string
```

We can also pass in named types as arguments.  Here is an example
where we also return a type constructor.  Named types look like
function arguments but without an argument list (e.g., `className: t`
instead of `fn(Args): t`).

```
doSomething(~x, namedNew: ~y): new[oneOf[x, y]]
    return if random(dbl) < 0.5 $(x) else $(namedNew)

print(doSomething(int, namedNew: dbl))  # will print `int` or `dbl` with 50-50 probability
```

To return multiple constructors, you can use the [type tuple syntax](#type-tuples).

### unique argument names

Arguments must have unique names; e.g., you must not declare a function with two arguments
that have the same name.  This is obvious because we wouldn't be able to distinguish between
the two arguments inside the function body.

```
myFun(X: int, X: dbl): oneOf[int, dbl] = X      # COMPILER ERROR.  duplicate identifiers
```

However, there are times where it is useful for a function to have two arguments with the same
name, and that's for default-named arguments in a function where (1) *order doesn't matter*,
or (2) order does matter but in an established convention, like two sides of a binary operand.
An example of (1) is in a function like `max`:

```
@orderIndependent
max(Int, Other Int): int
    return if Int > Other Int
        Int
    else
        Other Int

max(5, 3) == max(3, 5)
```

The compiler is not smart enough to know whether order matters or not, so we need to annotate
the function with `@orderIndependent` -- otherwise it's a compiler error -- and we need to use
namespaces (e.g., `Other` with `Other Int`) in order to distinguish between the two variables
inside the function block.  When calling `max`, we don't need to use those namespaces, and
can't (since they're invisible to the outside world).

There is one place where it is not obvious that two arguments might have the same name, and
that is in method definitions.  Take for example the vector dot product:

```
vector2: {
    ;;renew(My X; dbl, My Y; dbl): Null

    @orderIndependent
    ::dot(Vector2): dbl
        return My X * Vector2 X + My Y * Vector2 Y
}
Vector2: vector2(1, 2)
OtherVector2: vector2(3, -4)
print(Vector2 dot(OtherVector2))    # prints -5
print(dot(Vector2, OtherVector2))   # equivalent, prints -5
```

The method `::dot(Vector2): dbl` has a function signature `dot(Me, Vector2): dbl`,
where `Me` is an instance of `vector2`, so ultimately this function creates a global
function with the function signature `dot(Vector2, Vector2): dbl`.  Therefore this function
*must* be order independent and should be annotated as such.  Otherwise it is
a compiler error.

As mentioned earlier, we can have order dependence in certain established cases, but these
should be avoided in hm-lang as much as possible, where we prefer unique names.
One example is the cross product of two vectors, where order matters but the
names of the vectors don't.  (The relationship between the two orders is also
somewhat trivial, `A cross(B) == -B cross(A)`, and this simplicity should be aspired to.)
The way to accomplish this in hm-lang is to use `First` and `Second` namespaces for
each variable.  If defined in a method, `Me` will be assumed to be namespaced as `First`,
so you can use `Second` for the other variable being passed in.  Using `First` and `Second`
allows you to avoid the compiler errors like `@orderIndependent` does.  You can also
use `You` as the variable name which in the class body is the same as `Second Me`.

```
vector3: {
    ;;renew(My X; dbl, My Y; dbl, My Z; dbl): Null

    # defined in the class body, we do it like this:
    ::cross(You): vector3(
        # you can use `You` or `Your` in this block:
        X: My Y * Your Z - My Z * Your Y
        Y: My Z * Your X - My X * Your Z
        Z: My X * Your Y - My Y * Your X
    )
}

# defined outside the class body, we do it like this:
# NOTE: both definitions are *not* required, only one.
cross(First Vector3, Second Vector3): vector3(
    X: First Vector3 Y * Second Vector3 Z - First Vector3 Z * Second Vector3 Y
    Y: First Vector3 Z * Second Vector3 X - First Vector3 X * Second Vector3 Z
    Z: First Vector3 X * Second Vector3 Y - First Vector3 Y * Second Vector3 X
)
```

One final note is that operations like `+` should be order independent, whereas `+=` should be
order dependent, since the method would look like this: `;;+=(Vector2)`, which is
equivalent to `+=(Me;, Vector2)`, where the first argument is writable.  These
two arguments can be distinguished because of the writeability.

## function overloads

Functions can do different things based on which arguments are passed in.
For one function to be an overload of another function, it must be defined in the same file,
and it must have different argument names or return values.  You can also have different
argument modifiers (i.e., `;` and `:` are different overloads, as are nullable types, `?`).

```
greet(String): null
    print("Hello, $(String)!")

greet(Say: string, To: string): null
    print("$(Say), $(To)!")

greet(Say: string, To: string, Times: int): null
    for Count: int < Times
        greet(Say, To)

# so you call this in different ways:
greet("World")
greet(To: "you", Say: "Hi")
greet(Times: 5, Say: "Hey", To: "Sam")

# note this is a different overload, since it must be called with `Say;`
greet(Say; string): null
    Say += " wow"
    print("$(Say), world...")

MySay; "hello"
greet(Say; MySay)   # prints "hello wow, world..."
print(MySay)            # prints "hello wow" since MySay was modified
```

Note also, overloads must be distinguishable based on argument **names**, not types.
Name modifiers (i.e., `;`, `:`, and `?`) also count as different overloads.

```
fibonacci(Times: int): int
    Previous; 1
    Current; 0
    for Count: int < Times
        NextPrevious: Current
        Current += Previous
        Previous = NextPrevious
    return Current

fibonacci(Times: dbl): int
    GoldenRatio: dbl = (1.0 + \\math sqrt(5)) * 0.5
    OtherRatio: dbl = (1.0 - \\math sqrt(5)) * 0.5
    return round((GoldenRatio^Times - OtherRatio^Times) / \\math sqrt(5))
# TODO: is this still correct?  we're specifying by name then filtering by type.
# COMPILE ERROR: function overloads of `fibonacci` must have unique argument names,
#                not argument types.

# NOTE: if the second function returned a `dbl`, then we actually could distinguish between
# the two overloads.  This is because default names for each return would be `Int` and `Dbl`,
# respectively, and that would be enough to distinguish the two functions.  The first overload
# would still be default in the case of a non-matching name (e.g., `Result: fibonnaci(Times: 3)`),
# but we could determine `Int: fibonacci(Times: 3)` to definitely be the first overload and
# `Dbl: fibonacci(Times: 7.3)` to be the second overload.
```

There is the matter of how to determine which overload to call.  We consider
only overloads that possess all the specified input argument names.  If there are some
unknown names in the call, we'll check for matching types.  E.g., an unnamed `4.5`, 
as in `fn(4.5)` or `fn(X, 4.5)`, will be checked for a default-named `Dbl` since `4.5`
is of type `dbl`.  Similarly, if a named variable, e.g., `Q` doesn't match in `fn(Q)`
or `fn(X, Q)`, we'll check if there is an overload of `fn` with a default-named type
of `Q`; e.g., if `Q` is of type `animal`, then we'll look for the `fn(Animal):` 
or `fn(X, Animal)` overload.

NOTE: we cannot match a function overload that has more arguments than we supplied in
a function call.  If we want to allow missing arguments in the function call, the declaration
should be explicit about that; e.g., `fn(X?: int): ...` or `fn(X: 0): ...`.
Similarly, we cannot match an overload that has fewer arguments than we supplied in the call.

Output arguments are similar, and are also matched by name.  This is pretty obvious with
something like `X: calling(InputArgs...)`, which will first look for an `X` output name
to match.  If there is no `X` output name, then the first non-null, default-named output
overload will be used.  E.g., if `calling(InputArgs...): dbl` was defined before
`calling(InputArgs...): str`, then `dbl` will win.  For an output variable with an explicit
type, e.g., `X: dbl = calling(InputArgs...)`, this will look for an overload with output
name `X` first, *then* look for the overload with a default-named `Dbl: dbl` type.
This might be surprising, but we want to enable easy conversions between types which can
implicitly convert to each other, so that we don't always need to be explicit about converting
iterators to containers, etc.  If there is no default-named `Dbl: dbl` output type, we'll look
for the first non-null, default-named output overload.  For function calls like
`X: dbl(calling(InputArgs...))`, we will lose all `X` output name information because
`dbl(...)` will hide the `X` name.  In this case, we'll look for an overload with a
default-named `Dbl: dbl` type output, then any non-null, default-named output overload.
For calls like`X: calling(InputArgs...) Dbl`, we will explicitly pick a default-named
`Dbl: dbl` output overload or throw a compiler error if none exists.

### nullable input arguments

When you call a function with an argument that is null, conceptually we choose the
overload that doesn't include that argument.  In other words, a null argument is
the same as a missing argument when it comes to function overloads.  Thus, we are
not free to create overloads that attempt to distinguish between all of these cases:
(1) missing argument, (2) present argument, (3) nullable argument, and (4) default argument.
Only functions for Cases (1) and (2) can be simultaneously defined; any other combination
results in a compile-time error.  Cases (3) and (4) can each be thought of as defining two function
overloads, one for the case of the missing argument and one for the case of the present argument.

Defining conflicting overloads, of course, is impossible.  Here are some example overloads;
again, only Cases (1) and (2) are compatible and can be defined together.

```
# missing argument (case 1):
someFunction(): dbl
    return 987.6

# present argument (case 2):
someFunction(Y: int): dbl
    return 2.3 * dbl(Y)

# nullable argument (case 3):
someFunction(Y?: int): dbl
    return if Y != Null $(1.77) else $(Y + 2.71)

# default argument (case 4):
someFunction(Y: 3): dbl
    return dbl(Y)
```

Note that writable arguments `;` are distinct overloads, which indicate either mutating
the external variable, taking it, or swapping it with some other value, depending on
the behavior of the function.  Temporaries are also allowed, so defaults can be defined
for writable arguments.

What are some of the practical outcomes of these overloads?  Suppose
we define present and missing argument overloads in the following way:

```
overloaded(): dbl
    return 123.4
overloaded(Y: int): string
    return "hi $(Y)"
```

The behavior that we get when we call `overloaded` will depend on whether we
pass in a `Y` or not.  But if we pass in a null `Y`, then we also will end up
calling the overload that defined the missing argument case.  I.e.:

```
Y?; int = ... # Y is maybe null, maybe non-null

# the following calls `overloaded()` if Y is Null, otherwise `overloaded(Y)`:
Z: overloaded(Y?) # also OK, but not idiomatic: `Z: overloaded(Y?: Y)`
# Z has type `oneOf[dbl, string]` due to the different return types of the overloads.
```

The reason behind this behavior is that in hm-lang, an argument list is conceptually an object
with various fields, since an argument has a name (the field name) as well as a value (the field value).
An object with a field that is `Null` should not be distinguishable from an object that
does not have the field, since `Null` is the absence of a value.  Thus, if we count up
the number of fields in an object using `count()`, we'll get results like this:
`object()::count() == 0`, `{Y: Null}::count() == 0`, and `{Y: 5}::count() == 1`.

Note that when calling a function with a nullable variable/expression, we need to
indicate that the field is nullable if the expression itself is null (or nullable). 
Just like when we define nullable variables, we use `?:` or `?;`, we need to use
`?:` or `?;` (or some equivalent) when passing a nullable field.  For example:

```
someFunction(X?: int): int
    return X ?? 1000

# when argument is not null:
someFunction(X: 100)    # OK, expression for X is definitely not null
someFunction(X?: 100)   # ERROR! expression for X is definitely not null

# when argument is an existing variable:
X?; Null
print(someFunction(X?))  # can do `X?: X`, but that's not idiomatic.

# when argument is a new nullable expression:
someFunction(X?: someNullishFunction())  # REQUIRED since someNullishFunction can return a Null
someFunction(X: someNullishFunction())   # ERROR! someNullishFunction is nullable, need `X?:`.

# where someNullishFunction might look like this:
someNullishFunction()?: int
    return if SomeCondition $( Null ) else $( 100 )
```

We also want to make it easy to chain function calls with variables that might be null,
where we actually don't want to call an overload of the function if the argument is null.

```
# in other languages, you might check for null before calling a function on a value.
# this is also valid hm-lang but it's not idiomatic:
X?: if Y != Null $(overloaded(Y)) else $(Null)

# instead, you should use the more idiomatic hm-lang version.
# putting a ? *before* the argument name will check that argument;
# if it is Null, the function will not be called and Null will be returned instead.
X?: overloaded(?Y)

# either way, X has type `oneOf[string, null]`.
```

You can use prefix `?` with multiple arguments; if any argument with prefix `?` is null,
then the function will not be called.

This can also be used with the `return` function to only return if the value is not null.

```
doSomething(X?: int): int
    Y?: ?X * 3    # Y is Null or X*3 if X is not Null.
    return ?Y       # only returns if Y is not Null
    #( do some other stuff )#
    ...
    return 3
```

### nullable output arguments

We also support function overloads for outputs that are nullable.  Just like with overloads
for nullable input arguments, there are some restrictions on defining overloads with (1) a
missing output, (2) a present output, and (3) a nullable output.  The restriction is a bit
different here, in that we cannot define (1) and (3) simultaneously for nullable outputs.
This enables us to distinguish between, e.g., `X?: myOverload(Y)` and `X: myOverload(Y)`,
which defines a nullable `X` or a non-null `X`.

```
# case 1, missing output (not compatible with case 3):
myOverload(Y: str): null
    print(Y)

# case 2, present output:
myOverload(Y: str): {X: int}
    {X: int(Y) orPanic("should be an integer")}

# case 3, nullable output (not compatible with case 1):
myOverload(Y: str): {X?: int}
    # this is essentially an implementation of `X?: int(Y), return {X}`
    what int(Y)
        Ok: ${X: Ok}
        Uh: ${}

{X}: myOverload(Y: "1234")  # calls (2) if it's defined, otherwise it's a compiler error.
{X?}: myOverload(Y: "abc")  # calls (1) or (3) if one is defined, otherwise it's a compiler error.
```

Note that if only Case 3 is defined, we can use a special notation to ensure that the return
value is not null, e.g., `{X: nonNull} = ...`.  This will throw a run-time error if the return
value for `X` is null.  Note that this syntax is invalid if Case 2 is defined, since there is
no need to assert a non-null return value in that case.
TODO: we probably need a way to do this for results as well, maybe `{X}: myOverload() assert()`,
which should also work for nonNull casting.  It also makes it clear there's an assertion
that's happening.

```
# normal call for case 3, defines an X which may be null:
{X?}: myOverload(Y: "123")

# special call for case 3; if X is null, this will throw a run-time error,
# otherwise will define a non-null X:
{X: nonNull}: myOverload(Y: "123")

# make a default for case 3, in case X comes back as null from the function
{X: -1}: myOverload(Y: "123")
```

If there are multiple return arguments, i.e., via an output type data class,
e.g., `{X: dbl, Y: str}`, then we support destructuring to help nail down
which overload should be used.  E.g., `{X:, Y:} = myOverload()` will
look for an overload with outputs named `X` and `Y`.  However, `X: myOverload()`
is not equivalent to `{X}: myOverload()`; if there is no destructuring
on the left-hand side, it will take the default return value.  You can also
explicitly type the return value, e.g., `Int: myOverload()` or `R: dbl = myOverload()`,
which will look for an overload with an `int` or `dbl` return type.

When matching outputs, the fields count as additional arguments, which must
be matched.  If you want to call an overload with multiple output arguments,
but you don't need one of the outputs, you can use the `@hide` annotation to
ensure it's not used afterwards.  E.g., `{@hide X:, Y: str} = myOverload()`.
You can also just not include it, e.g., `{Y: str} = myOverload()`, which is
preferred, in case the function has an optimization which doesn't need to
calculate `X`.

We also allow calling functions with any dynamically generated arguments, so that means
being able to resolve the overload at run-time.

### pass-by-reference or pass-by-value

Functions can be defined with arguments that are passed-by-value, e.g., via
`TemporaryValue. typeOfTheTemporary`.  This argument type can be called with
temporaries, e.g., `fn(ArgName. "my temp string")`, or with easily-copyable
types like `dbl` or `i32` like `MyI32: i32 = 5, fn(MyArg. MyI32)`, or
with larger-allocation types like `int` or `str` with an explicit copy:
`MyStr: "asdf...", fn(TmpArg. str(MyStr))`.  In any case, the passed-by-value
argument, if changed inside the function block, will have no effect on the
things outside the function block.  Inside the function block, pass-by-value
arguments are mutable, and can be reassigned or modified as desired.
Similar to Rust, variables that can be easily copied implement a `::copy(): me`
method, while variables that may require large allocations should only implement
`;;renew(Other Me): null` (essentially a C++ copy constructor).  This is done
by default for most hm-lang classes.

Functions can also be defined with writable or readonly reference arguments, e.g., via
`MutableArgument; typeOfTheWriteable` and `ReadonlyArgument: typeOfTheReadonly` in the
arguments list, which are passed by reference.  This choice has three important
effects: (1) readonly variables may not be deeply constant (see section on
[passing by reference gotchas](#passing-by-reference-gotchas)), (2) you can modify
writable argument variables inside the function definition, and (3) any
modifications to a writable argument variable inside the function block persist
in the outer scope.  Note that pass-by-constant-reference arguments are the default,
so `fn(Int): null` is the same as `fn(Int: int): null`.
TODO: maybe autogenerate a `fn(Int.): null` function for default arguments.

When passing by reference for `:` and `;` variables, we cannot automatically 
cast to the correct type.  Two exceptions: (1) if the variable is a temporary
we can cast like this, e.g., `MyValue: 123` can be used for a `MyValue: u8` argument,
and (2) child types are allowed to be passed by reference when the function asks
for a parent type.

Note that return types are never references, so one secondary difference between
`fn(Int.): ++Int` and `fn(Int;): ++Int` is that a copy/temporary is required
before calling the former and a copy is made for the return type in the latter.
The primary difference is that the latter will modify the passed-in variable in
the outer scope.  To avoid dangling references, any calls of `fn(Int;)` with a
temporary will actually create a hidden `int` before the function call.  E.g.,
`fn(Int; 12345)` will essentially become `UniquelyNamedInt; 12345` then
`fn(Int; @hide UniquelyNamedInt)`, so that `UniquelyNamedInt` is hidden from the
rest of the block.  See also [lifetimes and closures](#lifetimes-and-closures).

In C++ terms, arguments declared as `.` are passed as temporaries (`t &&`),
arguments declared as `:` are passed as constant reference (`const t &)`,
and arguments declared as `;` are passed as reference (`t &`).
Overloads can be defined for all three, since it is clear which is desired
based on the caller using `.`, `:`, or `;`.  Some examples:

```
# this function passes by value and won't modify the external variable
check(Arg123. string): string
    Arg123 += "-tmp"    # OK since Arg123 is defined as writable, implicit in `.`
    return Arg123

# this function passes by reference and will modify the external variable
check(Arg123; string): string
    Arg123 += "-writable"  # OK since Arg123 is defined as writable via `;`.
    return Arg123

# this function passes by constant reference and won't allow modifications
check(Arg123: string): string
    return Arg123 + "-readonly"

MyValue; string = "great"
check(Arg123. MyValue copy())   # returns "great-tmp".  needs `copy` since
                                # `.` requires a temporary.
print(MyValue)                  # prints "great"
check(Arg123: MyValue)          # returns "great-readonly"
print(MyValue)                  # prints "great"
check(Arg123; MyValue)          # returns "great-writable"
print(MyValue)                  # prints "great-writable"
```

Note that if you try to call a function with a readonly reference argument,
but there is no overload defined for it, this will be an error.  Similarly
for writable-reference or temporary variable arguments.

```
onlyReadonly(A: int): str
    return str(A) * A

MyA; 10
onlyReadonly(A; MyA)        # COMPILE ERROR, no writable overload for `onlyReadonly(A;)`
onlyReadonly(A. int(MyA))   # COMPILE ERROR, no temporary overload for `onlyReadonly(A.)`

print(onlyReadonly(A: 3))       # OK, prints "333"
print(onlyReadonly(A: MyA))     # OK, prints "10101010101010101010"

onlyMutable(B; int): str
    Result: str(B) * B
    B /= 2
    return Result

MyB; 10
onlyMutable(B: MyB)         # COMPILE ERROR, no readonly overload for `onlyMutable(B:)`
onlyMutable(B. int(MyB))    # COMPILE ERROR, no temporary overload for `onlyMutable(B.)`

print(onlyMutable(B; MyB))  # OK, prints "10101010101010101010"
print(onlyMutable(B; MyB))  # OK, prints "55555"

onlyTemporary(C. int): str
    Result; ""
    while C != 0
        Result append(str(C % 3))
        C /= 3
    Result reverse()
    Result

MyC; 5
onlyTemporary(C: MyC)       # COMPILE ERROR, no readonly overload for `onlyTemporary(C:)`
onlyTemporary(C; MyC)       # COMPILE ERROR, no temporary overload for `onlyTemporary(C;)`

print(onlyTemporary(C. 3))      # OK, prints "10"
print(onlyTemporary(C. MyC!))   # OK, prints "12"
```

Note there is an important distinction between variables defined as writable inside a block
versus inside a function argument list.  Mutable block variables are never reference types.
E.g., `B; A` is always a copy of `A`, so `B` is never a reference to the variable at `A`.
For a full example:

```
referenceThis(A; int): int
    B; A  # B is a copy of A
    A *= 2
    B *= 3
    return B

A; 10
print(referenceThis(A;))    # prints 30, not 60.
print(A)                    # A is now 20, not 60.
```

You are allowed to have default parameters for reference arguments, and a suitable
block-scoped (but hidden) variable will be created for each function call so that
a reference type is allowed.

```
fn(B; int(3)): int
    B += 3
    return B

# This definition would have the same return value as the previous function:
fn(B?: int): int
    if B != Null 
        B += 3
        return B    # note that this will make a copy.
    else
        return 6

# and can be called with or without an argument:
print(fn())         # returns 6
MyB; 10
print(fn(B; MyB))   # returns 13
print(MyB)          # MyB is now 13 as well.
print(fn(B; 17))    # MyB is unchanged, prints 20
```

Note that a mooted variable will automatically be considered a temporary argument
unless otherwise specified.

```
over(Load: int): str
    return str(Load)

over(Load; int): str
    return str(Load++)

over(Load. int): str
    return str(++Load)

Load; 100
print(over(Load!))  # calls `over(Load.)` with a temporary, prints 101
print(Load)         # Load = 0 because it was mooted, and was not modified inside the function

Load = 100
print(over(Load: Load!))    # calls `over(Load)` with a const temporary, prints 100
print(Load)                 # Load = 0 because it was mooted

Load = 100
print(over(Load; Load!))    # calls `over(Load;)` with a temporary, prints 100
print(Load)                 # Load = 0 because it was mooted

# for reference, without mooting:
Load = 100
print(over(Load;))          # calls `over(Load;)` with the reference, prints 100
print(Load)                 # Load = 101 because it was passed by reference
```

The implementation in C++ might look something like this:

```
// these function overloads are defined for `fn(Str;)` and `fn(Str)`:
void fn(string &String);        // reference overload for `fn(Str;)`
void fn(string &&String);       // temporary overload for `fn(Str.)`
void fn(const string &String);  // constant reference just for `fn(Str:)`
```

Implementation detail: while `.` corresponds to finality as a sentence ender, and thus might
appear to relate most closely to a readonly type, we choose to use `:` as readonly and `;` as
writable references due to the similarity between `:` and `;`; therefore `.` can be the
odd-one-out corresponding to a non-reference, pass-by-value type.

If you want to define multiple overloads, you can use the template `;:` (or `:;`) declaration
syntax for writable/readonly references.  There will be some annotation/macros which can be
used while before compiling, e.g., `@writable`/`@readonly` to determine if the variable is
writable or not.  Similarly, we can use templates like `:;.` for
readonly-reference/writable-reference/temporary.

```
myClass[of]: {
    X; of

    ;;take(X; of):
        My X = X!
    ;;take(X: of):
        My X = X

    # maybe something like this?
    ;;take(X;: of):
        My X = @mootOrCopy(X)
        # `@mootOrCopy(Z)` can expand to `@if @readonly(Z) $(Z) @else $(Z!)`
        # or maybe we can do something like `My X = X!:`
}
```

Alternatively, we can rely on some boilerplate that the language will add for us, e.g.,

```
myClass[of]: {
    X; of

    # these are added automatically by the compiler since `X; t` is defined.
    ;;x(Of; of): $( My X<->Of )
    ;;x(Of: of): $( My X = Of )
    ;;x(Of. of): $( My X = Of! )

    # so `take` would become:
    ;;take(X:;. of):
        I x(X:;!)
}
```

### passing by reference gotchas

For example, this function call passes `Array[3]` by reference, even if `Array[3]` is a primitive.

```
Array; int[] = [0, 1, 2, 3, 4]
myFunction(Array[3];)   # passed as writable reference
myFunction(Array[3]:)   # passed as readonly reference
myFunction(Array[3])    # also passed as readonly reference, it's the default.
```

You can switch to passing by value by using `.` or making an explicit copy:

```
Array; int = [0, 1, 2, 3, 4]
myFunction(int(Array[3]))   # passed by value (e.g., `myFunction(Int.)`):
```

Normally this distinction of passing by reference or value does not matter except
for optimization considerations.  The abnormal situations where it does matter in
a more strange way is when the function modifies the outside variable
in a self-referential way.  While this is allowed, it's not recommended!  Here is
an example with an array:

```
Array; [0, 1, 2, 3, 4]
sawOffBranch(Int;): null
    Array erase(Int)
    Int *= 10

sawOffBranch(Array[3];)
print(Array)    # prints [0, 1, 2, 40]
# walking through the code:
# sawOffBranch(Array[3];):
#     Array erase(Array[3]) # Array erase(3) --> Array becomes [0, 1, 2, 4]
#     Array[3] *= 10        # reference to 4 *= 10  --> 40
```

Note that references to elements in a container are internally pointers to the container plus the ID/offset,
so that we don't delete the value at `Array[3]` and thus invalidate the reference `Array[3];` above.
Containers of containers (and further nesting) require ID arrays for the pointers.
E.g., `MyStore["Ginger"][1]["Soup"]` would be a struct which contains `&MyStore`, plus the tuple `("Ginger", 1, "Soup")`.

TODO: discussion on how `Array[5]` gets passed by reference when used as an argument.
e.g., `myFunction(X; Array[5])` will do something like `Array[5, (T;): myFunction(X; T)]`.

Here is an example with a store.  Note that the argument is readonly, but that doesn't mean
the argument doesn't change, especially when we're doing self-referential logic like this.

```
Animals; ["hello": cat(), "world": snake(Name: "Woodsy")]

doSomething(Animal): string
    Result; Animal Name
    Animals["world"] = cat()        # overwrites snake with cat
    Result += " $(Animal speak())"
    return Result

print(doSomething(Animals["world"]))    # returns "Woodsy hisss!" (snake name + cat speak)
```

Here is an example without a container, which is still surprising, because
the argument appears to be readonly.  Again, it's not recommended to write your code like this;
but these are edge cases that might pop up in a complex code base.

```
MyInt; 123
notActuallyConstant(Int): null
    print("Int before $(Int)")
    MyInt += Int
    print("Int middle $(Int)")
    MyInt += Int
    print("Int after $(Int)")
    # Int += 5  # this would be a compiler error since `Int` is readonly from this scope.

notActuallyConstant(MyInt) # prints "Int before 123", then "Int middle 246", then "Int after 492"
```

Because of this, one should be careful about assuming that a readonly argument is deeply constant;
it may only be not-writable from your scope's reference to the variable.

In cases where we know the function won't do self-referential logic,
we can try to optimize and pass by value automatically.  However, we
do want to support closures like `nextGenerator(Int; int): (): ++Int`,
which returns a function which increments the passed-in, referenced integer,
so we can never pass a temporary argument (e.g., `Arg. str`) into `nextGenerator`.

### destructuring

If the return type from a function has multiple fields, we can grab them
using the notation `{Field1:, Field2;, Field3=} doStuff()`, where `doStuff` has
a function signature of `(): {Field1: field1, Field2: field2, Field3: field3, ...}`,
and `...` are optional ignored return fields.  In the example, we're declaring
`Field1` as readonly, `Field2` as writable, and `Field3` is an existing variable
that we're updating (which should be writeable), but any combination of `;`, `:`,
and `=` are possible.  The standard case, however, is to declare (or reassign) all
variables at the same time, which can be done with `{Field1, Field2}: doStuff()`
(`{Field1, Field2}; doStuff()`) for readonly (writeable) declaration + assignment,
or `{Field1, Field2} = doStuff()` for reassignment.

This notation is a bit more flexible than JavaScript, since we're
allowed to reassign existing variables while destructuring.  In JavaScript,
`const {Field1, Field2} = doStuff();` declares and defines the fields `Field1` and `Field2`,
but `{Field1, Field2} = doStuff();`, i.e., reassignment in hm-lang, is an error in JS.

Some worked examples follow, including field renaming.

```
fraction(In: string, Io; dbl): {RoundDown: int, RoundUp: int}
    print(In)
    RoundDown: Io round(Down)
    RoundUp: Io round(Up)
    Io -= RoundDown
    {RoundDown, RoundUp}

# destructuring
Io; 1.234
{RoundDown}: fraction(In: "hello", Io;)

# === calling the function with variable renaming ===
Greeting: "hello!"
InputOutput; 1.234      # note `;` so it's writable.
# just like when we define an argument for a function, the newly scoped variable goes on the left,
# so too for destructuring return arguments.  this one uses the default type of `RoundDown`:
{IntegerPart; roundDown} fraction(In: Greeting, Io; InputOutput)
# or if you want to put it on the right, you use the following TypeScript-friendly syntax:
{RoundDown as IntegerPart;} fraction(Greeting as In, InputOutput as Io;)
# you can specify the return type of the renamed variable like this to do casting:
{RoundDown as IntegerPart; int} fraction(In: Greeting, Io; InputOutput)

# there is no automatic de-nesting.
Io; 1.234
Result: fraction(In: "hello", Io;)
# `Result` is an object with these fields:
print(Result RoundDown, Result RoundUp)
```

Note that destructuring looks different than defining a lambda function due
to the difference in parentheses type and an extra `:` with lambda functions.
E.g., `{X: int, Y: str} someFunction(Z)` is destructuring,
but `(X: int, Y: str): someFunction(Z)` is defining a lambda.

Note, you can also have nullable output arguments.  These will be discussed
more in the function overload section, but here are some examples.

```
# standard definition:
wow(Lives: int)?: cat
    if Lives == 9
        cat()
    else
        Null
```

For nested object return types, there is some syntactic sugar for dealing with them.
Note, however, that nested fields won't help the compiler determine the function overload.

```
nest(X: int, Y: str): {W: {Z: {A: int}, B: str, C: str}}
    {W: {Z: {A: X}, B: Y, C: Y * X}}

# defines `A`, `B`, and `C` in the outside scope:
{W: Z: A:, W: B:, W: C:} = nest(X: 5, Y: "hi")
print(A)    # 5
print(B)    # "hi"

# equivalently:
{W: {Z: A:, B:, C:}} = nest(X: 5, Y: "hi")

# for renaming the variables, you can do something like this:
Q; int
R; str
S: str
{Q as W: Z: A, R as W: B, S as W: C} = nest(X: 3, Y: "whoa")

# or, defining the variables inline:
{
    Q; int as W: Z: A
    R; str as W: B
    S: str as W: C
} = nest(X: 3, Y: "whoa")
```

### dynamically determining arguments for a function

We allow for dynamically setting arguments to a function by using the `call` type,
which has a few fields: `Input` and `Output` for the arguments and return values
of the function, as well as optional `Info`, `Warning`, and `Error` fields for any issues
encountered when calling the function.  These fields are named to imply that the function
call can do just about anything (including fetching data from a remote server).

```
call: {
    # TODO: need to distinguish between readonly and writable references.
    #       this can be done on the pointer (e.g., Ptr[]; for writable
    #       and Ptr[]: for readonly) or here somehow.
    Input[str]; ptr any
    # we need to distinguish between the caller asking for specific fields
    # versus asking for the whole output.
    Output?; oneOf[multipleOutputs: any[str], oneOutput: any]
    # things printed to stdout via `print`:
    Print[]; string
    # things printed to stderr via `error`:
    Error[]; string

    # adds an argument to the function call.
    # e.g., `Call input(Name: "Cave", Value: "Story")`
    ;;input(Name: str, Value: ptr any): null
        My Input[Name] = Value

    # adds an argument to the function call.
    # e.g., `Call input(Cave: "Story")`
    ;;input(~Name: ptr any): null
        My Input[@@Name] = Name

    # adds a single-value return type
    ;;output(Any): null
        assert My Output == Null
        My Output = oneOutput(Any)

    # adds a field to the return type with a default value.
    # e.g., `Call output(FieldName: 123)` will ensure
    # `{FieldName}` is defined in the return value, with a
    # default of 123 if `FieldName` is not set in the function.
    ;;output(~Name: any): My output(Name: @@Name, Value: Name)

    # adds a field to the return type with a default value.
    # e.g., `Call output(Name: "FieldName", Value: 123)` will ensure
    # `{FieldName}` is defined in the return value, with a
    # default of 123 if `FieldName` is not set in the function.
    ;;output(Name: string, Value: any): null
        if My Output == Null
            My Output = multipleOutputs()
        assert My output == multipleOutputs
        My Output[Name] = Value 
}
```

When passing in a `call` instance to actually call a function, the `Input` field will be treated
as constant/read-only.  The `Output` field will be considered "write-only", except for the fact
that we'll read what fields are defined in `Output` (if any) to determine which overload to use.
This call structure allows you to define "default values" for the output, which won't get
overwritten if the function doesn't write to them.  To make things easier to reason about, you
can't influence the function overload by requesting nested fields in the output (e.g., `{X: {Y: Z}}`);
only fields directly attached to the `Output` (e.g., `{X, T}`) can influence the function overload.

Let's try an example:

```
# define some function to call:
someFunction(X: int): string
    return "hi" * X
# second overload:
someFunction(X: string): int
    return X countBytes()

MyString: string = someFunction(X: 100)     # uses the first overload
MyInt: int = someFunction(X: "cow")         # uses the second overload
CheckType1: someFunction(X: 123)            # uses the first overload since the type is `int`
CheckType2: someFunction(X: "asdf")         # uses the second overload since the type is `string`
Invalid: someFunction(X: 123.4)             # COMPILE ERROR: 123.4 is not referenceable as `int` or `string`

# example which will use the default overload:
Call; call
Call input(X: 2)
# use `Call` with `;` so that `Call;;Output` can be updated.
someFunction(Call;)
print(Call Output)  # prints "hihi"

# define a value for the object's Output field to get the other overload:
Call; call
Call input(X: "hello")
someFunction(Call;)
print(Call Output)  # prints 5

# dynamically determine the function overload:
Call; call
if someCondition()
    Call {input(X: 5), output("?")}
else
    Call {input(X: "hey"), output(-1)}

someFunction(Call;)
print(Call Output)  # will print "hihihihihi" or 3 depending on `someCondition()`.
```

Note that `call` is so generic that you can put any fields that won't actually
be used in the function call.  In this, hm-lang will return an error at run-time.

```
Call; call() { input(X: "4"), output(Value1: 123), output(Value2: 456) }
someFunction(Call;) assert()    # returns error since there are no overloads with {Value1, Value2}
```

If compile-time checks are desired, one should use the more specific
`myFn call` type, with `myFn` the function you want arguments checked against.

```
# throws a compile-time error:
Call; someFunction call() { input(X: "4"), output(Value1: 123), output(Value2: 456) }
# the above will throw a compile-time error, since two unexpected fields are defined for Output.

# this is ok (calls first overload):
Call2; someFunction call() { input(X: "4"), output(0) }
# also ok (calls second overload):
Call3; someFunction call() { input(X: 4), output("") }
```

Note that it's also not allowed to define an overload for the `call` type yourself.
This will give a compile error, e.g.:

```
# COMPILE ERROR!!  you cannot define a function overload with a default-named `call` argument!
someFunction(Call;): null
    print(Call Input["X"])
```

This is because all overloads need to be representable by `Call; call`, including any
overloads you would create with `call`.  Instead, you can create an overload with a
`call` argument that is not default-named.

```
someFunction(MyCall; call): null
    print(Call Input["X"]) # OK
```

## redefining a function

To declare a reassignable function, use `;` after the arguments.

```
greet(Noun: string); null
    print("Hello, $(Noun)!")

# you can use the function:
greet(Noun: "World")

# or you can redefine it:
greet(Noun: string); null
    print("Overwriting!")
# it's not ok if we use `greet(Noun: string): null` when redefining, since that looks like
# we're switching from writable to readonly.
```

It needs to be clear what function overload is being redefined (i.e., having the same function signature),
otherwise you're just creating a new overload (and not redefining the function).

## nullable functions

The syntax for declaring a nullable/optional function is to put a `?` after the function name
but before the argument list.  E.g., `optionalFunction?(...Args): returnType` for a non-reassignable
function and swapping `:` for `;` to create a reassignable function.
When calling a nullable function, unless the function is explicitly checked for non-null,
the return type will be nullable.  E.g., `X?: optionalFunction(...Args)` will have a
type of `oneOf[returnType, null]`.  Nullable functions are checked by the executable, so the
programmer doesn't necessarily have to do it.

A nullable function has `?` before the argument list; a `?` after the argument list
means that the return type is nullable.  The possible combinations are therefore the following:

* `normalFunction(...Args): returnType` is a non-null function
  returning a non-null `returnType` instance.  You can also
  declare this as `normalFunction: returnType(...Args)`.

* `nullableFunction?(...Args): returnType` is a nullable function,
  which, if non-null, will return a non-null `returnType` instance.
  Conversely, if `nullableFunction` is null, trying to call it will return null.
  You can also declare this as `nullableFunction?: returnType(...Args)`
  or `nullableFunction: returnType(...Args)?`.

* `nullableReturnFunction(...Args)?: returnType` is a non-null function
  which can return a nullable instance of `returnType`.  You can also
  declare this as `nullableReturnFunction: returnType?(...Args)`.

* `superNullFunction?(...Args)?: returnType` is a nullable function
  which can return a null `returnType` instance, even if the function is non-null.
  I.e., if `superNullFunction` is null, trying to call it will return null,
  but even if it's not null `superNullFunction` can still return null.
  You can also declare this as `superNullFunction?: returnType?(...Args)`
  or `superNullFunction: returnType?(...Args)?`.

Some examples:

```
# creating an optional method in a class:
parent: {
    ;;renew(My X: dbl, My Y: dbl): Null

    # note that this is a reassignable method, which means it is defined on a per-instance basis.
    ::optionalMethod?(Z: dbl); int
}

Example; parent(X: 5, Y: 1)

# define your own function for optionalMethod:
Example::optionalMethod(Z: dbl); int
    return floor(Z * My X)

Example optionalMethod(2.15)    # returns 10

# or set it to Null:
Example optionalMethod = Null

Example optionalMethod(3.21)    # returns Null

# child classes can define a "method" that overrides the parent's optional function:
child: parent {
    ::optionalMethod(Z: dbl); int
        return ceil(My X * My Y * exp(-Z))
}

Child; child(X: 6, Y: 2)

Child optionalMethod(0)     # returns 12

# but note that unless the method is protected, users can still redefine it.
Child::optionalMethod(Z: dbl): int
    return floor(Z)

Child optionalMethod(5.4)   # returns 5

# also note that while the child method appears to always be defined,
# we cannot stop a Null from being assigned here.  this is because
# if the instance is passed into a function which takes a parent class,
# that function scope can reassign the method to Null (since the parent
# class has no restrictions).
# TODO: we probably need to specify the whole function overload here.
Child optionalMethod = Null
# TODO: but this looks like defining the function to return Null.
Child::optionalMethod?(Z: dbl); int = Null      # should be the same
# TODO: maybe need to erase it.
erase(Child, optionalMethod?(Me, Z: dbl); int)

# therefore the executable will always check for Null before calling the method:
Child optionalMethod(1.45)  # returns Null
```

## generic/template functions

We can have arguments with generic types, but we can also have arguments
with generic names.

### argument type generics

For functions that accept multiple types as input/output, we define template types
inline, e.g., `copy(Value: ~t): t`, using `~` the first time you see the type in the
definition (reading left to right).  You can use any unused identifier for the new
type, e.g., `~q` or `~sandwichType`.

```
copy(Value: ~t): t
    print("got $(Value)")
    return Value

vector3: {X: dbl, Y: dbl, Z: dbl}
Vector3: vector3(Y: 5)
Result: copy(Value: Vector3)    # prints "got vector3(X: 0, Y: 5, Z: 0)".
Vector3 == Result           # equals True
```

Alternatively, you can add the new types in brackets just after the function name,
e.g., `copy[of: myTypeConstraints](Value: of): of`, which allows you to specify any
type constraints (`myTypeConstraints` being optional).  Note that types defined with
`~` are always "default named", but to get a default name in the type brackets
you need to use `of`; see [default named generic types](#default-named-generic-types).

TODO: probably need some discussion and examples, what about `copy[value](Value): value`?
does this need to be passed by name `copy(Value: 1)`?

### argument *name* generics: with associated generic type

TODO: restrictions here, do we need to only have a single argument, so that
argument names are unique?  it's probably ok if we have an `@orderIndependent`
or use `First ~T` and `Second ~U` to indicate order is ok.

To avoid needing an argument name when calling the function, we can use
name and type generics.  This differs from named generics where the lowerCamelCase
type doesn't match the UpperCamelCase identifier (like the example
`copy(Value: ~t): t` which requires naming the argument `Value` in function calls
like `copy(Value: MyVariable)`).

If the UpperCamelCase identifier matches the lowerCamelCase type of a generic,
then it's a default-named argument, e.g., `MyType; ~myType` or `T: ~t`.  There
is a shorthand for this which is more idiomatic: `~MyType;` or `~T`.  Here
is a complete example:

```
logger(~T): t
    print("got $(T)")
    return T

vector3: {X: dbl, Y: dbl, Z: dbl}
Vector3: vector3(Y: 5)
Result: logger(Vector3)     # prints "got vector3(X: 0, Y: 5, Z: 0)".
Vector3 == Result           # equals True

# implicit type request:
IntResult: logger(5)        # prints "got 5" and returns the integer 5.

# explicit type request:
DblResult: logger(dbl(4))   # prints "got 4.0" and returns 4.0
```

Note that you can use `myFunction(~T;)` for a writable argument.

### argument name generics: with different type

You can also define an argument with a known type, but an unknown name.

```
thisFunction(~TheName: int): null
    ArgumentName: str = @@TheName
    print("calling thisFunction with $(ArgumentName): $(TheName)")
```

We cannot define an argument name and an argument type to both be
generic and different.  `myFunction(~MyName: ~anotherType)` (COMPILE ERROR)
is needlessly verbose; if the type should be generic, just rely on what
is passed in: `myFunction(~MyName: myName)` or `myFunction(~MyName) for short.
TODO: this actually might make sense, where we're doing `myFunction(SomeValue: 3)`
or `myFunction(OtherValue: "asdf")`.

## pure functions and functions with side effects

Functions that are completely deterministic based on their inputs are called pure functions,
whereas functions with side effects, i.e., relying on memory in other locations to determine
the result, or modifying some memory elsewhere when executed, are not pure.  (External memory
in this situation means memory corresponding to variables not passed in to the function,
or memory in other locations, e.g., on the hard disk.)  Earlier proposals of hm-lang distinguished
between these types of functions notationally, but this is no longer required.

For examples of pure functions: `fn(X: dbl): X^2` is pure, as well as most mathematical
functions (`sin`, `cos`, `atan`, etc.).

Examples of impure functions include reading data from a file, which is non-deterministic,
since another program or thread could have modified the file in between reads, e.g., `read(File): string`.
A function which writes data to a file could be `write(File, String): null`
Random functions are of course impure as well.  Class methods are considered impure,
since they use data within the class instance to determine the result of the function call.

```
# we want to pass in a function here:
check(fn(Int): bool, Int): int
    Result: if fn(Int)
        Int // 2
    else
        2 * Int + 1
    print(Result)
    return Result

# but suppose we have a class which has a method that looks like this function:
exampleClass: {
    CheckTimes; int

    ;;someMethod(Int): bool
        ++My CheckTimes
        return (My CheckTimes % 2) >< (Int % 2)
}

ExampleClass; exampleClass

# if we allow calling this instance's method in the `check` function, we get
# results that depend on how many times the method has already been called:
check(ExampleClass;;someMethod, 3)  # returns 7
check(ExampleClass;;someMethod, 3)  # returns 1
```

Calling an impure function anywhere in a function makes that function impure.
Passing an impure function as an argument to a pure function similarly makes
the function call impure.

Importantly, the function `print` is considered a pure function, despite having some side
effects outside the program.  Standard output and error logging -- i.e., `stdout` and `stderr` --
are not normally connected to the input of the program, which is why we consider `print`
as pure.  Some uses of `stdout` and `stderr` can make the execution of the program impure (with
real side effects), e.g., if they are redirected to files which are then read by the program.
We don't consider this use case to provide a sufficient justification to make `print` an impure
function, since this can be considered as making the pure program execution as impure
via its being nested within an impure function (i.e., the user redirecting output to a file).
Plus, while redirecting `stderr` and `stdout` to files is a regular occurrence, reading from
those same files in the program generating those streams is not.

TODO: discussion on how it's impossible to copy impure functions
unless they are explicitly typed as copyable.  e.g., reading or writing to a file
should be easily copyable, reading/writing a class instance's variables might
be problematic (thread contention), similarly for random.


# classes

A class is defined with a `lowerCamelCase` identifier.
Class definitions must be constant/non-reassignable, so they are declared using
the `:` symbol.  Variables defined within the class body are known as instance
variables, and functions defined within the class body can have three types:
(1) class instance methods (or methods for short), (2) class functions (i.e.,
static methods in C++), and (3) instance functions.

Class methods (1) can access instance variables and call other class methods,
and require a `My: my`, `I; i`, or `Me. me` argument to indicate that it's an instance method.
(`I`, `Me`, and `My` are all interchangeable.)
Mutating methods -- i.e., that modify the class instance, `My/Me/I`, i.e., by modifying
its values/variables -- must be defined with `I;` in the arguments.
Non-mutating methods must be defined with `I:` and can access variables but not modify them.
Methods defined with `I.` indicate that the instance is temporary.
We'll use the notation `SomeClass..temporaryMethod()` to refer to a temporary instance method,
`SomeClass;;someMutatingMethod()` to refer to a mutable instance methods
and `SomeClass::someMethod()` to refer to readonly instance methods, with an implicit `I` due
to the class instance `SomeClass` being present.  Calling a class method does not require
the `..`, `;;`, or `::` prefix, but it is allowed, e.g.,

```
SomeClass; someClass("hello!")
SomeClass someMethod()      # ok
SomeClass::someMethod()     # also ok
SomeClass someMutatingMethod()  # ok
SomeClass;;someMutatingMethod() # also ok
# you can get a temporary by using moot (!):
MyResult1: SomeClass!..temporaryMethod()
# or you can get a temporary by creating a new class instance:
MyResult2: someClass("temporary")..temporaryMethod()
```

Note that you can overload a class method with readonly instance `::`, writable
instance `;;` versions, and temporary instance versions `..`.
Callers are recommended to be explicit and use `::`, `;;`, or `..` instead of ` ` (member access).
See the section on member access operators for how resolution of ` ` works in this case.
You can also call a class method via an inverted syntax, e.g., `someMethod(SomeClass)`,
`someMutatingMethod(SomeClass;)`, or `temporaryMethod(SomeClass!)`,
with any other arguments to the method added as well.
This is useful to overload e.g., the printing of your class instance, via defining
`print(Me)` as a method, so that `print(SomeClass)` will then call `SomeClass::print()`.
Similarly, you can do `count(SomeClass)` if `SomeClass` has a `count(Me)` method, which
all container classes have.  This also should work for multiple argument methods, since
`Array swap(Index1, Index2)` can easily become `swap(Array;, Index1, Index2)`.

And of course, class methods can also be overridden by child classes (see section on overrides).

Class functions (2) can't depend on the instance, i.e., `I/Me/My`, and must be declared
with a `@class` annotation before the function name in order to distinguish from class methods
(and instance functions), e.g., `@class myStaticFunction(): print("hi")`.  They can
be called from the class name, e.g., `x myClassFunction()`, or
from an instance of the class, e.g., `X myClassFunction()`.  Note that because of this,
we're not allowed to define class functions with the same overload as instance methods.
Similar to class functions are class variables, which are defined in an analogous way:
`@class StaticVariable: 123`, and which cannot shadow any class instance variables,
since `X StaticVariable` and `x StaticVariable` should be the same thing.

Instance functions (3) normally can't depend on any instance variables, but are declared
like instance methods, just without `Me/My/I` as an argument.
Instance functions can be different from instance to instance.
They cannot be overridden by child classes but they can be overwritten.  I.e.,
if a child class defines the instance function of a parent class, it overwrites the parent's
instance function; calling one calls the other.  To declare an instance function, we simply
declare a writable function, without a `Me/My/I`, inside the class definition.
In addition instance functions must be pure functions so that they can be copied freely.
TODO: we maybe should allow instance functions access to `Me/My/I`, implicitly; the `Me/My/I` will change
to the next instance when copied.

Class constructors are methods (1) which are defined using `;;renew(Args...)`,
which also allow you to renew the class instance as long as the variable is writable.
The first `renew` method defined in the class is also the default constructor,
which will be called with default-constructed arguments (if any) if a default
instance of the class is needed.  It is a compiler error if a `;;renew()` method
(with no arguments besides `Me/My/I;`) is defined after other `renew` methods (with arguments).
Note that `renew` should be a class instance method (1), i.e., `;;renew(...)`.

The reason why we use `@class` for static variables and functions is because
we don't want to require defining class instance variables and functions
with `My` in front of them, e.g., `vector2: {My X: dbl, My Y: dbl}`, but instead
only do `vector2: {X: dbl, Y: dbl}`).  The more common use case is to add fields
to your type, and we want to make inline class definitions super pain-free.
TODO: we probably can omit `@class` for functions like `i(...)`, `me(...)`, etc.

## class type and instance abbreviation

When defining methods or functions of all kinds, note that you can use `me` (or `i`/`my`)
to refer to the current class instance type.  E.g.,

```
myClass: {
    ::copy(): me    # OK
        print("logging a copy")
        return me(Me)   # or fancier copy logic
}
```

Inside a class method, you must use `Me/My/I` to refer to any other instance variables or methods,
e.g., `My X` or `I doStuffMethod()`, so that we can disambiguate calling a global function
that might have the same name as our class instance method, or using a global variable that
might have the same name as a class instance variable.

## class example

```
exampleClass: {
    # class instance variables can be defined here.
    # if they are public, a public constructor like `exampleClass(X;:. int)` will be created.
    X; int

    # classes must be resettable to a blank state, or to whatever is specified
    # as the starting value based on a `renew` function.  this is true even
    # if the class instance variables are defined as readonly.
    # NOTE:  defining this method isn't necessary since we already would have had
    # `exampleClass(X: int)` based on the public variable definition of `X`, but
    # we include it as an example in case you want to do extra work in the constructor
    # (although avoid doing work if possible).
    ;;renew(X; int): null
        My X = X!
    # or short-hand: `;;renew(My X; int): Null`
    # adding `My` to the arg name will automatically set `My X` to the passed in `X`.

    # create a different constructor.  constructors use the class reference `i` and must
    # return either an `i` or a `hm[ok: i, uh]` for any error type `uh`.
    # this constructor returns `i`:
    i(K: int): i(X: K * 1000)

    # some more examples of class methods (1):
    # prefix `::` (`;;`) is shorthand for adding `My: my` (`My; my`) as an argument.
    # this one does not change the underlying instance:
    ::doSomething(Int): int
        return My X + Int

    # this method mutates the class instance, so it uses `My; my` instead of `My:`:
    ;;addSomething(Int): null
        My X += Int

    # this reassignable method is defined on a per-instance basis. changing it on one class instance
    # will be isolated from any changes on another class instance.
    ::reassignableMethod(Int); string
        # this is the default implementation that all instances will start with.
        return string(My X + Int)

    # same as the other reassignable method above, but with a writable instance.
    ;;reassignableMutableMethod(Int); null
        # this is the default implementation that all instances will start with.
        My X -= Int

    # some examples of class functions (2):
    # this pure function does not require an instance, and cannot use instance variables:
    @class someStaticFunction(Y; int): int
        Y /= 2
        return Y!

    # this function does not require an instance, and cannot use instance variables,
    # but it can read/write global variables (or other files):
    @class someStaticImpureFunctionWithSideEffects(Y: int): null
        write(Y, File: "Y")

    # this function does not require an instance, and cannot use instance variables,
    # but it can read (but not write) global variables (or other files):
    @class someStaticImpureFunction(): int
        YString: read(File: "Y")
        return int(?YString) ?? 7

    # class instance functions (3) can be defined here; this is a *pure function*
    # that cannot depend on instance variables, however.  it can be set 
    # individually for each class instance, unlike a static class function.
    somePureFunction(): null
        print("hello!")

    # this class instance function (3) can be changed after the instance
    # has been created, as long as the instance is mutable.
    someMutablePureFunction(); null
        print("hello!")
}

Example; exampleClass(X: 5)
print(Example doSomething(7))   # should print 12
Example = exampleClass(X: 7)    # note: variable can be reassigned.
Example X -= 3                  # internal fields can be reassigned as well.

# note that if you define an instance of the class as readonly, you can only operate
# on the class with functions that do not mutate it.
ConstVar: exampleClass(X: 2)
ConstVar X += 3                 # COMPILER ERROR! `ConstVar` is readonly.
ConstVar = exampleClass(X: 4)   # COMPILER ERROR! variable is readonly.
```

You can also define your own custom methods/functions on a class outside of the class body.
Note that we do not allow adding instance functions or instance variables outside
of the class definition, as that would change the memory footprint of each class instance.
You can also use [sequence building](#sequence-building) outside of the class to define
a few methods, but don't use `:` since we're no longer declaring the class.

```
# static function that constructs a type or errors out (no `@class` annotation necessary)
exampleClass(Z: dbl): hm[ok: exampleClass, uh: str]
    X: Z round() int() assert(Uh: "Need `round(Z)` representable as an `int`.")
    exampleClass(X)

# a method which can mutate the class instance:
# this could also be defined as `exampleClass anotherMethod(My;, PlusK: int): null`.
exampleClass;;anotherMethod(PlusK: int): null
    My X += PlusK * 1000

# Don't use `:` here since we're not defining a class:
exampleClass {
    # static function; note that no `@class` annotation is needed here.
    # without sequence building, `exampleClass myAddedClassFunction(K: int): exampleClass`
    # is exactly how you'd define a class function.
    myAddedClassFunction(K: int): exampleClass
        exampleClass(X: K * 1000)

    # a method which keeps the instance readonly:
    ::myAddedMethod(Y: int): int
        My X * 1000 + Y * 100
}
```

If they are public, you can import these custom methods/functions in other files in two
ways: (1) import the full module via `import \/relative/path/to/file` or `import \\library/module`,
or (2) import the specific method/function via e.g.,
`{exampleClass myAddedClassFunction(K: int): exampleClass} = \/relative/path/to/file`
or `{exampleClass::myAddedMethod(Y: int): int} = \\library/module`.

Note that we recommend using named fields for constructors rather than static
class functions to create new instances of the class.  This is because named fields
are self descriptive and don't require named static functions for readability.
E.g., instead of `MyDate: dateClass fromIsoString("2020-05-04")`, just use
`MyDate: dateClass(IsoString: "2020-05-04")` and define the
`;;renew(IsoString: string)` method accordingly.

## unicode/localization support

We intend hm-lang to support all languages, and so the upper/lower-case requirements
may seem a bit strange in other alphabets.  To set a custom "UpperCamelCase" default name
for an instance of the class, use this notation:

```
örsted: {
    # define a custom UpperCamelCase name.
    I: Örsted 

    ... usual class methods ...
}

# Now we can use `Örsted` to mean a default-named variable of the class `örsted`:
doSomething(Örsted): bool
    return ...
```

We will throw a compile error when a class begins with a non-ascii letter, unless
the class defines the default-name of a variable of the class.  Also, you'll get
a compile error unless the custom default name is the first statement in the class
definition.

## public/private/protected visibility

We use annotations `@public`, `@private`, and `@protected` to indicate various
levels of access to a class instance's variables.  Public is default, and means
that the variable can be both accessed and modified by anyone.  Protected means
that the variable can be accessed and modified by friends, but for non-friends
the variable can only be accessed.  Friendship is defined by being in the same
directory as the module in question; i.e., being in the same file or being in
neighboring files in the same filesystem folder.  Private means the variable
can be accessed by friends only, and modified only by functions in the same module; i.e.,
the class (or other instances of the class) *or other functions in the same
file as the class definition*.  Non-friends are not able to access or modify private variables.

|  variable access  |  public   | protected |  private  |
|:-----------------:|:---------:|:---------:|:---------:|
|   module access   |   yes     |   yes     |   yes     |
|   module mutate   |   yes     |   yes     |   yes     |
|   friend access   |   yes     |   yes     |   yes     |
|   friend mutate   |   yes     |   yes     |   no      |
| non-friend access |   yes     |   yes     |   no      |
| non-friend mutate |   yes     |   no      |   no      |

The privacy for methods on a class follows the same table.
Using the method depends on visibility as well
as if the method modifies the class or not, i.e., whether the method was
defined as `mutatingMethod(My;): returnType` (AKA `;;mutatingMethod(): returnType`)
or `nonMutatingMethod(My): returnType` (AKA `::nonMutatingMethod(): returnType`).
Mutating methods follow the "mutate" visibility in the table above, and non-mutating methods follow
the "access" visibility in the table above.

To put into words -- `@public` methods can be called by anyone, regardless
of whether the method modifies the class instance or not.  `@protected`
methods which modify the class instance cannot be called by non-friends,
but constant `@protected` methods can be called by anyone.  `@private` methods which
modify the class instance can only be called by module functions, and
constant `@private` methods can be called by friends.

Note that reassignable methods, e.g., those defined with
`::someConstantMethod(...Args); returnType` or `;;someMutatingMethod(...Args); returnType`
can only be reassigned based on their visibility as if they were variables.
I.e., public reassignable methods can be reassigned by anyone,
protected reassignable methods can be reassigned by friends or module,
and private reassignable methods can only be reassigned within the module.

One final note, child classes are considered friends of the parent class,
even if they are defined outside of the parent's directory, and even if they
are defined in the same module as the parent (discouraged).  What this means
is they can modify public and protected variables defined on the parent instance,
and read (but not modify) private variables.  Overriding a parent class method
counts as modifying the method, which is therefore possible for public and protected
methods, but not private methods.

## getters and setters on class instance variables

Note that all variables defined on a class are given methods to access/set
them, but this is done with syntactical sugar.  That is,
*all uses of a class instance variable are done through getter/setter methods*,
even when accessed/modified within the class.  The getters/setters are methods
named the `lowerCamelCase` version of the `UpperCamelCase` variable,
with various arguments to determine the desired action.

```
# for example, this class:
example: {
    @visibility
    X; str = "hello"
}
W = example()
W X += ", world"
print(W X)  # prints "hello, world"

# expands to this:
example: {
    @invisible
    X; str

    # getter: calls an external function with X, which can
    #         avoid a copy if the function argument is readonly.
    @visibility
    ::x(fn(Str): ~t): fn(My X)

    # copy.  returns a copied value of `X`.  this method
    # has lower priority than the no-copy getter above.
    @visibility
    ::x(): My X

    # move X from this temporary; no-copy
    # TODO: this should probably also be defined along with
    #           ;;x()!: My X!
    #       i.e., they shouldn't need to both be defined.
    ..x(): My X!

    # move+reset (moot)
    @visibility
    ;;x()!: str
        return My X!

    # swapper: swaps the value of X with whatever is passed in
    #          returns the old value of X.
    @visibility
    ;;x(Str;.):
        My X <-> Str

    # modifier: allows the user to modify the value of X
    #           without copying it, using references.
    @visibility
    ;;x(fn(Str;): ~t): fn(My X;)

    # note that the no-copy getter and modifier can be 
    # read/write-template  defined like this:
    ;:x(fn(Str;:): ~t): fn(My X;:)
}
W = example()
W x(fn(Str;)
    Str += ", world"
)
W x((Str): print(Str))
```

If you define overloads for any of these methods on child classes,
they will be used as the getters/setters for that variable.  Anyone
trying to access the variable (or set it) will use the overloaded methods.
Note that only one of the copy or getter methods needs to be defined
on a class; the compiler will automatically use the defined method internally
even if the undefined method is requested.  The same is true of swapper
and modifier classes.

```
# a class with a copy method gets a getter method automatically:
justCopyable: {
    ::someVar(): int
        return 1000

    #(#
    # the following becomes automatically defined:
    ::someVar(fn(Int): ~t): t
        SomeVar: My someVar()
        return fn(SomeVar)
    #)#
}

# a class with a getter method gets a copy method automatically:
justGettable: {
    @invisible
    SomeVar; int

    ::someVar(fn(Int): ~t): fn(My SomeVar)

    #(#
    # the following becomes automatically defined:
    ::someVar(): int
        return My someVar(fn(Int): Int)
    #)#
}

# a class with a swapper method gets a modifier and move+reset method automatically:
justSwappable: {
    @invisible
    SomeVar; int

    @visibility
    ;;someVar(Int;): null
        My SomeVar <-> Int
        # you can do some checks/modifications on SomeVar here if you want,
        # though it's best not to surprise developers.  a default-constructed
        # value for `SomeVar` (e.g., in this case `Int: 0`) should be allowed
        # since we use it in the modifier to swap out the real value into a temp.
        # if that's not allowed, you would want to define both the swapper
        # and modifier methods yourself.

    #(#
    # the following modifier becomes automatically defined:
    ;;someVar(fn(Int;): ~t): t
        Temporary; int
        # swap SomeVar into Temporary:
        My someVar(Temporary;)    # could also write `My SomeVar <-> Temporary`
        T: fn(Temporary;)
        # swap Temporary back into SomeVar:
        My someVar(Temporary;)
        return T!

    # the following temp value modifier becomes automatically defined
    ;;someVar(Int.): null
        My someVar(Int;)

    # and the following move+reset method becomes automatically defined:
    ;;someVar()!: t
        Temporary; int
        # swap SomeVar into Temporary:
        My someVar(Temporary;)    # could also write `My SomeVar <-> Temporary`
        return Temporary!
    #)#
}

# a class with a modifier method gets a swapper and move+reset method automatically:
justModdable: {
    @invisible
    SomeVar; int

    ;;someVar(fn(Int;): ~t): t
        T: fn(My SomeVar;)
        # you can do some checks/modifications on SomeVar here if you want,
        # though it's best not to surprise developers
        return T!

    #(#
    # the following swapper becomes automatically defined:
    ;;someVar(Int;.): null
        My someVar((Old Int;): null
            Int <-> Old Int
        )

    # the following temp modifier becomes automatically defined:
    ;;someVar(Int.): null
        My someVar((Old Int;): null
            Old Int = Int!
        )

    # and the following move+reset method becomes automatically defined:
    ;;someVar()!: t
        Result; int
        My someVar((Int;): null
             Result <-> Int
        )
        return Result!
    #)#
}

# A class with just a moot (move+reset) method doesn't get swapper and modifier methods automatically;
# the moot method is in some sense a one way modification (pull only) whereas swapper and
# modifier methods are two way (push and pull).
```

TODO: some example of child class overriding parent class getter/setters.
TODO: parent class with getter defined, child class with copy defined.

## parent-child classes and method overrides

You can define parent-child class relationships with the following syntax.
For one parent, `childClass: parentClassName {#( child methods )#}`.  Multiple
inheritance is allowed as well, via `allOf[parent1, parent2] {#( child methods )#}`.
We can access the current class instance using `My` (or `Me`/`My`),
and `i` (or `me`/`my`) will be the current instance's type.  Thus, `i`/`me`/`my` is
the parent class if the instance is a parent type, or a subclass if the instance
is a child class.  E.g., a parent class method can return a `me` type instance,
and using the method on a subclass instance will return an instance of the subclass.
If your parent class method truncates at all (e.g., removes information from child classes),
make sure to return the same `parentClassName` that defines the class.

We can access member variables or functions that belong to that the parent type,
i.e., without subclass overloads, using the syntax `parentClassName someMethod(My, ...Args)`
or `ParentClassName::someMethod(...Args)`.  Use `My;` to access variables or methods that will
mutate the underlying class instance, e.g., `parentClassName someMethod(My;, ...Args)`
or `ParentClassName;;someMethod(...Args)`.  hm-lang doesn't have a `super` keyword
because we want inheritance to be as clear as composition for how method calls work.

Some examples:

```
animal: {
    ;;renew(My Name: string): Null

    # define two methods on `animal`: `speak` and `go`.
    # these are "abstract" methods, i.e., not implemented by this base class.
    ::speak(): null
    ::go(): string

    # this method is defined, so it's implemented by the base class.
    # derived classes can still change it, though.
    ::escape(): null
        print("$(My Name) $(I go()) away!!")

    # a method that returns an instance of whatever the class instance
    # type is known to be.  e.g., an animal returns an animal instance,
    # while a subclass would return a subclass instance:
    ::clone(): me
        return me(My Name)
}

snake: animal {
    # if no `renew` functions are defined,
    # child classes will inherit their parent `renew()` methods.

    ::speak(): null
        print("hisss!")
    ::go(): string
        return "slithers"

    # no need to override `clone`, since we can create a snake using a name.
}

Snake: snake(Name: "Fred")
Snake escape()  # prints "Fred slithers away!!"

cat: animal {
    # here we define a `renew` method, so the parent `reset` methods
    # become hidden to users of this child class:
    ;;renew(): null
        # can refer to parent methods using the `UpperCamelCase` class name:
        Animal renew(Name: "Cat-don't-care-what-you-name-it")

    ::speak(): null
        print("hisss!")
    ::go(): string
        return "saunters"

    ::escape(): null
        print("CAT ESCAPES DARINGLY!")

    # the parent `clone()` method won't work, so override:
    ::clone(): me
        # cats are essentially singletons, that cannot have their own name;
        return me()
}

Cat: cat()
Cat escape()    # prints "CAT ESCAPES DARINGLY!"
```

We have some functionality to make it easy to pass `renew` arguments to
a parent class via the `ParentClassName` namespace in the constructor arguments.
This way you don't need to add the boiler plate logic inside the
constructor like this `;;renew(ParentArgument): $( Parent renew(ParentArgument) )`,
you can make it simpler like this instead:

```
horse: animal {
    ;;renew(Animal Name: str, My Owner: str, NeighTimes: int = 0)
        for Int: int < NeighTimes
            This speak()

    ::speak(): null
        print("Neigh!")

    ::go(): string
        return "gallops"
}

Horse: horse(Name: "James", Owner: "Fred", NeighTimes: 1)
print(Horse Owner)  # Fred
print(Horse Name)   # James
```

All abstract base classes also provide ways to instantiate using lambda functions.
All abstract methods must be defined for the instance to be created, and if a
`reset` method is defined on the parent, any arguments passed into the first reset
(i.e., which is the default constructor) should be defined for the lambda class.
While these don't look like lambda functions, they use the notation `::speak(): null`
to mean `speak(My): null`, which is fine as a lambda.

```
WeirdAnimal: animal(
    Name: "Waberoo"
    ::speak(): null
        print("Meorooo")
    ::go(): "meanders"
    ::escape(): null
        # to call the parent method `escape()` in here, we can use this:
        animal::escape()
        print("$(My Name) $(I go()) back...")
        # or we can use this:
        animal escape(Me)
)

WeirdAnimal escape()    # prints "Waberoo ... meanders ... meanders back ... meanders away!!"
```

## inheritance and dynamic allocation

We will likely use C/C++ to implement hm-lang at first, i.e., so that it transpiles to C/C++.
In C++, a variable that is typed as a parent instance cannot be a child instance in disguise;
if a child instance is assigned to the variable, the extra derived bits get sliced off.  This
helps avoid dynamic allocation, because the memory for the parent class instance can be allocated
on the stack.  On the other hand, if the variable is a pointer to a parent instance, the variable
can actually point to a child instance in disguise.  This is great in practice for object-oriented
programming, since you can use the child instance in place of the parent instance; as long as
the child class fulfills the same contract as the parent class, it shouldn't matter the exact
implementation.  But this generally requires dynamic memory allocation, which has a cost.

In hm-lang, we want to make it easy for variables of a parent class to be secretly instances
of child classes, so by default we are paying the cost and dynamically allocating non-primitive
types.  That way, we can easily do things like this:

```
SomeAnimal; animal
SomeAnimal = snake(Name: "Josie")
SomeAnimal go()   # prints "slithers"
SomeAnimal = cat()
SomeAnimal go()   # prints "saunters"
```

TODO: discuss wrapper class which allocates enough data for any child class; if some child
class takes up too much memory it's created as a new unique pointer.

This is less surprising than the C++ behavior.  But in cases where users want to gain back
the no-dynamically-allocated class instances, we have a `@only` annotation that can be used
on the type.  E.g., `SomeVariable: @only someType` will ensure that `SomeVariable` is
stack allocated (non-dynamically).  If defined with `;`, the instance can still be modified,
but it will be sliced if some child instance is copied to it.  To prevent confusion, we
enforce that upcasting (going from child to parent) must be done *explicitly*.  For example:

TODO: classes should probably be allowed to be marked final.  e.g., `i64` and similar
fixed-width integers should be `final` so that we don't need to worry about vtables,
or specifying `@only i64`.  classes that are `final` would not need to be marked `@only`.

```
mythologicalCat: cat {
    # extra field which will get sliced off when converting from
    # mythologicalCat to cat:
    Lives; 9
}

Cat; @only cat
MythologicalCat; mythologicalCat()

Cat = MythologicalCat       # COMPILER ERROR, implicit cast to `@only cat` not allowed.
Cat = cat(MythologicalCat)  # OK.  explicit upcast is allowed.

OtherCat; @only cat
Cat <-> MythologicalCat     # COMPILER ERROR.  swaps not allowed unless both types are the same.
OtherCat <-> Cat            # OK.  both variables are `@only cat`.
```

We also will likely ignore `@only` annotations for tests, so that we can mock out
classes if desired.

One final note: abstract classes cannot be `@only` types for a variable, since they
are not functional without child classes overriding their abstract methods.

## template methods

You can define methods on your class that work for a variety of types.

```
someExample: {
    Value: int
    ;;renew(Int): null
        My Value = Int
    # in your own code, prefer adding `t new(SomeExample): t`
    # outside of this class body as the more idiomatic way
    # to convert `SomeExample` to a different type.
    ::to(): ~t
        return t(My Value)
}

SomeExample: someExample(5)

# you can use type inference here based on variable taking the return value:
ToString: string = SomeExample to()

# or you can explicitly ask like this:
To64: i64(SomeExample to())

# but you can't implicitly ask for the type.
Unspecified: SomeExample to()     # COMPILER ERROR, specify a type for `Unspecified`
```

## modifier pattern

In addition to getters/swappers (`::x(): int` and `;;x(X;. int): null`), another
standard pattern is modifying some internal data on the class.  Instead of recommending
a word like `modify`, we prefer using `;;[fn(ThingToModify;): ~t]: t` as
the method signature.  If there are multiple things you could modify in a class,
you can define multiple methods like this as long as they are distinguishable overloads.
(Note that arguments to the passed-in function can distinguish overloads, so
`;;[fn(ThingToModify;): ~t]: t` and `;;[fn(OtherThing;): ~t]: t` are distinguishable.)
With containers, values are keyed by some ID, so we do `;;[Id, fn(Value;): ~t]: t`
(or `hm[ok: t, uh]` as the return type in case of errors).  But if you have a class like a mutex
or a guard, where there is no ID needed to access the underlying data, you simply use e.g.
`Mutex[fn(Data;): Data someMethod()]`.

This is consistent with brackets `[]` being associated with container classes,
and giving access to the underlying data.

## generic/template classes

TODO: discuss how `null` can be used as a type in most places.  unless we
want to explicitly allow for it only if we use `{id?}` for example.
however that is pretty painful; `null` acts differently than `Null`,
so we should be able to do `hm[ok: null, uh: ...]` without `ok?: null`.
however, there is some inconsistency about how we're defining things:
```
myGeneric[of](Y: of, Z: of): of
    X: Y * Z
    X
```
e.g., if `of` is nullable, then `X` is potentially nullable, and should
be defined via `X?: Y * Z`.  but maybe we can avoid this by requiring non-null
in certain template declarations.

TODO: maybe support specifying mutability in templates, e.g., `gen[x; int]` 
for a `gen[x]: {MyValue: x}` would result in `{MyValue; int}`.  or if this
would be prone to errors/confusion with converting `:` into `;` where not desired,
disallow it in compiler, and support it another (more explicit) way.
maybe use `.` in the class definition.  but that might get confused with
function arguments.  we probably just want to use `gen[x: int, Access: Mutable]`,
and then do

```
myGeneric[of, Access: Mutable]: {
    if Access == Mutable
        X; of
        Y; of
    else
        X: of
        Y: of
}
```

To create a generic class, you put the expression `[types...]` after the
class identifier, or `[of]` for a single template type, where `of` is the
[default name for a generic type](#default-named-generic-types).  For example, we use
`mySingleGenericClass[of]: {...}` or `myMultiGenericClass[type1, type2]: {...}`
for single/multiple generics, respectively, to define the generic class.
When specifying the types of the generic class, we use
`mySingleGenericClass[int]` (for an `of`-defined generic class) or
`myMultiGenericClass[type1: int, type2: str]` (for a multi-type generic).
Note that any static/class methods defined on the class can still be accessed
like this: `mySingleGenericClass[int] myClassFunction(...)` or
`myMultiGenericClass[type1: int, type2: str] otherClassFunction()`.

```
# create a class with two generic types, `id` and `value`:
genericClass[id, value]: {
    ;;renew(My Id: id, My Value: value): null
}
# also equivalent:
genericClass: {Id: ~id, Value: ~value}
# even more concise:
genericClass: {~Id, ~Value}

# creating an instance using type inference:
ClassInstance: genericClass(Id: 5, Value: "hello")
 
# creating an instance with template/generic types specified:
OtherInstance: genericClass[id: dbl, value: string](Id: 3, Value: "4")
```

You can also have virtual generic methods on generic classes, which is not allowed by C++.

```
generic[of]: {
    Value; of

    # not a `@final` method, so this can be extended/overridden:
    # TODO: maybe switch to final as `:;method(): int` and virtual as `:;method(); int`
    ::method(~U): u
        OtherOf: Of = My Value * (U + 5)
        U + u(OtherOf) orPanic()
}

Generic; generic[string]
Generic Value = "3"
print(Generic method(i32(2)))    # prints "3333335" which is i32("3" * (2 + 5)) + 2

specific[of]: generic[of] {
    ;;renew(My Scale; of = 1): Null

    ::method(~U): u
        ParentResult: generic[of]::method(U)
        return ParentResult * My Scale 
}
```

Since we switch methods to global functions, we're probably ok.
E.g., `::method(U: ~u): u` becomes `globalMethod(Generic: generic[of], U: ~u): u`
becomes `template <class t, class u> u globalMethod(readonlyRef<generic<t>> Generic, readonlyRef<u> U)`
TODO: actually probably not ok, since we need to tell if generic is a child class before proceeding
with its implementation.  maybe we need to do a switch-case on the actual instance being held
inside the `Generic` type, and look up that type's instance's method.  e.g., define also
`template <class t, class u> u globalMethod(readonlyRef<specific<t>> Specific, readonlyRef<u> U)`,
then inside `globalMethod(...<generic<t>>...)` we ensure to call the correct overload.  we'll need to do this in
such a way that we don't need to know all the child class definitions when we write the parent
class definition; e.g., build it into the type's vtable.  if it's null, just use the parent definition.

Just like with function arguments, we can elide a generic field value if the
field name is already a type name in the current scope.  For example:

```
MyNamespace id: int
value: {X: flt, Y: flt}
MyStore2; store[MyNamespace id, value]
# Equivalent to `MyStore1; store[id: MyNamespace id, value: value]`.
```

### generic type constraints

To constrain a generic type, use `[type: constraints, ...]`.  In this expression,
`constraints` is simply another type like `nonNull` or `number`, or even a combination
of classes like `union[container[id, value], number]`.  It may be recommended for more
complicated type constraints to define the constraints like this:
`myComplicatedConstraintType: allOf[t1, oneOf[t2, t3]]` and declaring the class as
`newGeneric~[of: myComplicatedConstraintType]`, which might be a more readable way to do
if `myComplicatedConstraintType` is a helpful name.

### generic type defaults

Type defaults follow the same pattern as type constraints but the default types are
not abstract.  So we use `[type: defaultType, ...]` where `defaultType` is a class
that is non-abstract.

### overloading generic types

Note that we can overload generic types for single types and stored types (e.g.,
`array[int]` vs. `array[Count: 3, int]`), which is especially helpful for creating
your own `hm` result class based on the stored type `hm[uh, ok]` which can become
`SomeNamespace uh: oneOf[Oops, MyBad], hm[of]: hm[ok: of, SomeNamespace uh]`.
Here are some examples:

```
# Note that in hm-lang we could define this as `pair[First of, Second of]`
# so we don't need to specify `first: int, second: dbl`, but for illustration
# in the following examples we'll make the generic parameters named.
pair[first, second]: {First, Second}
pair[of]: pair[first: of, second: of]

# examples using pair[of]: ======
# an array of pairs:
PairArray[]: pair[int] = [{First: 1, Second: 2}, {First: 3, Second: 4}]
# this declaration is equivalent: `PairArray: pair[int][]`.

# a pair of arrays:
PairOfArrays: pair[int[]] = {First: [1, 2], Second: [3, 4]}

# a store of pairs:
PairStore[str]: pair[int] = ["hi there": {First: 1, Second: 2}]
# this declaration is equivalent: `PairStore: pair[int][str]`

# a pair of stores:
PairOfStores: pair[int[str]] = {First: ["hello": 1], Second: ["world": 2]}A

# examples using pair[first, second]: ======
# an array of pairs:
PairArray[]: pair[first: int, second: dbl] = [
    {First: 1, Second: 2.3}
    {First: 100, Second: 0.5}
]
# note that this declaration is equivalent:
# `PairArray: pair[first: int, second: dbl][]`

# a pair of arrays:
PairOfArrays: pair[first[]: int, second[]: dbl] = {
    First: [1, 2]
    Second: [3.4, 4.5]
}
# note this declaration is equivalent:
# `PairOfArrays: pair[first: int[], second: dbl[]]`

# a store of pairs:
PairStore[str]: pair[first: int, second: dbl] = [
    "hi there": {First: 1, Second: 2.3}
]
# note that this declaration is equivalent:
# `PairStore: pair[first: int, second: dbl][str]`

# a pair of stores:
PairOfStores: pair[first[str]: int, second[str]: dbl] = [
    "hi there": {First: 1, Second: 2.3}
]
# note this declaration is equivalent:
# `StoreOfPairs: pair[first: int[str], second: dbl[str]]`
```

### default named generic types

The default name for a type is `of`, mostly to avoid conflicts with
`type` which is a valid verb (e.g., to type in characters), but also
to reduce the letter count for generic class types.  Default names
are useful for generics with a single type requirement, and can be
used for overloads, e.g.:

```
aClass[x, y, N: count]: array[{X, Y}, Count: N]

aClass[of]: aClass[x: of, y: of, N: 100]
```

Similar to default-named arguments in functions, default-named generics
allow you to specify the generic without directly using the type name.
For example:

```
# use the default-name `type` here:
aClass[of, N: count]: aClass[x: of, y: of, N]

# so that we can do this:
AnInstance: aClass[dbl, N: 3]
# equivalent but not idiomatic: `AnInstance: aClass[of: dbl, N: 3]`.
```

Similar to default-named arguments in functions, there are restrictions.
You are not able to create multiple default-named types in your generic
signature, e.g., `myGeneric[A of, B of]`, unless we use `First` and
`Second` namespaces, e.g., `myGeneric[First of, Second of]`.  These
should only be used in cases where order intuitively matters.

### generic overloads must use the original class or a descendant

To avoid potential confusion, overloading a generic type must use
the original class or a descendant of the original class for any
overloads.  Some examples:

```
someClass[x, y, N: count]: { ... }

# this is OK:
someClass[of, N: count]: someClass[x: of, y: of, N]

# this is also OK:
childClass[of]: someClass[x: of, y: of, N: 256] {
    # additional child methods
    ...
}
someClass[of]: childClass[of]

# this is NOT OK:
someClass[t, u, v]: { ...some totally different class... }
```

### type tuples

One can conceive of a tuple type like `[x, y, z]` for nested types `x`, `y`, `z`.
Note that these are *not* order dependent, so `[z, x, y]` is the same tuple type,
so they are grammatically equivalent to a `store` of types, and their use is
make it easy to specify types for a generic class.  This must be done using the
spread operator `...` in the following manner.

```
tupleType: [x, y, z]

# with some other definition `myGeneric[w, x, y, z]: {...}`:
someSpecification: myGeneric[...tupleType, w: int]

# you can even override one of your supplied tupleType values with your own.
# make sure the override comes last.
anotherSpec[Override of]: myGeneric[...tupleType, w: str, x: Override of]

# Note that even if `tupleType` completely specifies a generic class
# `someGeneric[x, y, z]: {...}`, we still need to use the spread operator
# because `someGeneric tupleType` would not be the correct syntax.  Instead:
aSpecification: someGeneric[...tupleType]
```

Tuple types cannot be used to return multiple values from a function.  Instead,
use `{}` with the requisite fields, e.g., `{X, Y, Z}` for types `x`, `y`, and `z`.
Named fields also increase readability, so e.g. `{LikelyIndex: index, ExpectedCount: count}`
can improve others' understanding of your code.

You could theoretically use tuple types to return class constructors from a function.
This is only really useful if there's some conditions to choose one type over another,
so we can return constrained types as `[constraint1, ...]` or any type as `[any, ...]`.
Of course they can be named as well, like `[namedType: constraint, ...]`.
Here is an example with usage.

```
tuple(Dbl): [number, vector: any]
    if abs(Dbl) < 128.0
        [number: flt, vector: {X: flt, Y: flt}]
    else
        [number: dbl, vector: {X: dbl, Y: dbl}]

myTuples: tuple(random() * 256.0)
MyNumber; myTuples number(5.0)
MyVector; myTuples vector(X: 3.0, Y: 4.0)
```

If you want to return a single constructor, use the [`new[any]` syntax](#returning-a-type-constructor).


### default field names with generics

Note that generic classes like `generic[of]: {Of}` will always have a field named `Of`
regardless of the specified type.  We don't make this like a generic name; `generic[int]`
would *not* be equivalent to `{Int: int}`; `generic[int]` is `{Of: int}`.  This is mostly
to avoid confusion when passing in two types that are the same like `store[id: int, value: int]`.
Internally, if we store a list of `{Id, Value}` objects, there could be a name collision
(e.g., `{Int, Int}`), and we don't want to expose that internal detail to consumers of
the generic class.

This is one minor inconsistency with argument name generics for functions, but exposing
internal details isn't desired at all.

## common class methods

All classes have a few compiler-provided methods which cannot be overridden.

* `Me;!: me` creates a temporary with the current instance's values, while
    resetting the current instance to a default instance -- i.e., calling `renew()`.
    Internally, this swaps pointers, but not actual data, so this method
    should be faster than copy for types bigger than the processor's word size.
* `..map(fn(Me.): ~t): t` to easily convert types or otherwise transform
    the data held in `Me`.  This method consumes `Me`.  You can also overload
    `map` to define other useful transformations on your class.
* `::map(fn(Me): ~t): t` is similar to `..map(fn(Me.): ~t): t`,
    but this method keeps `Me` constant (readonly).  You can overload as well.
* `i(...): me` class constructors for any `;;renew(...): null` methods.
* `i(...): hm[ok: me, uh]` class or error constructors for any methods defined as
    `;;renew(...): hm[ok: i, uh]`
* `;;renew(...): null` for any `i(...): me` class constructors.
    This allows any writable variable to reset without doing `X = x(...)`,
    which may be painful for long type names `x`, and instead do `X renew(...)`.
* `;;renew(...): hm[uh]` for any `i(...): hm[ok: me, uh]` construct-or-error class functions
    This allows any writable variable to reset without doing `X = x(...) assert()`,
    which may be painful for long type names `x`, and instead do `X renew(...) assert()`.

## singletons

Defining a singleton class is quite easy, simply by instantiating a class when
you define it with trailing `()` (which may include arguments you need to instantiate):

```
AwesomeService: allOf[parentClass1, parentClass2, #(etc.)#] {
    UrlBase: "http://my/website/address.bazinga"
    ::get(Id: string): awesomeData 
        Json: Http get("$(My UrlBase)/awesome/$(Id)") 
        return awesomeData(Json)
}()
```

Using `@singleton lowerCamelCase` on the LHS defines an abstract singleton.
These are useful when you want to be able to grab an instance of the concrete
child-class but only through the parent class reference.

```
### screen.hm ###
@singleton
screen: {
    ;;draw(Image, Vector2): null
    ;;clear(Color: color Black)
}
### implementation/sdl-screen.hm ###
# TODO: we probably can convert `\/../screen screen` -> `\/../screen`
#       where we're requesting the class name of a file that's named correctly.
SdlScreen: \/../screen screen {
    ;;draw(Image, Vector2): null
        # actual implementation code:
        My SdlSurface draw(Image, Vector2)

    ;;clear(Color: color Black)
        My SdlSurface clear(Color)
}()
### some-other-file.hm ###
# this is an error if we haven't imported the sdl-screen file somewhere:
Screen; screen
Screen clear color(R: 50, G: 0, B: 100)
```

You get a run-time error if multiple child-class singletons are imported/instantiated
at the same time.

## sequence building

Some languages use a builder pattern, e.g., Java, where you add fields to an object
using setters.  For example, `MyBuilder.setX(123).setY(456).setZ("great").build()` [Java].
In hm-lang, this is mostly obviated by named arguments: `myClass(X: 123, Y: 456, Z: "great")`
will do the same thing.  However, there are still situations where it's useful to chain 
methods on the same class instance, and hm-lang lacks the ability to return a reference to
the class instance from a method like in other languages.  Instead, we use `{}` with all the
method calls inside.  For example, if we were to implement a builder pattern with setters,
we could combine a bunch of mutations like this:

```
myBuilder: {
    ;;set(String, Int): null
}

# Note, inside the `{}` we allow mutating methods because `myBuilder()` is a temporary.
# The resulting variable will be readonly after this definition + mutation chain,
# due to being defined with `:`.
MyBuilder: myBuilder() {
    set("Abc", 123)
    set("Lmn", 456)
    set("Xyz", 789)
    # etc.
}

# You can also do inline, but you should use commas here.
# Note that this variable can be mutated after this line due to `;`.
MyBuilder2; myBuilder() { set("Def", 987), set("Uvw", 321) }
```

By default, if the left-hand side of the sequence builder is writable (readonly),
the methods being called on the right will be the writable (readonly) versions.
E.g., `myBuilder()` is the left-hand side for the sequence builder, and `{ set... }`
is the right; and in this case, `myBuilder()` is a temporary which defaults to
writable.  You can explicitly ask for the readonly (or writable) version of a
method using `::` (or `;;`) like this, although it will be a compile-error if
you are trying to write a readonly variable.

The return value of the sequence builder also depends on the LHS.
If it is a temporary, the return value will be the temporary after it has been called
with all the methods in the RHS of the sequence builder.  E.g., from the above example,
a `myBuilder` instance with all the `set` methods called.  Otherwise, if the LHS
is a reference (either readonly or writable), the return value of the sequence
builder will be an object with all the fields built out of the RHS methods.
Some examples of the LHS being a reference follow:

```
ReadonlyArray: [0, 100, 20, 30000, 4000]
Results: ReadonlyArray {
    [2]             # returns 20
    ::sort()        # returns a sorted copy of the array; `::` is unnecessary
    ::print()       # prints unsorted array; `::` is unnecessary
    # this will throw a compile-error, but we'll discuss results
    # as if this wasn't here.
    ;;[3, ++$Int]   # compile error, `ReadonlyArray` is readonly
}
# should print [0, 100, 20, 30000, 4000]
# Results = {Int: 20, Sort: [0, 20, 100, 4000, 30000]}

WriteableArray; [0, 100, 20, 30000, 4000]
Results: WriteableArray {
    [2]             # returns 20
    sort()          # in-place sort, i.e., `;;sort()`
    ;;[3, ++$Int]   # OK, a bit verbose since `;;` is unnecessary
    # prints the array after all the above modifications:
    ::print()       # OK, we probably don't have a `;;print()` but you never know
}
# should print [0, 20, 100, 4001, 30000]
# Results = {Int: 20}
```

### nested sequence builders

In fact, `{}` acts somewhat like bash sequence builders (which also use `{}`).  E.g.,

```
# Example method sequence builder:
MyClass {
    myMethod() {nextMethod(), nextMethod2(), NestedField}
    otherMethod()
    SomeField
}

# Is equivalent to this sequence:
Result: MyClass myMethod()
NextMethod: Result nextMethod()
NextMethod2: Result nextMethod2()
NestedField: Result NestedField
OtherMethod: MyClass otherMethod()
SomeField: MyClass SomeField
# This is constructed only if necessary (e.g., for a return value).
{MyMethod: {NextMethod, NextMethod2, NestedField}, OtherMethod, SomeField}
```

When two sequence builders combine, e.g., `{A, B} {c(), d()}`, they execute in a deterministic
order, e.g., `A c(), A d(), B c(), B d()`.  In this case, when the LHS is itself a sequence,
it acts like a temporary, so that the return value is an object with fields `A` and `B`,
i.e., `{A, B}`, where `A` and `B` have already had the methods `c()` and `d()` called on them.

You can use field names in sequence builders in order to better organize things.
This is only useful if the LHS is not a temporary, since a temporary LHS is returned
as sequence builder's value.

```
MyClass: {...}
# MyClass is not a temporary, so we can include field names here:
Results: MyClass {
    Field1: myMethod()
    Field2: nextMethod()
}
# The above is equivalent to the following:
Results: {
    Field1: MyClass myMethod()
    Field2: MyClass nextMethod()
}

# This is a compile error because the LHS of the sequence builder
# is a temporary, so the fields are not used in the return value.
Results: MyClass getValue() {
    Field1: doSomething()
    Field2: doSomethingElse()
} # COMPILE ERROR
```

### conditionals in sequence builders

TODO: talk about conditionals in sequence building.
E.g., `{ myMethod(), if Value $(someMethod()) else $(otherMethod()) }`
Everything is scoped to the LHS, however, so `if Value` would be `if Lhs Value`.

### class definitions aren't sequence builders

Classes look like a type of sequence builder, but for consistency with
other parts of the grammar, we don't actually build sequences here.
The issue comes for parent/child classes, where we use the notation
`childClass: parentClass {...child methods...}`.  This makes it clear that
`childClass` is a type of `parentClass` but with additional methods.
However, this logic would not make sense from a sequence building perspective;
if `parentClass` expands to `{...parent methods...}` and then we'd have
combinatorical proportions combining all parent methods with child methods
which don't make any sense.  If instead we prefix each child method with
`parentClass` and think about each variable/method as being a new way
to approach `parentClass` data but in the namespace of `childClass`,
it kinda works:

```
parentClass := {X: dbl}
childClass: parentClass {Y: dbl, ::length(): (X**2 + Y**2)**0.5}
# If we literally do sequence building, we'd have
childClass: {parentClass Y: dbl, parentClass::length(): (X**2 + Y**2)**0.5}
# and this kinda makes sense but it's not super helpful to think about,
# because we're not allowed to add new instance variables `Y` to a parent class
# outside of its primary definition.
```

So overall, it's not a helpful picture to think about classes as sequence
builders.  You can use sequence building outside of a class definition to
add new methods and class variables/functions (not instance variables/functions).

# aliases

Aliases enable writing out logic with semantically similar descriptions, and 
are useful for gently adjusting programmer expectations.  The hm-lang formatter will
substitute the preferred name for any aliases found, and the compiler will only
warn on finding aliases.

Aliases can be used for simple naming conventions, e.g.:

```
options: anyOrNoneOf[
    oneOf[AlignInheritX: 0, AlignCenterX, AlignLeft, AlignRight]
] {
    @alias InheritAlignX: AlignInheritX
}

Options: options InheritAlignX    # converts to `options AlignInheritX` on next format.
```

Aliases can also be used for more complicated logic and even deprecating code.

```
myClass: {
    # TODO: we probably want to support `My` working here as well:
    # explicit constructor:
    i(My X; int): i()

    # implicit constructor:
    ;;renew(My X; int): null

    # This was here before...
    # ;;myDeprecatedMethod(DeltaX: int): null
    #     My X += DeltaX

    # But we're preferring direct access now:
    @alias ;;myDeprecatedMethod(DeltaX: int): null
        My X += DeltaX
}

MyClass; myClass(X: 4)
MyClass myDeprecatedMethod(DeltaX: 3)   # converts to `MyClass X += 3` on next format.
```

While it is possible, it is not recommended to use aliases to inline code.
This is because the aliased code will be "inlined" in the source directly,
so it won't benefit from any future updates to the aliased code.

# modules

Every file in hm-lang is its own module, and we make it easy to reference
code from other files to build applications.  All `.hm` files must be known
at compile time, so referring to other files gets its own special notation.
The operator `\/` begins a file-system search in the current directory.
and two backslashes becomes a search for a library module, e.g., `\\math`.

Subsequent subdirectories are separated using forward slashes, e.g.,
`\/relative/path/to/file` to reference the file at `./relative/path/to/file.hm`,
and `..` is allowed between forward slashes to go to the parent directory relative
to the current directory, e.g., `\/../subdirectory_in_parent_directory/other/file`.
Note that we don't include the `.hm` extension on the final file, and we allow
underscores in file and directory names.
TODO: we can probably alias `.hm` imports into their non-`.hm` version.

For example, suppose we have two files, `vector2.hm` and `main.hm` in the same
directory.  Each of these is considered a module, and we can use backslashes
to invoke logic from these external files.

```
# vector2.hm
vector2: {
    ;;renew(My X: dbl, My Y: dbl): null

    @orderIndependent
    ::dot(Vector2: vector2): My X * Vector2 X + My Y * Vector2 Y
}

# main.hm
Vector2Module: hm = \/vector2    # .hm extension must be left off.
Vector2: Vector2Module vector2(X: 3, Y: 4)
print(Vector2)
# you can also destructure imports like this:
{vector2}: \/vector2
```

TODO: discussion on how the formatter will move imports to the bottom
of the file so you can see the main part of your code instantly.

TODO: how to import functions, i.e., to distinguish from classes?
e.g., is `{vector2}: \/vector2` a function or a class definition?
we probably want to force importing the overload to ensure that we can
determine if it's a function or a class in the importing file.  e.g.,
`{myFunction(MyArg1: int, MyArg2: dbl): str} = \/util`.
we can always come up with a solution to "grab all overloads" like
`{myFunction: ...(...)} = \/util`. -- TODO something better.
or `{myFunction(_): _} = \/util` or `{myFunction(*): *} = \/util`.

You can use this `\/` notation inline as well, which is recommended
for avoiding unnecessary imports.  It will be a language feature to
parse all imports when compiling a file, regardless of whether they're used,
rather than to dynamically load modules.  This ensures that if another imported file
has compile-time errors they will be known at compile time, not run time.

```
# importing a function from a file in a relative path:
print(\/path/to/relative/file functionFromFile("hello, world!"))

# importing a function from the math library:
Angle: \\math atan2(X: 5, Y: -3)
```

To import a path that has special characters, just use the special characters
inline after the `\/`, e.g., `\/sehr/übel` to reference the file at `./sehr/übel.hm`.
For a path that has spaces (e.g., in file or directory names), use parentheses to
surround the path, e.g., `\\[library/path/with spaces]` for a library path or 
`\/(relative/path/with a space/to/a/great file)` for a relative path.  Or you can
use a backslash to escape the space, e.g., `\\library/path/with\ spaces` or
`\/relative/path/with\ a\ space/to/a/great\ file`.  Other standard escape sequences
(using backslashes) will probably be supported.

Note that we take the entire import as
if it were an UpperCamelCase identifier.  E.g., `\\math` acts like one identifier, `Math`,
so `\\math atan(X, Y)` resolves like `Math atan(X, Y)`, i.e., member access or drilling down
from `Math: \\math`.  Similarly for any relative import; `\/relative/import/file someFunction(Q)`
correctly becomes like `File someFunction(Q)` for `File: \/relative/import/file`.

## scripts

While it's generally nice to compile your code for performance, there are times where
it doesn't make sense to optimize for performance at the cost of compile times.  For example,
when prototyping movement in a game, it's useful to get a feel for what your code is doing
by trying out many things.  For this reason, it's important that hm-lang offers an interpreter
for your scripts, so that you can iterate quickly without always waiting for your program to compile.

In order to reduce compile times, you can define scripts to be interpreted in your main binary
using files with an `.hms` extension.  After you are satisfied with the script, you can promote
it to compiled code by converting the `.hms` file to a `.hm` file.

Note that one downside of scripting is that what could be compile-time errors become runtime errors.

TODO: figure out how we import hm scripts (`.hms` files).  Maybe something like
`Script: import("../my_scripts/doom.hms"), Script doom()`.
TODO: how are we actually going to do this, e.g., need to expose public/protected functions to
the calling code, pulling in other import dependencies should not reload code if we've already loaded
those dependencies in other compiled files, etc.

## tests

Unit tests should be written inside the file that they are testing.  Files should generally be less than
1000 lines of code, including tests, but this is not strictly enforced.  Because unit tests live inside
the files where the code is defined, tests can access private functions for testing.  It is generally
recommended to test the public API exhaustively, however, so private function testing should be redundant.
Tests are written as indented blocks with a `@test` annotation.

```
@private
privateFunction(X: int, Y: int): {Z: str}
    "$(X):$(Y)"

@protected
protectedFunction(X: int, Y: int): {Z: str}
    {Z;} = privateFunction(X, Y)
    Z += "!"
    {Z}

publicFunction(X1: int, Y1: int, X2: int, Y2: int): null
    print(protectedFunction(X: X1, Y: Y1) Z, privateFunction(X: X2, Y: Y2))

@test("foundation works fine")
    assert privateFunction(X: 5, Y: 3) == {Z: "5:3"}
    assert privateFunction(X: -2, Y: -7) == {Z: "-2:-7"}

@test("building blocks work fine")
    assert protectedFunction(X: 5, Y: -3) == {Z: "5:-3!"}
    assert protectedFunction(X: -2, Y: 7) == {Z: "-2:7!"}

@test("public function works correctly")
    publicFunction(X1: -5, Y1: 3, X2: 2, Y2: 7)
    assert Test print() == "-5:3!2:7" + \\os NewLine

    @test("nested tests also work")
        publicFunction(X1: 2, Y1: -7, X2: -5, Y2: -3)
        assert Test print() == "2:-7!-5:-3" + \\os NewLine
```

Inside of a `@test` function, you have access to a `Test` variable which includes
things like what has been printed (`Test print()`).  In this example, `Test print()`
will pull everything that would have been printed in the test and puts it into a string
for comparisons and matching.  It then clears its internal state so that new calls
to `Test print()` will only see new things since the last time `Test print()` was called.

Integration tests can be written in files that end with `.test.hm` or `.test.hms` (i.e., as a script).
These can pull in any dependencies via standard file/module imports, including other test files.
E.g., if you create some test helper functions in `helper.test.hm`, you can import these
into other test files (but not non-test files) for usage.

In debug mode (the default), unit tests are always run before a program runs its non-test code
(i.e., whenever `hm run` is invoked), and integration tests (and unit tests) are run via `hm test`.
In hardened or optimized modes, unit tests can be invoked via `hm test -h` (hardened, also `--hardened`
works instead of `-h`) or `hm test -o` (optimized, also `--optimized` works instead of `-o`).
If you are in a subdirectory of your main project, `hm test` will only run tests in that directory
and any subdirectories.

## file access / file system

TODO: how does file access work with the reference pattern

TODO: make it possible to mock out file system access in unit tests.

# errors and asserts

hm-lang borrows from Rust the idea that errors shouldn't be thrown, they should be
returned and handled explicitly.  We use the notation `hm[ok, uh]` to indicate
a generic return type that might be `ok` or it might be an error (`uh`).
In practice, you'll often specify the generic arguments like this:
`hm[ok: int, uh: string]` for a result that might be ok (as an integer) or it might
be an error string.

To make it easy to handle errors being returned from other functions, hm-lang uses
the `assert` method on a result class.  E.g., `Ok: MyHm assert()` which will convert
the `MyHm` result into the `ok` value or it will return the `uh` error in `MyHm` from
the current function block, e.g., `Ok: what MyHm $( Ok: $(Ok), Uh: $(return Uh))`.
It is something of a macro like `?` in Rust.  Note that `assert` doesn't panic.
There are a few helpful overloads for the `assert()` method, including changing the
error type `uh` by including it, e.g., `MyHm assert(Uh: newUhType("Bad"))`.

Note that we can automatically convert a result type into a nullable version
of the `ok` type, e.g., `hm[ok: string, uh: errorCode]` can be converted into
`string?` without issue, although as usual nulls must be made explicit with `?`.
E.g., `myFunction(StringArgument?: MyHm)` to pass in `MyHm` if it's ok or null if not,
and `String?: MyHm` to grab it as a local variable.

```
hm[ok, uh]: oneOf[ok, uh] {
    # The API is `Ok: Hm assert()`, which will bubble up this `uh`
    # if the result was an error.  Note that we use the `loop` API
    # which normally is implicit but can be used explicitly if needed.
    #   ```
    #   doSomething(Dbl): hm[ok: dbl, uh: str]
    #       if Dbl < 0
    #           uh("Invalid Dbl: $(Dbl)")
    #       else
    #           ok(Dbl sqrt())
    #
    #   # example with implicit `block` (like a guard/control flow):
    #   implicitBlock(): hm[ok: null, uh: str]
    #       # will return early if an invalid type.
    #       Result: doSomething(1.234) assert()
    #       print(Result)
    #
    #   # example with explicit `block` that does the same thing as implicit:
    #   explicitBlock(): hm[ok: null, uh: str]
    #       indent((Block: block[hm[ok: null, uh: str]]):
    #           Result: doSomething(1.234) assert(Block)
    #           print(Result)
    #           Block exit(null)
    #       )
    #   ```
    ..assert(Block: block[uh]): ok
        what Me
            Ok: $(Ok)
            Uh: $(debug error(Uh), Block exit(Uh))

    # The API is `Ok: Hm assert(Uh: "custom error if not `ok`")`.
    # This will moot `Me` and shortcircuit a failure (i.e., if `result`
    # is `uh`) to the calling block.  For example,
    #   ```
    #   doSomething(Dbl): hm[ok: dbl, uh: str]
    #       if Dbl < 0
    #           uh("Invalid Dbl: $(Dbl)")
    #       else
    #           ok(Dbl sqrt())
    #
    #   implicitBlock(): hm[ok: null, uh: oneOf[InvalidDoSomething, OtherError]]
    #       # will return early if an invalid type.
    #       Result: doSomething(1.234) assert(Uh: InvalidDoSomething)
    #       print(Result)
    #   ```
    ..assert(New Uh: ~woe, Block: block[woe]): ok
        what Me
            Ok: $(Ok)
            Uh:
                debug error(Uh)
                Block exit(New Uh)

    ..orPanic(String: ""): ok
        what Me
            Ok: $(Ok)
            Uh:
                error(Uh)
                # `panic` exits program
                panic(String || Uh)

    # If ok, returns the `Ok` value; otherwise returns the passed-in value.
    # If it is expensive to create the in-case-of-error `ok` value, use 
    # `Hm map((Uh.): myExpensiveOkConstruction())` to create it only as necessary.
    ..okOr(InCaseOfError Ok.): ok
        what Me
            Ok: $(Ok)
            Uh: $(InCaseOfError Ok)

    # maps an `Ok` result to a different type of `ok`, consuming `Me`.
    # we can also use namespaces like this to avoid renaming `ok` in the `hm[...]` specification:
    # ..map(fn(Ok.): A~ok): hm[A ok, uh].
    # TODO: in case `ok` and `uh` are the same type, we need to make sure that `fn(Ok): ok2`
    #       requires passing in by name (i.e., `fn(Ok. 123)`) so that we can distinguish the overloads
    #       for this versus `map(fn(Uh.): ~x): ~y`
    ..map(fn(Ok.): ~ok2): hm[ok: ok2, uh]

    # maps an `Ok` result to a different type of `ok`, with possibly an error, consuming `Me`.
    ..map(fn(Ok.): hm[A~ok, uh]): hm[A ok, uh]

    # passes through any `Ok` result, but maps an `Uh` to the desired `ok` result
    # via the passed-in function.
    ..map(fn(Uh.): ok): ok

    # maps an `Uh` result to a different type of `uh`.
    ..map(fn(Uh.): O~uh):  hm[ok, O uh]

    # maps `ok` and `uh` separately.
    ..map(fn(Ok.): t, fn(Uh.): t): t

    # TODO: should we use `to` here or is there a better way to indicate casting?
    # it's technically something like `oneOf[ok, null](Result: hm[ok, uh]): oneOf[ok, null]`
    # which is pretty verbose.  could use `(Me.): oneOf[ok, null]` for implicit conversion
    ..to(): oneOf[ok, null]
}

Result: if X $( ok(3) ) else $( uh("oh no") )
if Result isOk()
    print("ok")

# but it'd be nice to transform `Result` into the `Ok` value along the way.
Result is((Ok): print("Ok: ", Ok))
Result is((Uh): print("Uh: ", Uh))

# or if you're sure it's that thing, or want the program to terminate if not:
Ok: Result orPanic("for sure")
```

hm-lang tries to make errors easy, automatically creating subclasses of error for each module,
e.g., `store.hm` has a `store uh` error type.  Use `return myUhType("message $(HelpfulVariableToDebug)")`
to return a specific error type, or `return uh("message $(HelpfulVariableToDebug)")` to automatically
use the correct error subclass for whatever context you're in.  Note that you're not able to return a
module-specific error from another module (e.g., you can't return `store uh` from the `array.hm` module),
but you can match module-specific errors from another module (e.g., `what Uh $(store uh InvalidId $(...))
from the `array.hm` module or even your own).  Of course, you can throw/catch explicitly defined errors
from other modules, as long as they are visible to you (see section on
[public/protected/private](#public-private-protected-visibility)).

The built-in `assert` statement will shortcircuit the block if the rest of the statement
does not evolve to truthy.  As a bonus, when returning, all values will be logged to stderr
as well for debugging purposes for debug-compiled code.

```
assert(SomeVariable == ExpectedValue)   # throws if `SomeVariable != ExpectedValue`,
                                        # printing to stderr the values of both `SomeVariable`
                                        # and `ExpectedValue` if so.

assert(SomeClass method(100))       # throws if `SomeClass method(100)` is not truthy,
                                    # printing value of `SomeClass` and `SomeClass method(100)`.

assert(SomeClass otherMethod("hi") > 10)    # throws if `SomeClass otherMethod("hi") <= 10`,
                                            # printing value of `SomeClass` as well as
                                            # `SomeClass otherMethod("hi")`.
```

If you want to customize the return error for an assert, pass it an explicit
`Uh` argument, e.g., `assert(MyValue, Uh: "Was expecting that to be true")`;
and note that asserts can be called like `MyValue assert()` or `Positive assert(Uh: "oops")`.

Note that `assert` logic is always run, even in non-debug code.  To only check statements
in the debug binary, use `debug assert`, which has the same signature as `assert`.  Using
`debug assert` is not recommended, except to enforce the caller contract of private/protected
methods.  For public methods, `assert` should always be used to check arguments.  Note also
that `assert` will return the correct uh subclass for the module that it is in;
`debug assert` will return a `debug uh` to help indicate that it is not a production error.

## automatically converting errors to null

If a function returns a `hm` type, e.g., `myFunction(...): hm[ok, uh]`,
then we can automatically convert its return value into a `oneOf[ok, null]`, i.e.,
a nullable version of the `ok` type.  This is helpful for things like type casting;
instead of `MyInt: what int(MyDbl) $(Ok. $(Ok), Uh: $(-1))` you can do
`MyInt: int(MyDbl) ?? -1`.  Although, there is another less-verbose option that
doesn't use nulls:  `int(MyDbl) map((Uh): -1)`.

TODO: should this be valid if `ok` is already a nullable type?  e.g.,
`myFunction(): hm[ok: oneOf[int, null], uh: str]`.
we probably should compile-error-out on casting to `Int?: myFunction()` since
it's not clear whether `Int` is null due to an error or due to the return value.

# standard container classes (and helpers)

Brackets are used to create containers, e.g., `Y: "Y-Naught", Z: 10, [X: 3, (Y): 4, Z]`
to create a store with keys "X", the value of `Y` ("Y-Naught"), and "Z", with
corresponding values 3, 4, and the value of `Z` (10).  Thus any bracketed values,
as long as they are named, e.g., `A: 1, B: 2, C: 3, [A, B, C]`, can be converted
into a store.  Because containers are by default insertion-ordered, they can implicitly
be converted into an array depending on the type of the receiving variable.

```
# TODO: should this be `container uh` instead of `Container uh`?
#       may depend on how we handle static stuff.
Container uh: oneOf[
    OutOfMemory
    # etc.
]

hm[of]: hm[ok: of, Container uh]

# TODO: should we rename `id` to `name` or `lookup` or `at`?
# TODO: rename `nonNull` to `present` or `notNull`.  definitely can't mirror `unNull`
container[id, value: nonNull]: {
    # Returns `Null` if `Id` is not in this container,
    # otherwise the `value` instance at that `Id`.
    # This is wrapped in an argument object to enable passing by reference.
    # TODO: do we like this?  it looks a bit like SFO logic that we killed off.
    # USAGE:
    #   # Get the value at `Id: 5` and make a copy of it:
    #   Value?: Container[Id: 5]
    #   # Get the value at `Id: 7` and keep a mutable reference to it:
    #   (Value?;) = Container[Id
    :;[Id]: (Value?:;)

    # no-copy getter, which passes in a Null to the callback
    # if the container does not have the given `Id`.
    ::[Id, fn(Value?): ~t]: t

    # Returns the value at `Id`, if present, while mooting
    # it in the container.  This may remove the `id` from
    # the container or may set its linked value to the default.
    # (Which depends on the child container implementation.)
    # Returns Null if not present.
    ;;[Id]!?: value

    # safe setter.
    # returns an error if we ran out of memory trying to add the new value.
    ;;put(Id, Value.): hm[null]

    # safe swapper.  replaces the value at `Id` with the `Value` passed in,
    # and puts the previous value into `Value`.  the new or old value can
    # be null which means to delete what was there or that nothing was present.
    # returns an error if we ran out of memory trying to add the new value.
    ;;swap(Id, Value?;): hm[null]
    
    @alias ::has(Id): My[Id] != Null
    @alias ::contains(Id): My[Id] != Null

    # Returns the number of elements in this container.
    ::count(): count

    # can implicitly convert to an iterator (with writeable/readonly references).
    # the iterator needs to be returned in an argument object
    # because it holds a reference to `Me`.
    (Me;:): (Iterator[(Id, Value;:)].)

    # iterate over values.
    ;:values(): (Iterator[(Value;:)].)

    # iterate over IDs.
    ::ids(): (Iterator[(Id)].)

}
```

TODO: discuss the expected behavior of what happens if you delete an element
out of a container when iterating over it (not using the iterator itself, if
it supports delete).  for the default implementation, iterators will only
hold an index to an element in the container, and if that index no longer
indexes an element, we can stop iteration.

## arrays

An array contains a list of elements in contiguous memory.  You can
define an array explicitly using the notation `Array: array[elementType]` for the
type `elementType` or implicitly via `Array[]: elementType` or `Array: elementType[]`.
E.g. `MyArray: int[]` or `MyArray: array[int]` for a readonly integer array.
The writable versions of course use `;` instead of `:`.  Note that arrays
are ID'd by an `index` type, but using `MyVar[index]: elementType` would
create a [store](#stores).

To define an array quickly (i.e., without a type annotation), use the notation
`["hi", "hello", "hey"]`.
The default-named version of an array does not depend on the element type;
it is always `Array`.  Declared as a function argument, a default array of strings
would thus be, e.g., `myFunction(Array[string];:.): null`.
Example usage and declarations:

```
# this is a readonly array:
MyArray[]: dbl = [1.2, 3, 4.5]      # converts all to dbl
MyArray append(5)   # COMPILE ERROR: MyArray is readonly
MyArray[1] += 5     # COMPILE ERROR: MyArray is readonly

# writable integer array:
Array; int[]        # declaring a writable, default-named integer array
Array append(5)     # now Array == [5]
Array[3] += 30      # now Array == [5, 0, 0, 30]
Array[4] = 300      # now Array == [5, 0, 0, 30, 300]
Array[2] -= 5       # now Array == [5, 0, -5, 30, 300]

# writable string array:
StringArray[]; string = ["hi", "there"]
print(StringArray pop())    # prints "there".  now StringArray == ["hi"]
```

The default implementation of `array` might be internally a contiguous deque,
so that we can pop or insert into the beginning at O(1).  We might reserve
`stack` for a contiguous list that grows in one direction only.

```
Array uh: oneOf[
    OutOfMemory
    # etc...
]
hm[of]: hm[ok: of, Array uh]

# some relevant pieces of the class definition
# Note that `container[id, value]` must have a non-null `value` type,
# but `array` can have nullable entries if desired, so convert to `nonNull`
# if necessary to extend `container`.
# TODO: check notation: `nonNull(~t): if nullable(t) $(unNull(t)) else t`
# TODO: find a good way to infer types, e.g., like this (or maybe `@infer y` instead of `~y`):
# e.g., `nonNull[of]: if of == nullable(~y) $(y) else $(of)`.
# or maybe `if of == oneOf[~y, null] $(y) else $(of)`.
# or maybe `if of == oneOf[...y, null] $(y) else $(of)`.
array[of]: container[id: index, value: nonNull[of]] {
    # TODO: a lot of these methods need to return `hm[of]`.
    # cast to bool, `::!!(): bool` also works, notice the `!!` before the parentheses.
    !!(Me): bool
        return My count() > 0

    # Returns the value in the array if `Index < My count()`, otherwise Null.
    # If `Index < 0`, take from the end of the array, e.g., `Array[-1]` is the last element
    # and `Array[-2]` is the second-to-last element.  If `Index < -Array count()` then
    # we will also return Null.
    ::[Index]?: of

    # Gets the existing value at `Index` if the array count is larger than `Index`,
    # otherwise increases the size of the array with default values and returns the
    # one at `Index`.  This has a possibility of panicking because it requires an increase
    # in memory; if that's important to check, use `;;at(Index): hm[of]`.
    ;:[Index]: (Of;:)

    # Gets the existing value at `Index` or creates a default if needed.
    # Can return an error if we run out of memory because this method can
    # expand the array if `Index >= ::count()`.
    ;;at(Index, Put. of): hm[of]

    # Returns the value at Array[Index] while resetting it to the default value.
    # We don't bring down subsequent values, e.g., `Index+1` to `Index`, (1) for
    # performance, and (2) we need to preserve the invariant that `Array[Index]!`
    # followed by `!Array::[Index]` should be true, which would be impossible
    # to guarantee if we shifted all subsequent elements down.
    ;;[Index]!: of

    ::count(): count

    ;;append(Of): null

    # returns an error if no element at that index (e.g., array isn't big enough).
    ;;pop(Index: index = -1): hm[of]

    # returns a copy of this array, but sorted.
    # uses `you` instead of `me` to indicate that `me` doesn't change.
    ::sort(): you

    # sorts this array in place:
    ;;sort(): null
    ...

    # swapper, sets the value at the index, returning the old value in the reference.
    # if the swapped in value is Null but the array value wasn't Null, the array
    # will shrink by one, and any later indexed values will move down one index.
    # USAGE: `Of ?; = of(...), My[Index] <-> Of`
    # TODO: is there better notation here?, e.g., `;;[Index] <-> (Of?;): null` or something with `swap`?
    ;;[Index, Of?;]: null

    # TODO: use `[]` for the unsafe API, `all()` or `put()` for the safe API (returning a `hm`)

    # modifier, allows access to modify the internal value via reference.
    # passes the current value at the index into the passed-in function by reference (`;`).
    # if Index >= the current count(), then the array count is increased (to Index + 1)
    # and filled with default values so that a valid reference can be passed into the callback.
    # USAGE: `My[Index] += 5` compiles to `My[Index, (Of;): $(Of += 5)]`
    # TODO: we can probably determine `$Of += 5` as `(Of;): $(Of += 5)` because `$Of += 5` requires mutability
    ;;[Index, fn(Of;): ~u]: hm u

    # getter, which returns a Null if index is out of bounds of the array:
    ::[Index, fn(Of?): ~u]: u

    # getter, which never returns Null, but will return an `uh` if the index is out of bounds of the array:
    ::[Index, fn(Of): ~u]: hm u

    # Note: You can use the `;:` const template for function arguments.
    # e.g., `myArray[of]: array[of] { ;:[Index, fn(Of;:): ~u]: array;:[Index, fn] }`
    
    # nullable modifier, which returns a Null if index is out of bounds of the array.
    # if the reference to the value in the array (`Of?;`) is null, but you switch to
    # something non-null, the array will expand to that count (with default values
    # in between, if necessary).  if you set the reference to Null and it wasn't
    # Null before, then the array will shrink by one, and all later index values
    # will move down one.
    ;;[Index, fn(Of?;): ~u]: u

    # non-nullable modifier, which will increase the count of the array (with default values)
    # if you are asking for a value at an index out of bounds of the array.
    ;;[Index, fn(Of;): ~u]: u
}
```

### fixed-count arrays

We declare an array with a fixed number of elements using the notation
`elementType[Count]`, where `Count` is a constant integer expression (e.g., 5)
or a variable that can be converted to the `count` type.  Fixed-count array elements
will be initialized to the default value of the element type, e.g., 0 for number types.

Fixed-count arrays can be passed in without a copy to functions taking
an array as a readonly argument, but will be of course copied into a 
resizable array if the argument is writable.  Some examples:

```
# readonly array of count 4
Int4: int[4] = [-1, 5, 200, 3450]
# writable array of fixed-count 3:
Vector3; dbl[3] = [1.5, 2.4, 3.1]
print("Vector3 is {$(Vector3[0]), $(Vector3[1]), $(Vector3[2])}")

# a function with a writable argument:
doSomething(Array; dbl[]): dbl[2]
    # you wouldn't actually use a writable array argument, unless you did
    # some computations using the array as a workspace.
    # PRETENDING TO DO SOMETHING USEFUL WITH Array:
    return [Array pop(), Array pop()]

# a function with a readonly argument:
doSomething(Array: dbl[]): dbl[2]
    return [ConstArray[-1], ConstArray[-2]]

# COMPILER ERROR: `Vector3` can't be passed as mutable reference
# to a variable-sized array:
print(doSomething(Array; Vector3))    # prints [3.1, 2.4]

# OK: can bring in Vector3 by constant reference (i.e., no copy) here:
print(doSomething(Array: Vector3))     # prints [3.1, 2.4]
```

There may be optimizations if the fixed-array count is known at compile-time,
i.e., being defined on the stack rather than the heap.  But when the fixed
count is unknown at compile time, the fixed-count array will be defined on the heap:

```
# note you can use an input argument to define the return type's
# fixed-array count, which is something like a generic:
range(Count): int[Count]
    Result; int[Count]
    for I: index < Count
        Result[I] = I
    return Result

print(range(10))    # prints [0,1,2,3,4,5,6,7,8,9]
```

### possible implementation

In hm-lang:

```
fixedCountArray[of]: array[of] {
    @private FixedCount: count
    ;;renew(Count): null
        My FixedCount = Count
        My count(Count)
    @hide pop
    @hide add
    @hide erase
    @hide append
    @hide shift
    @hide reserve
    ;;count(Count): null
        assert Count == My FixedCount
        array;;count(Count)
}
```

## stores

A `store` is hm-lang's version of a map (or `dict` in python).  Instead of mapping from a `key`
to a `value` type, stores link an `id` to a `value`.  This change from convention is mostly
to avoid overloading the term `map` which is used when transforming values such as `hm`, but also
because `map`, `key`, and `value` have little to do with each other; we don't "unlock" anything
with a C++ `map`'s key: we look up a value.

TODO: we could rename `store` to `lookup`, e.g., `Lookup[5]` sounds good but
`Lookup[5] = 3` doesn't make a lot of sense.  maybe `keep` for `Keep[5] = 3`
or something else?

A store can look up, insert, and delete elements by key quickly (ideally amortized
at `O(1)` or at worst `O(lg(N)`).  You can use the explicit way to define a store, e.g.,
`VariableName: store[id: idType, value: valueType]`, or you can use an implicit method
with brackets, `VariableName[idType]: valueType` or `VariableName: valueType[idType]`.
For example, for a store that links integers to strings, you can use: `MyStore[int]: string`.
The default name for a store variable is `Store`, regardless of ID or value type.
Note that while an array can be thought of as a store linking the `index` type to
whatever the array element type is, `elementType[index]` indicates a store type,
not an array type.  The array type, `elementType[]` would be useful for densely
packed data (i.e., instances of `elementType` for most indices), while the store
type `elementType[index]` would be useful for sparse data.

To define a store (and its contents) inline, use this notation:

```
Jim1: "Jim C"
Jim2: "Jim D"
Jim: 456
# store linking string to ints:
EmployeeIds: int[str] = [
    # option 1.A: `X: Y` syntax
    "Jane": 123
    # option 1.B: `{Id: X, Value: Y}` syntax
    {Id: "Jane", Value: 123}
    # option 1.C: `[X, Y]` syntax, ok if ID and value types are different
    ["Jane", 123]
    # option 1.D:
    Jane: 123
    # if you have some variables to define your id, you need to take care (see WARNING below).
    # option 2.A, wrap in parentheses to indicate it's a variable not an ID
    (Jim1): 203
    # option 2.B
    {Id: Jim1, Value: 203}
    # option 2.C
    [Jim1, 203]
    # WARNING! not a good option for 2; no equivalent of option 1.D here.
    # Jim1: Jim1Id # WARNING, looks like option 1.C, which would define "Jim1" instead of "Jim C"
    # option 3: `X` syntax where `X` is a known variable, essentially equal to `@@X: X`
    Jim
]
# note that commas are optional if elements are separated by newlines,
# but required if elements are placed on the same line.

# equivalent definition would occur with this first line:
# `EmployeeIds: store[id: string, value: int] [`
```

To define a store quickly (i.e., without a type annotation), use the notation
`["Jane": 123, "Jim": 456]`.

Stores require an ID type whose instances can hash to an integer or string-like value.
E.g., `dbl` and `flt` cannot be used, nor can types which include those (e.g., `array dbl`).

```
DblDatabase; dbl[int]      # OK, int is an OK ID type
DblDblDatabase; dbl[dbl]   # COMPILE ERROR, dbl is an invalid ID type.
```

However, we allow casting from these prohibited types to allowed ID types.  For example:

```
NameDatabase; string[int]
NameDatabase[123] = "John"
NameDatabase[124] = "Jane"
print(NameDatabase[123.4])  # RUNTIME ERROR, 123.4 is not representable as an `int`.
print(NameDatabase[123.4 round(Stochastically)])    # prints "John" with 60% probability, "Jane" with 40%.

# note that the definition of the ID is a readonly array:
StackDatabase; string[int[]]
StackDatabase[[1,2,3]] = "stack123"
StackDatabase[[1,2,4]] = "stack124"
# prints "stack123" with 90% probability, "stack124" with 10%:
print(StackDatabase[map([1.0, 2.0, 3.1], $Dbl round(Stochastically))])
# things get more complicated, of course, if all array elements are non-integer.
# the array is cast to the ID type (integer array) first.
StackDatabase[[2.2, 3.5, 4.8] map($Dbl round(Stochastically))]
# result could be stored in [2, 3, 4], [2, 3, 5], [2, 4, 4], [2, 4, 5],
#                           [3, 3, 4], [3, 3, 5], [3, 4, 4], [3, 4, 5]
# but the ID is decided first, then the store is added to.
```

Note: when used as a store ID, objects with nested fields become deeply constant,
regardless of whether the internal fields were defined with `;` or `:`.
I.e., the object is defined as if with a `:`.  This is because we need ID
stability inside a store; we're not allowed to change the ID or it could
change places inside the store and/or collide with an existing ID.

Some relevant pieces of the class definition:

```
uh: oneOf[
    OutOfMemory
    MissingId
    # etc...
]
hm[of]: hm[ok: of, uh]

store[id: hashable, value: nonNull]: container[id, value] {
    # Returns Null if `Id` is not in the store.
    ::[Id]?: value

    # Gets the existing value at `Id` if present,
    # otherwise inserts a default `value` into the store and returns it.
    ;;[Id]: hm[value]

    # Ejects the possibly null value at `Id` and returns it.
    # A subsequent, immediate call to `::[Id]` returns Null.
    ;;[Id]!?: value

    ::count(): count

    # Returns the last element added to the store if the store is
    # insertion ordered, otherwise returns any convenient element.
    # The element is removed from the store.
    ;;pop(): hm[id, value]

    @alias ;;pop(Id)?: My[Id]!

    # always returns a non-null type, adding
    # a default-initialized value if necessary:
    # returns a copy of the value at ID, too.
    ;;[Id]: hm[value]

    # no-copy getter: which will create a default value instance if it's not present at Id.
    ;;[Id, fn(Value): ~t]: t

    # copy getter: which will return a copy of the value at ID;
    # returns a Null if ID is not in the store.
    ::[Id]?: value

    # no-copy getter: which will pass back a Null if the ID is not in the store.
    ::[Id, fn(Value?): ~t]: t

    # swapper: sets the value at the ID, returning the old value:
    ;;[Id, Value;]: value

    # modifier, allows access to modify the internal value via reference.
    # passes the current value at the ID into the passed-in function by reference (`;`).
    # the return value of the passed-in function will become the new value at the ID.
    # if a value at `Id` is not already present, a default `Value` will be created.
    ;;[Id, fn(Value;): ~t]: t

    # nullable modifier, which returns a Null if the ID is not in the store.
    # if the Value wasn't Null, but becomes Null via the passed-in function,
    # the ID will be deleted from the store.  conversely, if the value was Null, but
    # the passed-in function turns it into something non-null, the ID/value will be added
    # to the store.
    ;;[Id, fn(Value?;): ~t]: hm[ok: t, uh: OutOfMemory]

    # getter and modifier in one definition, with the `;:` "template mutability" operator:
    # will return an error for the const store (`My:`) if Id is not in the store.
    ;:[Id, fn(Value;:): ~t]: hm[t]

    # nullable getter/modifier in one definition, with the `;:` template mutability operator:
    ;:[Id, fn(Value?;:): ~t]: hm[ok: t, uh: OutOfMemory]
}
```

The default store type is `insertionOrderedStore`, which means that the order of elements
is preserved based on insertion; i.e., new IDs come after old IDs when iterating.
Other notable stores include `idOrderedStore`, which will iterate over elements in order
of their sorted IDs, and `unorderedStore`, which has an unpredictable iteration order.
Note that `idOrderedStore` has `O(lg(N))` complexity for look up, insert, and delete,
while `insertionOrderedStore` has some extra overhead but is `O(1)` for these operations,
like `unorderedStore`.

```
@private
indexedStoreElement[id, value]: {
    NextIndex; index
    PreviousIndex; index
    Id: id
    Value; value
}

insertionOrderedStore[id, value]: store[id, value] {
    # due to sequence building, we can use @private {...} to set @private for
    # each of the fields inside this block.
    @private {
        IdIndices; @only unorderedStore[id, value: index]
        IndexedStore; @only unorderedStore[
            id: index
            value: indexedStoreElement[id, value]
        ] = [{Id: 0, Value: {NextIndex: 0, id(), value(), PreviousIndex: 0}}]
        NextAvailableIndex; index = 1
    }

    # creates a default value if not present at the ID to pass in to the modifier:
    ;;[Id;:, fn(Value;): ~t]: t
        Index?: My IdIndices[Id]
        return if Index != Null
            My modifyAlreadyPresent(Index, fn)
        else
            My needToInsertThenModify(Id;:, fn)

    ::[Id, fn(Value?): ~t]: t
        Index?: My IdIndices[Id]
        return if Index != Null
            assert Index != 0
            My IndexedStore[Index, (IndexedStoreElement): t
                return fn(IndexedStoreElement Value)
            ]
        else
            fn(Null)
    
    ::forEach(Loop fn(Id, Value): loop): null
        Index; My IndexedStore[0] NextIndex
        while Index != 0
            {Value:, Id:} = My IndexedStore[Index] orPanic("broken invariant!")
            ForLoop: Loop fn(Id, Value)
            if ForLoop == loop Break
                break
            Index = My IndexedStore[Index] NextIndex
        # mostly equivalent to using nested functions to avoid copies:
        #   ForLoop: My IndexedStore[Index, (IndexedStoreElement?):
        #       if IndexedStoreElement == Null
        #           error("insertion-ordered store invariant was broken")
        #       Index = IndexedStoreElement NextIndex
        #       return Loop fn(IndexedStoreElement Id, IndexedStoreElement Value)
        #   ]

    # modifier for a ID'd value not yet in the store, need to insert a default first:
    @private
    ;;needToInsertThenModify(Id;:, fn(Value;): ~t): t
        NewIndex: My NextAvailableIndex++ or reshuffle()
        PreviouslyLastIndex: My IndexedStore[0] PreviousIndex
        My IndexedStore[NewIndex] = {
            PreviousIndex: PreviouslyLastIndex
            NextIndex: 0
            Id
            Value: value()
        }
        My IdIndices[@mootOrCopy(Id)] = NewIndex
        My IndexedStore[0] PreviousIndex = NewIndex
        My IndexedStore[PreviouslyLastIndex] NextIndex = NewIndex
        return My modifyAlreadyPresent(NewIndex, fn)

    # modifier for an already indexed value in the store:
    @private
    ;;modifyAlreadyPresent(Index, fn(Value;): ~t): t
        debug assert(Index != 0)
        return My IndexedStore[Index, (IndexedStoreElement?;): t
            assert IndexedStoreElement != Null
            return fn(IndexedStoreElement Value;)
        ]
}
```

## sets

A set contains some elements, and makes checking for the existence of an element within
fast, i.e., O(1).  Like with store IDs, the set's element type must satisfy certain properties
(e.g., integer/string-like).  The syntax to define a set is `VariableName: set elementType`
to be explicit, or we can use `VariableName[elementType]:` or `VariableName: [elementType]`.
The default-named variable name for a set of any type is `Set`.

```
uh: oneOf[
    OutOfMemory
    # etc...
]
hm[of]: hm[ok: of, uh]

set[of: hashable]: container[id: of, value: true] {
    # Returns `True` iff `Of` is in the set, otherwise Null.
    # NOTE: the `true` type is only satisfied by the instance `True`;
    # this is not a boolean return value but can easily be converted to boolean.
    ::[Of]?: true

    # TODO: use `[]` for the unsafe API, `all()` or `put()` for the safe API (returning a `hm`)

    # Adds `Of` to the set and returns `True` if
    # `Of` was already in the set, otherwise `Null`.
    # this can be an error in case of running out of memory.
    ;;[Of]: hm[true?]

    # Ejects `Of` if it was present in the set, returning `True` if true
    # and `Null` if not.
    # A subsequent, immediate call to `::[Of]?` returns Null.
    ;;[Of]!?: true 

    # Modifier for whether `Of` is in the set or not.
    # The current value is passed into the callback and can be modified;
    # if the value was `Null` and is converted to `True` inside the function,
    # then the set will get `Of` added to itself.  Example:
    #   `Set[X] = if Condition $(True) else $(Null)` becomes
    #   `Set[X, fn(Maybe True?;): $(Maybe True = if Condition $(True) else $(Null))]`
    # TODO: if we used `True?` as the identifier everywhere we wouldn't need to do `Maybe True`, e.g.,
    #   `Set[X, fn(True?;): $(True? = if Condition $(True) else $(Null))]`
    ;;[Of, fn(Maybe True?; true): ~t]: t

    # Fancy getter for whether `Of` is in the set or not.
    ::[Of, fn(Maybe True?): ~t]: t

    ::count(): count

    # Unions this set with values from an iterator, returning True if
    # all the values from the iterator were already in this set, otherwise False.
    # can error if running out of memory.
    ;;all(Iterator[of].): hm[bool]

    # can convert to an iterator.
    (Me): (Iterator[(Of)].)

    @hide ;:values(): (Iterator[(True;:)])
    @hide ;:ids(): (Iterator[(Of)])

    # Removes the last element added to the set if this set is
    # insertion ordered, otherwise any convenient element.
    # Returns an error if there is no element available.
    ;;pop(): hm[of]
    ;;pop(Of)?: My[Of]!

    @alias ;;remove(Of)?: My[Of]!
    ...
}
```

Like the IDs in stores, items added to a set become deeply constant,
even if the set variable is writable.

TODO: discussion on `insertionOrderedSet` and `unorderedSet`, if we want them.

To define a set quickly, use the notation `[str]["hello", "world"]`, where the
initial `[str]` should be the type of whatever element is in the set.

TODO: make it easy to pass in a set as an argument and return a store with e.g. those IDs.
  maybe this isn't as important as it would be if we had a dedicated object type.

```
fn(Fields: [~k], PickFrom: ~t[~q extends k]): t[k]
    return Fields map((Field: str): storeElement(Field, PickFrom[Field]))

fn(PickFrom: ~o, Ids: ~k from ids(o)): pick(o, k)
    return pick(PickFrom, Ids)
```

TODO: discuss how `in` sounds like just one key from the set of IDs (e.g., `k in ids(o)`)
and `from` selects multiple (or no) IDs from the set (`k from ids(o)`).

## iterator

For example, here is a way to create an iterator over some incrementing values:

```
range[of: number]: iterator[of] {
    @private
    NextValue: of = 0

    ;;renew(StartAt: of = 0, My LessThan: of = 0): null
        My NextValue = StartAt

    ;;next()?: of
        if My NextValue < My LessThan
            return My NextValue++
        return Null

    ::peak()?: if My NextValue < My LessThan
        My NextValue 
    else
        Null
}

for Index: in range(LessThan: index(10))
    print(Index)
# prints "0" to "9"
```

We want to avoid pointers, so iterators should just be indices into
the container that work with the container to advance, peak, etc.
Thus, we need to call `Iterator next` with the container to retrieve
the element and advance the iterator, e.g.:

```
Array: [1,2,3]
Iterator; iterator[int]
assert Iterator next(Array) == 1
assert next(Array, Iterator;) == 2  # you can use global `next`
assert Iterator::peak(Array) == 3
assert peak(Iterator, Array) == 3   # or you can use global `peak`
assert Iterator next(Array) == 3
assert Iterator next(Array) == Null
assert Iterator peak(Array) == Null
# etc.
```

The way we achieve that is through using an array iterator:

```
# by requesting the `next()` value of an array with this generic iterator,
# the iterator will become an array iterator.  this allows us to check for
# `@only` annotations (e.g., if `Iterator` was not allowed to change) and
# throw a compile error.
next(Iterator; iterator[~t] @becomes(arrayIterator[t]), Array: array[t])?: t
    Iterator = arrayIterator[t]()
    Iterator;;next(Array)

arrayIterator[of]: iterator[of] {
    Next; index
    ;;renew(Start: index = 0):
        My Next = Start

    ;;next(Array: array[of])?: if My Next < Array count()
        Array[My Next++]
    else
        Null

    ::peak(Array: array[of])?: if My Next < Array count()
        Array[My Next]
    else
        Null
    
    # note that this function doesn't technically need to modify this
    # `arrayIterator`, but we keep it as `;;` since other container
    # iterators will generally need to update their index/ID.
    ;;remove(Array; array[of])?: if My Next < Array count()
        Array remove(This Next)
    else
        Null
}
```

We can also directly define iterators on the container itself.
We shouldn't need to define both the `iterator` version and the `forEach` version;
we should be able to translate one into the other.
TODO: think of a good mechanism for this.

```
array[of]: {
    # const iteration, with no-copy if possible:
    ::forEach(fn(Of): loop): null
        for Index: index < My count()
            # use the no-copy getter, here:
            # explicit:
            ForLoop: My[Index, fn]
            # implicit:
            ForLoop: fn(My[Index];)
            if ForLoop == loop Break
                break

    # no-copy iteration, but can mutate the array.
    ;;forEach(fn(Of;): loop): null
        for Index: index < My count()
            # do a swap on the value based on the passed in function:
            # explicit:
            ForLoop: My[Index, fn]
            # implicit:
            ForLoop: fn(My[Index];)
            if ForLoop == loop Break
                break

    # mutability template for both of the above:
    ;:forEach(fn(Of;:): loop): bool
        for Index: index < My count()
            if My[Index, fn] == Break
                return True
        return False
}
```

# standard flow constructs / flow control

We have a few standard control statements or flow control keywords in hm-lang.

TODO -- `return`
TODO -- description, plus `if/else/elif` section

## conditional expressions

Conditional statements including `if`, `elif`, `else`, as well as `what`,
can act as expressions and return values to the wider scope.  This obviates the need
for ternary operators (like `X = doSomething() if Condition else DefaultValue` in python
which inverts the control flow, or `int X = Condition ? doSomething() : DefaultValue;`
in C/C++ which takes up two symbols `?` and `:`).  In hm-lang, we borrow from Kotlin the idea that
[`if` is an expression](https://kotlinlang.org/docs/control-flow.html#if-expression)
and write something like:

```
X: if Condition
    doSomething()
else
    calculateSideEffects(...) # ignored for setting X
    DefaultValue

# now X is either the result of `doSomething()` or `DefaultValue`.
# note, we can also do this with the `$(` `)` operator to indicate blocks, e.g..

X: if Condition $( doSomething() ) else $( calculateSideEffects(...), DefaultValue )
```

Note that ternary logic short-circuits operations, so that calling the function
`doSomething()` only occurs if `Condition` is true.  Also, only the last line
of a block can become the RHS value for a statement like this.

TODO: more discussion about how `return` works vs. putting a RHS statement on a line.

Of course you can get two values out of a conditional expression, e.g., via destructuring:

```
{X:, Y:} = if Condition
    {X: 3, Y: doSomething()}
else
    {X: 1, Y: DefaultValue}
```

Note that indent matters quite a bit here.  Conditional blocks are supposed to indent
at +1 from the initial condition (e.g., `if` or `else`), but the +1 is measured from
the line which starts the conditional (e.g., `{X, Y}` in the previous example).  Indenting
more than this would trigger line continuation logic.  I.e., at +2 or more indent,
the next line is considered part of the original statement and not a block.  For example:

```
# WARNING, PROBABLY NOT WHAT YOU WANT:
Q?: if Condition
        What + IndentTwice
# actually looks to the compiler like:
Q?: if Condition What + IndentTwice
```

Which will give a compiler error since there is no internal block for the `if` statement.

### then statements

We can rewrite conditionals to accept an additional `then` "argument".  For `if`/`elif`
statements, the syntax is `if Expression, Then:` to have the compiler guess the `then`'s
return type, or `elif Expression, WhateverName: then[whateverType]` to explicitly provide it
and also rename `Then` to `WhateverName`.  Similarly for `what` statements, e.g.,
`what Expression, WhateverName: then[whatever]` or `what Expression, Then:`.  `else`
statements of course elide the expression as `else Then:` or `else Whatever: then[elseType]`.
Note that we use a `:` here because we're declaring an instance of `then`; if we don't use
`then` logic we don't use `:` for conditionals.  Also note that `then` is a thin wrapper
around the `block` class (i.e., a reference that removes the `::loop()` method that
doesn't make sense for a `then`).  If you want to just give the type without renaming,
you can do `if Whatever, Then[myIfBlockType]:`, etc.

```
if SomeCondition, Then:
    # do stuff
    if SomeOtherCondition, Named Then:
        if SomethingElse1
            Then exit()
        if SomethingElse2
            Named Then exit()
    # do other stuff

Result: what SomeValue, Then[str]:
    5
        ...
        if OtherCondition
            Then exit("Early return for `what`")
        ...
    ...

# note that grammatically commas are equivalent to newlines,
# so you can also write conditions like this:
if SomeCondition
Then:
    print("`Then` on a newline is ok but not idiomatic")
    ...

# prefer using commas as it's slightly more readable.
# if you are running out of space, try using parentheses.
if (
        Some Long Condition
    &&  Some OtherFact
    &&  NeedThis Too
), Then:
    print("good")
    ...
```

### what statements

`what` statements are comparable to `switch-case` statements in C/C++,
but in hm-lang the `case` keyword is not required.  You can use the keyword
`else` for a case that is not matched by any others, i.e., the default case.
We switch from the standard terminology for two reasons: (1) even though
`switch X` does describe that the later logic will branch between the different
cases of what `X` could be, `what X` is more descriptive as to checking what `X` is,
which is the important thing that `what` is doing at the start of the statement.
(2) `switch` is something that a class instance might potentially want to do,
e.g., `MyInstance switch(Background1)`, and having `switch` as a keyword negates
that possibility.

TODO: non-ternary example with numbers.
TODO: explain how `case` values are cast to the same type as the value being `what`-ed.

You can use RHS expressions for the last line of each block to return a value
to the original scope.  In this example, `X` can become 5, 7, 8, or 100, with various
side effects (i.e., printing).

```
X: what String
    "hello"
        print("hello to you, too!")
        5
    # you can do multiple matches over multiple lines:
    "world"
    "earth"
        # String == "world" or "earth" here.
        print("it's a big place")
        7
    #or you can do multiple matches in a single line with commas:
    "hi", "hey", "howdy"
        # String == "hi", "hey", or "howdy" here.
        print("err, hi.")
        8
    else
        100

# Note again that you can use `$(` ... `)` block operators to make these inline.
# Try to keep usage to code that can fit legibly on one line:
Y: what String $( "hello" $(5), "world" $(7), else $(100) )
```

You don't need to explicitly "break" a `case` statement like in C/C++.
Because of that, a `break` inside a `what` statement will break out of
any enclosing `for` or `while` loop.  This makes `what` statements more
like `if` statements in hm-lang.

```
AirQualityForecast: ["good", "bad", "really bad", "bad", "ok"]
MehDays; 0
for Quality: in AirQualityForecast
    what Quality
        "really bad"
            print("it's going to be really bad!")
            break   # stops `for` loop
        "good"
            print("good, that's good!")
        "bad"
            print("oh no")
        "ok"
            ++MehDays
```

The `what` operation is also useful for narrowing in on `oneOf` variable types.
E.g., suppose we have the following:

```
status: oneOf[Unknown, Alive, Dead]
vector3: {X; dbl, Y; dbl, Z; dbl}

update: oneOf[
    status
    position: vector3
    velocity: vector3
]
# example usage:
Update0: update status Alive
Update1: update position(X: 5.0, Y: 7.0, Z: 3.0)
Update2: update velocity(X: -3.0, Y: 4.0, Z: -1.0)
```

We can determine what the instance is internally by using `what` with
variable declarations that match the `oneOf` type and field name.
We can do this alongside standard `switch`-like values, like so,
with earlier `what` cases taking precedence.  

```
...
# checking what `Update` is:
what Update
    status Unknown
        print("unknown update")
    Status:
        # match all other statuses:
        print("known status: $(Status)")
    Position: vector3
        print("got position update: $(Position)")
    Velocity: vector3
        print("got velocity update: $(Velocity)")
```

We don't use function style here, e.g.,
`what Update $( (Position: vector3): print("got position update: $(Position)") )`,
because a `return` from such a function should technically only get you out of the `what`
block and not out of the function that has `what` inside.

Note that variable declarations can be argument style, i.e., including
temporary declarations (`.`), readonly references (`:`), and writable
references (`;`), since we can avoid copies if we wish.  This is only
really useful for allocated values like `str`, `int`, etc.  However, note
that temporary declarations via `.` can only be used if the argument to
`what` is a temporary, e.g., `what MyValue!` or `what SomeClass value()`.
There is no need to pass a value as a mutable reference, e.g., `what MyValue;`;
since we can infer this if any internal matching block uses `;`.

```
whatever: oneOf[
    str
    card: {Name: str, Id: u64}
]

Whatever; whatever str("this could be a very long string, don't copy if you don't need to")

what Whatever!      # ensure passing as a temporary by mooting here.
    Str.
        print("can do something with temporary here: $(Str)")
        doSomething(Str!)
    Card.
        print("can do something with temporary here: $(Card)")
        doSomethingElse(Card!)
```

### what implementation details

```
# The implementation can be pretty simple.
switch (Update.hm_Is) {
    case update::status::hm_Is:
        DEFINE_CAST(update::status *, Status, &Update.hm_Value);
        if (*Status == status::Unknown) {
            // print("unknown update")
            ...
        } else {
            // print("known status: $(Status)")
            ...
        }
        break;
    case update::position::hm_Is:
        DEFINE_CAST(update::position *, Position, &Update.hm_Value);
        // print("got position update: $(Position))
        break;
    ...
}
```

Implementation details for strings: at compile time we do a fast hash of each 
string case, and at run time we do a switch-case on the fast hash of the considered
string.  (If the hashes of any two string cases collide, we redo all hashes with a
different salt.)  Of course, at run-time, there might be collisions, so before we
proceed with a match (if any), we check for string equality.  E.g., some pseudo-C++
code:

```
switch (fastHash(ConsideredString, CompileTimeSalt)) {
    case fastHash(StringCase1, CompileTimeSalt): { // precomputed with a stable hash
        if (ConsideredString != StringCase1) {
            goto __Default__;
        }
        // logic for StringCase1...
        break;
    }
    // and similarly for other string cases...
    default: {
    __Default__:
        // logic for no match
    }
}
```

TODO: do we even really want a `fallThrough` keyword?  it makes it complicated that it
will essentially be a `goto` because fall through won't work due to the check for string
equality.

We'll add a compiler hint with the best compile-time `Salt` to the `what` statement,
so that future transpilations are faster.  The compiler will still try more salts
if the chosen salt doesn't work, however, e.g., in the situation where new cases
were added to the `what` statement.

```
X: what String    #salt(1234)
    "hello"
        print("hello to you, too!")
        5
    "world"
        print("it's a big place")
        7
    else
        100
```

Similarly, any class that supports a compile-time fast hash with a salt can be
put into a `what` statement.  Floating point classes or containers thereof
(e.g., `dbl` or `flt[]`) are not considered *exact* enough to be hashable, but
hm-lang will support fast hashes for classes like `int`, `i32`, and `u64[]`,
and other containers of precise types, as well as recursive containers thereof.

```
# TODO: there should maybe be a way to avoid using extend syntax for all interfaces.
#       maybe we can do `@override ::hash(~Builder): null` for common interfaces
# TODO: maybe something like `myHashableClass: { ... }, assert(myHashableClass is hashable)`.
#       even better, maybe the callers should be responsible for checking if a class is
#       hashable (or whatever).
myHashableClass: hashable {
    Id: u64
    Name; string

    # we allow a generic hash builder so we can do cryptographically secure hashes
    # or fast hashes in one definition, depending on what is required.
    # This should automatically be defined for classes with precise fields (e.g., int, u32, string, etc.)!
    ::hash(~Builder;):
        Builder hash(My Id)       # you can use `hash` via the builder or...
        My Name hash(Builder;)    # you can use `hash` via the field.

    # TODO: can we come up with a cleaner syntax to make this `null`?
    # consider doing sequence building for the hash builder:
    ::hash(~Builder;): null
        Builder {
            hash(My Id)
            hash(My Name)
        }
}

# note that defining `::hash(~Builder;)` automatically defines a `fastHash` like this:
# fastHash(MyHashableClass, ~Salt): salt
#   Builder: \\hash fast(Salt)
#   Builder hash(MyHashableClass)
#   return Builder build()

MyHashableClass: myHashableClass(Id: 123, Name: "Whatever")

what MyHashableClass
    myHashableClass(Id: 5, Name: "Ok")
        print("5 OK")
    myHashableClass(Id: 123, Name: "Whatever")
        print("great!")
    MyHashableClass:
        print("it was something else: $(MyHashableClass)")
```

Note that if your `fastHash` implementation is terrible (e.g., `fastHash(Salt): Salt`),
then the compiler will error out after a certain number of attempts with different salts.

For sets and stores, we use a hash method that is order-independent (even if the container
is insertion-ordered).  E.g., we can sum the hash codes of each element, or `xor` them.
Arrays have order-dependent hashes, since `[1, 2]` should be considered different than `[2, 1]`,
but the store `{"hi": 1, "hey": 2}` should be the same as `{"hey": 2, "hi": 1}` (different
insertion order, but same contents).

## for loops

TODO: Can we write other conditionals/loops/etc. in terms of `indent/block` to make it easier to compile
from fewer primitives?  E.g., `while Condition, Do: $(... Do exit(3) ...)`, where
`do` is a thin wrapper over `block`?

```
# for-loop with counter that is readonly inside the for-loop's block:
for Value: int < 10
    # Value goes from 0 to 9 by 1;
    # Value is not allowed to be mutated (defined with `:`).
    # trying to mutate it throws a compiler error.
    print(Value)
# prints "0" to "9" on separate newlines.

# for-loop whose counter can be modified inside the block.
# not recommended, since it's a bit harder to reason about.
for Special; int < 4
    print("A:$(Special)")
    ++Special
    print("B:$(Special)")
    ++Special
# prints "A:0", "B:1", "A:3", "B:4"     # notice skip from B:1 to A:3 as `Special`
# increments on its own because of the for-loop iteration logic.  Note also the
# possibly undesired behavior that `Special` becomes >= 4 inside the for-loop block.
# Prefer while loops here to be explicit about when/how the incrementing occurs.

# you can do a for-loop with an existing variable.
# this allows you to start at a different value, and keep the last value from the for loop.
# NOTE the variable should be writable!
IteratingIndex; 3
for IteratingIndex < 7
    print(IteratingIndex)
# prints 3, 4, 5, 6 each on new lines.
assert IteratingIndex == 7      # you can keep IteratingIndex for use outside the for loop.

# you can also inline the variable's starting value,
# but this means you don't have access to the variable outside the for loop (probably for the best).
for OtherIndex: index(3), OtherIndex < 7
    # note that `OtherIndex` is readonly inside this block, but will be updated by the for loop.
    print(OtherIndex)

# TODO: consider if we want to add `OtherIndex += 2` logic; i'm not the biggest fan of it.
#       i'd prefer it go in the middle, but that would probably confuse people

# for-loop iterating over non-number elements:
vector2: {X: dbl, Y: dbl}
Array: vector2[] = [{X: 5, Y: 3}, {X: 10, Y: 17}]
for Vector2: in Array       # `for (Vector2:) in Array` also works.
    print(Vector2)

# if the variable is already declared, you avoid the declaration `:` or `;`:
# NOTE the variable should be writable!
IteratingVector; vector2
for IteratingVector in Array
    print(IteratingVector)
# this is useful if you want to keep the result of the last element outside the for-loop.
```

# printing and echoing output

TODO: allow tabbed print output.  instead of searching through each string,
maybe we just look at `print` and add the newlines at the start.  Each thread should
have its own tab stop.  E.g.,

```
array[of]: {
    ...
    ::print(): null
        if My count() == 0
            return print("[]")
        print("[")
        with indent():
            for Of: in Me
                print(Of)
        print("]")
}
```

TODO: defining `print` on a class will also define the `string()` method.
essentially any `print`s called inside of the class `print` will be redirectable to
a string-stream, etc.  `indent` should maybe do something different for `string()`;
maybe just add commas *after* the line instead of spaces before the line to be printed.

TODO: we should also have a print macro here in case we want to stop printing,
e.g., in case the string buffer is full (e.g., limited output).  if the print
command triggers a stop at any point, then abort (and stop calling the method)

## blocks

You can write your own `assert` or `return`-like statements using `block` logic.  The `block`
class has a method to return early if desired.  Calling `Block exit(...)` shortcircuits the
rest of the block (and possibly other nested blocks).  This is annotated by using the `jump`
return value.  You can also call `Block loop()` to return to the start of the block.
You don't usually create a `block` instance; you'll use it in combination with the global
`indent` function.

```
# indent function which returns whatever value the `Block` exits the loop with.
indent(fn(Block[~t]): never): t
# indent function which populates `Block Declaring` with the value passed in.
indent(~Declaring., fn(Block[~t, declaring]): never): t

# TODO: can we use `block[of: int, declaring; string]` to make
#       `Declaring: declaring` convert into `Declaring; string`??
#       maybe we need another symbol for it, e.g., `Declaring~: declaring`
@referenceableAs(then)
block[of, declaring: null]: {
    # variables defined only for the lifetime of this block's scope.
    # TODO: give examples, or maybe remove, if this breaks cleanup with the `jump` ability
    Declaring; declaring

    # exits the `indent` with the corresponding `of` value.  example:
    #   Value; 0
    #   what indent((Block[str]): never
    #       Old Value: Value
    #       Value = Value // 2 + 9
    #       # sequence should be: 0, 9, 4+9=13, 6+9=15, 7+9=16, 8+9=17
    #       if Old Value == Value
    #           Block exit("exited at $(Old Value)")
    #       # note we need to `loop` otherwise we won't satisfy the `never`
    #       # part of the indent function.
    #       Block loop()
    #   )
    #       String.
    #           print(String)       # should print "exited at 17"
    ::exit(Of.): jump

    # like a `continue` statement; will bring control flow back to
    # the start of the `indent` block.  example:
    #   Value; 0
    #   indent((Block[str]):
    #       if ++Value >= 10 $(Block exit("done"))
    #       if Value % 2
    #           Block loop()
    #       print(Value)
    #       Block loop()
    #   )
    #   # should print "2", "4", "6", "8"
    @hideFrom(then)
    ::loop(): jump
}
```

TODO: can we use an `um` internally inside `block`?

### blocks to define a variable

TODO: i don't think this makes much sense to do explicitly, e.g.,
```
MyInt, Block[int]:
    if someCondition()
        Block exit(3)
    Block loop()
```

Mostly because we can't type `MyInt` as `;` or `:` in this way.


### then with blocks

When using `then`, it's recommended to always exit explicitly, but like with the
non-`then` version, the conditional block will exit with the value of the last
executed line.  There is a rawer version of this syntax that does require an
explicit exit, but also doesn't allow any `return` functions since we are literally
defining a `(Then): never` with its block inline.  This syntax is not recommended
unless you have identical block handling in separate conditional branches, but
even that probably could be better served by pulling out a function to call in
both blocks.

```
if SomeCondition, Block[str]:
    if OtherCondition
        if NestedCondition
            # breaks out of `Block`
            return X
    else
        Block exit("whatever")
    # COMPILE ERROR, this function returns here if
    # `OtherCondition && !NestedCondition`.

# here's an example where we re-use a function for the block.
MyBlock: block[str]
    ... complicated logic ...
    exit("made it")

# not super excited by this syntax, but should be ok.
if SomeCondition, MyBlock
elif SomeThingElse
    print("don't use it here")
else MyBlock
```

### function blocks

Similar to conditionals, we allow defining functions with `block` in order
to allow low-level flow control.  Declarations like `myFunction(X: int): str`,
however, will be equivalent to `myFunction(X: int), Block: block[str]` or
the more raw form `myFunction(X: int, Block: block[str]): never`.  Thus
there is no way to overload a function defined with `block` statements compared
to one that is not defined explicitly with `block`.

```
# the `never` return type means that this function can't use `return`, either
# explicitly via `return ...` or implicitly by leaving a value as the last
# evaluated statement (which can occur if you don't use `Block exit(...)`
# or `Block loop(...)` on the last line of the function block).
# i.e., you must use `Block exit(...)` to return a value from this function.
myFunction(X: int, Block[str]): never
    innerFunction(Y: int): dbl
        if Y == 123
            Block exit("123")    # early return from `myFunction`
        Y dbl() orPanic()
    for Y: int < X
        innerFunction(Y)
    Block exit("normal exit")

# this definition essentially is syntactical sugar for the one above.
myFunction(X: int), Block[str]:
    innerFunction(Y: int): dbl
        if Y == 123
            Block exit("123")    # early return from `myFunction`
        Y dbl() orPanic()
    for Y: int < X
        innerFunction(Y)
    "normal exit"
```

We'll need to use look ahead for these cases to see that the function
is being declared/defined, since there is no `:` immediately after
`myFunction`.

## coroutines

Coroutines use an outer `co[of]` class with an inner `ci[of]` class.  (`i` for inner and
`o` for outer.)  The outer class has methods to grab the next value from the inner
coroutine.

```
cv[of]: oneOf[Cease, Value: of]

co[of]: {
    # TODO: think about how `resumable` works, probably should be `Resumable`
    ;;renew(My resumable(Ci[of]): never): null

    ;;take(): cv[of]

    @alias ;;next(): I take()
}

ci[of]: resumable[oneOf[Cease, Value: of]] {
    # returns control back to caller of `co;;take` with a new value.
    ::give(Of.): jump
        ::exit(Of!)

    @alias ::yield(Of.): I give(Of.)

    # returns control back to caller of `co;;take` but to `Cease`.
    ::exit(): jump
        ::exit(Cease)

    @alias ::quit(): I exit()

    @hide ::exit(Of.): jump
}
```

```
countdown: co[int] {
    ;;renew(My Int.): Co renew((Ci[int];):
        while My Int > 0
            Ci give(--My Int)
        Ci exit()
    )
}

# implicit usage
for Int: in countdown(20)
    print(Int)      # prints 19, 18, ..., 0

# explicit usage
Co; countdown(20)
while True
    what Co take()
        Value.
            print(Value)
        Cease
            break
    # also OK:
    if Co take() is Value.
        print(Value)
    else
        break
```

# futures

hm-lang wants to make it very simple to do async code, without additional
metadata on functions like `async` (JavaScript).  It's also desirable to
avoid even acknowledging that your function returns a future, since changing
an inner function to return a future would then require changing all nested
functions' signatures to return futures.  Instead, functions return the
value that they will receive after any futures are completed (and we recommend
a timeout `uh` being present for a result error).  If the caller wants to
treat the function as a future, i.e., to run many such futures in parallel,
then they ask for it as a future using `@um` before the function name, which
returns the `um[of]` type, where `of` is the normal function's return type.
You can also type the variable explicitly as `um[of]` and then avoid using `@um`
before the function name.

```
someVeryLongRunningFunction(Int): string
    Result; ""
    for New Int < Int
        sleep(Seconds: New Int)
        Result += str(New Int)
    Result

# this uses the default `string` return value:
print("starting a long running function...")
MyName: someVeryLongRunningFunction(10)
print("the result is $(MyName) many seconds later")

# this does it as a future
print("starting a future, won't make progress unless polled")
# `Future` here has the type `um[string]`:
Future: @um someVeryLongRunningFunction(10)
# Also ok: `Future: um[string] = someVeryLongRunningFunction(10)`
# TODO: can `~` by itself count as the `auto` keyword?
# probably also ok: `Future: um[~] = someVeryLongRunningFunction(10)`
print("this `print` executes right away")
Result: string = Future
print("the result is $(Result) many seconds later")
```

That is the basic way to resolve a future, but you can also use
the `::decide(): of` method for an explicit conversion from `um[of]`
to `of`.  Ultimately futures are more useful when combined for
parallelism.  Here are two examples, one using an array of futures
and one using an object of futures:

```
after(Seconds: int, Return: string): string
    sleep(Seconds)
    Return

FuturesArray[]: um[string]
# no need to use `@um after(...)` here since `FuturesArray`
# elements are already typed as `um[string]`:
FuturesArray append(after(Seconds: 2, Return: "hello"))
FuturesArray append(after(Seconds: 1, Return: "world"))
ResultsArray: decide(FuturesArray)
print(ResultsArray) # prints `["hello", "world"]`

# here we use sequence building to ensure we're creating futures,
# i.e., `@um {A, B}` has type `{A: um[a], B: um[b]}` and executes `A`/`B` asynchronously.
FuturesObject: @um {
    Greeting: after(Seconds: 2, Return: "hello")
    Noun: after(Seconds: 1, Return: "world")
}
print(decide(FuturesObject)) # prints `{Greeting: "hello", Noun: "world"}`
```

Notice that all containers with `um` types for elements will have
an overload defined for `decide`, which can be used like with the
`FuturesArray` example above.  Similarly all object types with `um`
fields have a `decide` function that awaits all internal fields that
are futures before returning.

# enums and masks

## enumerations

We can create a new type that exhaustively declares all possible values it can take.
The syntax is `lowerCamelCase: oneOf` followed by a list of named values
(each an `UpperCamelCase` identifier), with optional values they take.  Enumerations
are mutually exclusive -- no two values may be held simultaneously.  See
masks for a similar class type that allows multiple options at once.

Enums are by default the smallest standard integral type that holds all values,
but they can be signed types (in contrast to masks which are unsigned).
If desired, you can specify the underlying enum type using `@i8 oneOf[...]` instead
of `oneOf[...]`, but this will be a compile error if the type is not big enough to
handle all options (or if the enum requires more complicated storage due to non-integer values).

Here is an example enum with some values that aren't specified.  Even though
the values aren't specified, they are deterministically chosen.

```
myEnum: oneOf[
    FirstValueDefaultsToZero
    SecondValueIncrements
    ThirdValueIsSpecified: 123
    FourthValueIncrements
]
assert myEnum FirstValueDefaultsToZero == 0
assert myEnum SecondValueIncrements == 1
assert myEnum ThirdValueIsSpecified == 123
assert myEnum FourthValueIncrements == 124
```

You can even pass in existing variable(s) to the enum, although they should be
compile-time constants.  This uses the same logic as function arguments to
determine what the name of the enum value is.

```
Super: 12
Crazy: 15
# the following will define
# `otherEnum OtherValue1 = 0`,
# `otherEnum Super = 12`,
# and `otherEnum OtherValue2 = 15`.
otherEnum: oneOf[
    OtherValue1
    Super
    OtherValue2: Crazy
]
```

Here is an example enum with just specified values, all inline:

```
# fits in a `u1`.
bool: oneOf[False: 0, True: 1]
```

Enums provide a few extra additional methods for free as well, including
the number of values that are enumerated via the class function `count(): count`,
and the min and max values `min(): enumType`, `max(): enumType`.  You can also
check if an enum instance `Enum` is a specific value `ThisValue` via
`Enum isThisValue()` which will return true iff so.

```
Test: bool = False  # or `Test: bool False`

if Test == True     # OK
    print("test is true :(")
if Test isFalse()   # also OK
    print("test is false!")

# get the count (number of enumerated values) of the enum:
print("bool has $(bool count()) possibilities:")
# get the lowest and highest values of the enum:
print("starting at $(bool min()) and going to $(bool max())")
```

Because of this, it is a bit confusing to create an enum that has `Count` as an
enumerated value name, but it is not illegal, since we can still distinguish between the
enumerated value (`enumName Count`) and total number of enumerated values (`enumName count()`).

Also note that the `count()` method will return the total number of
enumerations, not the number +1 after the last enum value.  This can be confusing
in case you use non-standard enumerations (i.e., with values less than 0):

```
sign: oneOf[
    Negative: -1
    Zero: 0
    Positive: 1
]

print("sign has $(sign count()) values")   # 3
print("starting at $(sign min()) and going to $(sign max())")     # -1 and 1

weird: oneOf[
    X: 1
    Y: 2
    Z: 3
    Q: 9
]

print(weird count())   # prints 4
print(weird min())     # prints 1
print(weird max())     # prints 9
```

### default values for a `oneOf`

Note that the default value for a `oneOf` is the first value, unless `null` is an option.
E.g., `oneOf[OptionA, OptionB]` defaults to `OptionA` but `oneOf[OptionC, Null]` would
default to `Null` and `oneOf[type1, type2, null]` defaults to the `null` type.  Nulls are
highly encouraged to come last in a `oneOf`, because they will match any input, so
casting like this: `oneOf[null, int](1234)` would actually become `Null` rather than
the expected value `1234`, since casts are attempted in order of the `oneOf` types.
Note that `oneOf[Null, X]` does not collapse to `oneOf[X]` because `[]` acts more like
an array in this case, and arrays can contain `Null`.

### testing enums with lots of values

Note that if you are checking many values, a `what` statement may be more useful
than testing each value against the various possibilities.  Also note that you don't need
to explicitly set each enum value; they start at 0 and increment by default.

```
option: oneOf[
    Unselected
    NotAGoodOption
    ContentWithLife
    BetterOptionsOutThere
    BestOptionStillComing
    OopsYouMissedIt
    NowYouWillBeSadForever
]

print("number of options should be 7:  $(option count())")

Option1: option ContentWithLife

# avoid doing this if you are checking many possibilities:
if Option1 isNotAGoodOption()   # OK
    print("oh no")
elif Option1 == OopsYouMissedIt # also OK
    print("whoops")
...

# instead, consider doing this:
what Option1
    NotAGoodOption
        print("oh no")
    BestOptionStillComing
        print("was the best actually...")
    Unselected
        fallThrough
    else
        print("that was boring")
```

Note that we don't have to do `option NotAGoodOption` (and similarly for other options)
along with the cases.  The compiler knows that since `Option1` is of type `option`,
that you are looking at the different values for `option` in the different cases.

### Default value of an enumeration

Defaults to whatever value is 0 (the first, if no values are specified).

If no value is zero, then the first specified value is default.

## declaring more complicated types via `oneOf`

Take this example `oneOf`.

```
tree: oneOf[
    leaf: {Value; int}
    branch: {
        Left; tree
        Right; tree
    }
]
```

When checking a `tree` type for its internal structure, you can use `isLeaf()` or `isBranch()`
if you just need a boolean, but if you need to manipulate one of the internal types, you should
use `::is(fn(InternalType): null): bool` or `;;is(fn(InternalType;): null): bool` if you need to modify
it, where `internalType` is either `leaf` or `branch` in this case.  For example:

```
Tree; tree = if Abc
    leaf(Value: 3)
else
    branch(Left: leaf(Value: 1), Right: leaf(Value: 5))

if Tree isLeaf()
    # no type narrowing, not ideal.
    print(Tree)

# narrowing to a `leaf` type that is readonly, while retaining a reference
# to the original `Tree` variable.  the nested function only executes if
# `Tree` is internally of type `leaf`:
Tree is((Leaf): print(Leaf))

# narrowing to a `branch` type that is writable.  `Tree` was writable, so `Branch` can be.
# the nested function only executes if `Tree` is internally of type `branch`:
Tree is((Branch;):
    print(Branch Left, " ", Branch Right)
    # this operation can affect/modify the `Tree` variable.
    Branch Left someOperation()
)

# you can also use this in a conditional; note we don't wrap in a lambda function
# because we're using fancier `Block` syntax.
if Tree is Branch;
    Branch Left someOperation()
    print("a branch")
else
    print("not a branch") 
```

If you need to manipulate most of the internal types, use `what` to narrow the type
and handle all the different cases.

```
what Tree
    Leaf: 
        print(Leaf)
    Branch;
        # this operation can affect/modify the `Tree` variable.
        print(Branch Left, " ", Branch Right)
        Branch Left someOperation()
```

Since function arguments are references by default, the above options are useful
when you want to modify the `Tree` variable by changing its internals whether
selectively it's a `leaf` or a `branch`.  If you want to make a copy, you can do so
via type casting: `NewLeaf?; leaf = Tree` or `MyBranch?; branch = Tree`; these
variables will be null if the `Tree` is not of that type, but they will also be
a copy and any changes to the new variables will not be reflected in `Tree`.

```
oneOf[..., ~t]: {
    # returns true if this `oneOf` is of type `T`, also allowing access
    # to the underlying value by passing it into the function.
    # we return `never` here because we don't want people to use the
    # value and expect it to return something based on the callback's return type,
    # or be confused if it should always return true if the internal type is `t`.
    ;:is(fn(T;:): null): never

    # type narrowing.
    # the signature for `if Tree is Branch; $(#[do stuff with `Branch`]#)`
    # the method returns true iff the block should be executed.
    # the block itself can return a value to the parent scope.
    ;:.is(), Block[declaring:;. t, exit: ~u]: bool
}
```

## `oneOf`s as function arguments

The default name for a `oneOf` argument is `OneOf`.  E.g.,

```
# this is short for `myFunction(OneOf: oneOf[int, str]): dbl`:
myFunction(OneOf[int, str]): dbl
    return dbl(OneOf)

print(myFunction(123))      # prints 123.0
print(myFunction("123.4"))  # prints 123.4
```

Internally this creates multiple function overloads:  one for when the argument's
type is unknown at compile time, and one for each of the possible argument types
when it is known (e.g., `int` and `str` in this case).

If you need to use multiple `oneOf`s in a function and still want them to be
default-named, it's recommended to give specific names to each `oneOf`, e.g.,

```
intOrString: oneOf[int, str]
weirdNumber: oneOf[u8, i32, dbl]

myFunction(IntOrString, WeirdNumber): dbl
    return dbl(IntOrString) * WeirdNumber
```

However, you can also achieve the same thing using namespaces,
if you don't want to add specific names for the `oneOf`s.

```
# arguments short for `A OneOf: oneOf[int, str]` and `B OneOf: oneOf[u8, i32, dbl]`.
myFunction(A OneOf[int, str], B OneOf[u8, i32, dbl]): dbl
    return dbl(A OneOf) * B OneOf
```

Again, this fans out into multiple function overloads for each of the cases
of compile-time known and compile-time unknown arguments.

TODO: ensure that we can use `Call` or similar to create our own version
of a `oneOf` with metaprogramming or whatever.

## masks

Masks are generalized from enumerations to allow multiple values held simultaneously.
Each value can also be thought of as a flag or option, which are bitwise OR'd together
(i.e., `|`) for the variable instance.  Under the hood, these are unsigned integer types.
Trying to assign a negative value will throw a compiler error.

Like with enums, you can specify the integer type that backs the mask, but in this case
it must be an unsigned type, e.g., `mask u32`.  Note that by default, the `maskType`
is exactly as many bits as it needs to be to fit the desired options, rounded up to
the nearest standard unsigned type (`u8`, `u16`, `u32`, `u64`, `u128`, etc.).
We will add a warning if the user is getting into the `u128+` territory.

TODO: is there a good generalization of "any type in an enum" functionality
that rust provides, for masks?

Since masks can be any or none of the options specified, they are created using
the `anyOrNoneOf` function.  Like with `oneOf` enumerations, masks don't need
to specify their values; but unlike `oneOf` enumerations, if you do specify them,
they must be powers of two.  Like enums, they have an `isThisValue()` method
for a `ThisValue` option, which is true if the mask is exactly equal to `ThisValue`
and nothing else.  You can use `hasThisValue()` to see if it contains `ThisValue`,
but may contain other values besides `ThisValue`.

TODO: should this be `containsThisValue()` to be consistent with containers?

TODO: is there a way to make this `anyOf` and use 0 as the `Null` value?

```
food: anyOrNoneOf[
    Carrots
    Potatoes
    Tomatoes
]
# this creates a mask with `food Carrots == 1`,
# `food Potatoes == 2`, and `food Tomatoes == 4`.
# there is also a default `food None == 0`.

Food: food = Carrots | Tomatoes
Food hasCarrots()   # true
Food hasPotatoes()  # false
Food hasTomatoes()  # true
Food isCarrots()    # false, since `Food` is not just `Carrots`.
```

And here is an example with specified values.

```
# the mask is required to specify types that are powers of two:
nonMutuallyExclusiveType: anyOrNoneOf[
    X: 1
    Y: 2
    Z: 4
    T: 32
]
# `nonMutuallyExclusiveType None` is automatically defined as 0.

# has all the same static methods as enum, though perhaps they are a bit surprising:
nonMutuallyExclusiveType count() == 16
nonMutuallyExclusiveType min() == 0
nonMutuallyExclusiveType max() == 39   # = X | Y | Z | T

Options; nonMutuallyExclusiveType()
Options == 0        # True; masks start at 0, or `None`,
                    # so `Options == None` is also true.
Options |= X        # TODO: make sure it's ok to implicitly add the mask type here.
                    #       maybe it's only ok if no `X` is in scope, otherwise
                    #       you need to make it explicit.
Options |= nonMutuallyExclusiveType Z   # explicit mask type

Options hasX()  # True
Options hasY()  # False
Options hasZ()  # True
Options hasT()  # False

Options = T
Options isT()   # True
Options hasT()  # True
```

## interplay with `oneOf`

We can also create a mask with one or more `oneOf` fields, e.g.:

```
options: anyOrNoneOf[
    oneOf[AlignCenterX, AlignLeft, AlignRight]
    oneOf[AlignCenterY, AlignTop, AlignBottom]

    oneOf[FontVerySmall, FontSmall, FontNormal: 0, FontLarge, FontVeryLarge]
]
```

It is a compiler error to assign multiple values from the same `oneOf`:

```
Options; options = AlignCenterX | AlignRight     # COMPILER ERROR!
```

Note that internally, an `OR` combination of the `oneOf` values may actually be valid;
it may be another one of the `oneOf` values in order to save bits.  Otherwise, each
new value in the `oneOf` would need a new power of 2.  For example, we can represent
`oneOf[AlignCenterX, AlignLeft, AlignRight]` with only two powers of two, e.g.,
`AlignCenterX = 4`, `AlignLeft = 8`, and `AlignRight = 12`.  Because of this, there
is special logic with `|` and `&` for `oneOf` values in masks.

```
Options2; options = AlignCenterX
Options2 |= AlignRight    # will clear out existing AlignCenterX/Left/Right first before `OR`ing
if Options2 & AlignCenterX
    print("this will never trigger even if AlignCenterX == 4 and AlignRight == 12.")
```

You can also explicitly tell the mask to avoid assigning a power of two to one of the
`oneOf` values by setting it to zero (e.g., `oneOf[..., Value: 0, ... ]`.
For example, the font size `oneOf` earlier could be represented by 3 powers of two, e.g.,
`FontVerySmall = 16`, `FontSmall = 32`, `FontLarge = 64`, `FontVeryLarge = 96`.
Note that we have the best packing if the number of non-zero values is 3 (requiring 2 powers of two),
7 (requiring 3 powers of two), or, in general, one less than a power of two, i.e., `2^P - 1`,
requiring `P` powers of two.  This is because we need one value to be the default for each
`oneOf` in the `mask`, which will be all `P` bits set to zero; the remaining `2^P - 1`
combinations of the `P` bits can be used for the remaining `oneOf` values.  A default
name can thus be chosen for each `oneOf`, e.g., `oneOf[..., WhateverName: 0, ...]`.

## named value-combinations

You can add some named combinations by extending a mask like this.

```
myMask: anyOrNoneOf[X, Y] {
    XAndY: X | Y
}

Result: myMask = XAndY
print(Result & X) # truthy, should be 1
print(Result & Y) # truthy, should be 2
```

# lifetimes and closures

## lifetimes of variables and functions

Variable and function lifetimes are usually scoped to the block that they
were defined in.  Initialization happens when they are encountered, and
descoping/destruction happens in reverse order when the block ends.  With
functions we have to be especially careful when they are impure.
If an impure function's lifetime exceeds that of any of its hidden
inputs' lifetimes, we'll get a segfault or worse.

Let's illustrate the problem with an example:

```
# define a re-definable function.
liveItUp(String); index
    return String countBytes()

if SomeCondition
    SomeIndex; index = 9
    # redefine:
    liveItUp(String); index
        return String countBytes() + ++SomeIndex

    print(liveItUp("hi"))   # this should print 12
    print(liveItUp("ok"))   # this should print 13

print(liveItUp("no"))       # should this print 14 or 2??
```

Within the `if SomeCondition` block, a new variable `SomeIndex` gets defined,
which is used to declare a new version of `liveItUp`.  But once that block
is finished, `SomeIndex` gets cleaned up without care that it was used elsewhere,
and if `liveItUp` is called with the new definition, it may segfault (or start
changing some other random variable's data).  Therefore, we must not allow the
expectation that `liveItUp("no")` will return 14 outside of the block.

We actually don't want `liveItUp("no")` to return 2 here, either; we want this
code to fail at compilation.  We want to detect when users are trying to do
closures (popular in garbage-collected languages) and let them know this is
not allowed; at least, not like this.

You can define variables and use them inside impure functions
but they must be defined *before* the impure function is first declared.  So this
would be allowed:

```
SomeIndex; index = 9
liveItUp(String); index
    return String countBytes()

if SomeCondition
    # redefine:
    liveItUp(String); index
        return String countBytes() + ++SomeIndex

    print(liveItUp("hi"))   # this should print 12
    print(liveItUp("ok"))   # this should print 13

print(liveItUp("no"))       # prints 14
```

Alternatively, we allow an impure function to "take" a variable into
its own private scope so that we could redefine `liveItUp` here with a new
internal variable.  We still wouldn't allow references; they'd have to be
new variables scoped into the function block.

```
if SomeCondition
    liveItUp(String); index
        # "@dynamic" means declare once and let changes persist across function invocations.
        # Note that this functionality cannot be used to store references.
        @dynamic X; int = 12345
        return String countBytes() + ++X
```

Similarly, returning a function from within a function is breaking scope:

```
nextGenerator(Int; int): fn(): int
    return ():
        return ++Int
```

However, technically this is OK because arguments in functions are references,
and they should therefore be defined before this function is called.
Even if `Int;` here is a temporary, it is defined before this function
(and not deleted after the function call).

Here is an example where the return value is a function which uses
the another function from the input.  This is ok because the returned
function has internals that have greater lifetimes than itself; i.e.,
the input function will be destroyed only after the output function
is descoped.

```
# function that takes a function as an argument and returns a function
# example usage:
#   someFn(): "hey"
#   otherFn: wow(someFn)
#   print(otherFn()) # 3
wow(Input fn(): string): fn(): int
    return (): int
        return Input fn() countBytes()
```

## the "no pointers" rule

TODO: is this applicable anymore now that we have argument objects?
i think so, because we can't hide implementation details in argument objects;
e.g., we can't make an argument object into a struct with public/private methods.
or do we want to allow that?

To modify a value that is held by another class instance, e.g., the element of
an array, we can use references (i.e., via passing in with `;`).  However, we
are not allowed to grab a hold of the reference for longer than the
duration of the function call/scope.  We use some syntax sugar in order
to make this not look clumsy.

```
ArrayArray; int[][] = [[1,2,3], [5]]
# to modify the array held inside the array, we can use this syntax:
ArrayArray[0] append(4)     # now ArrayArray == [[1,2,3,4], [5]]
# but under the hood, this is converted to something like this:
ArrayArray[0, (Nested Array; int[]): null
    Nested Array append(4)
]
```

This function approach avoids lifetime issues that can arise with pointers,
because the function cannot escape with a reference to the element held
by the class instance.  In languages with pointers, like C++, you could
e.g. get a pointer to an element of an array, and later try to modify
the element via the pointer.  The element might not exist anymore
(e.g., the array was shrunk), and in the worst case, the array
might not even exist (e.g., the array was descoped).

TODO: we probably actually want pointers for interfacing with C/C++ code.
but they should be hidden behind a type that requires an `@unsafe` label,
e.g., `ptr u64`.

## handling system callbacks

We want to allow a `caller`/`callee` contract which enables methods defined
on one class instance to be called by another class instance, without being
in the same nested scope.  (This is strictly regulated, since it is an exception
to the "no pointers" rule.)  The `caller` which will call the callback needs
to be defined before the `callee`, e.g., as a singleton or other instance.
When the `callee` is descoped, it will deregister itself with the `caller`
internally, so that the `caller` will no longer call the `callee`.

```
variableAccess: oneOf[Mutable, Readonly]
caller[t, VariableAccess]: {
    Callees[ptr[callee[t, VariableAccess]]];
    @if VariableAccess == Readonly
        ::runCallbacks(T: t): for Ptr: in Callees $(Ptr call(T)) 
    @if VariableAccess == Mutable
        ::runCallbacks(T; t): for Ptr: in Callees $(Ptr call(T;)) 
    # probably can do this with shorthand:
    # ::runCallbacks(@access(T, VariableAccess) t): null
    #       for Ptr: in Callees $(Ptr call(@access(T, VariableAccess)))
}
audio: caller[sample[~Count], Mutable] {
    # this `audio` class will call the `call` method on the `callee` class.
    # TODO: actually show some logic for the calling.

    # amount of time between samples:
    DeltaT: flt
}()

audioCallee: callee[sample[~Count], Mutable] {
    Frequency; flt = 440
    Phase; flt = 0

    ;;call(Array; sample[Count]): null
        for Index: index < Count
            Array[Index] = sample(Mono: \\math sin(2 * \\math Pi * My Phase))
            Phase += My Frequency * Audio DeltaT

    # automatically defined by the callee class:
    ;;hangUp(): null  
}

someFunction(): null
    Callee; audioCallee
    Callee Frequency = 880
    Audio call(Callee;)
    sleep(Seconds: 10)
    # `Audio hangUp(Callee;)` automatically happens when `Callee` is descoped.
```

# grammar/syntax

TODO: {} is equivalent to Null, or a null object or empty args list.

TODO: can we define the grammar in the terms below, but then create a finite state machine
which reads in the grammar below, deduces the possible states, then reads tokens in and
flips between states while reading through files?
we probably can keep it like this, but optimize `oneOfMatcher` and internally in this way.

Note on terminology:

* Declaration: declaring a variable or function (and its type) but not defining its value

* Definition: declaration along with an assignment of the function or variable.

* `Identifier`: starts with an alphabetical character, can have numerical characters after that.
    Note that underscores are **not** permitted, since they are an operator.  

* `LowerCamelCase`: Identifier which starts with a lowercase alphabetical character.

* `UpperCamelCase`: Identifier which starts with an uppercase alphabetical character.

```
# list of elements that can compose the grammar.
# doesn't include stuff like LowerCamelCase or UpperCamelCase,
# which are not grammatically relevant.
grammarElement: oneOf[
    # "TypeElement" to avoid overload with type/Type
    TypeElement
    FunctionType
    ForLoop,
    NonFunctionType
    FunctionArgsWithReturnType
    VariableName
    VariableDeclaration
    VariableDefinition
    FunctionName
    FunctionDeclaration
    FunctionDefinition
    FunctionArgument
    FunctionCall
    RhsStatement
    AtomicStatement
    ClassName
    ClassDefinition
    ClassStatement
    ClassMethod
    EndOfInput
]

tokenMatcher: {
    # don't have to restore the token array Index to the correct state,
    # consume as many tokens as you like here.  make sure to go
    # through "Grammar match(...)" in order to restore the Index
    # in case of a bad match.  @private so that only Grammar can call.
    @private
    ::match(Index;, Array: token[]): bool
}

singleTokenMatcher: tokenMatcher (
    ;;renew(My Token): Null

    # we automatically inherit the @visibility of the parent class method
    # unless we specifically provide one.
    ;;match(Index;, Array: token[]): bool
        return Index < Array count() and Array[Index++] == My Token
)

grammarMatcher: oneOf[tokenMatcher, grammarElement, token]

Grammar: {
    @private
    Elements: tokenMatcher[grammarElement] = [
        TypeElement: oneOfMatcher([
            FunctionType
            NonFunctionType
        ])
        VariableName: singleTokenMatcher(UpperCamelCase)
        VariableDeclaration: sequenceMatcher([
            VariableName
            optionalMatcher(operatorMatcher("?"))
            oneOfMatcher([operatorMatcher(":"), operatorMatcher(";")])
            TypeElement
        ])
        VariableDefinition: sequenceMatcher([
            VariableName
            optionalMatcher(operatorMatcher("?"))
            oneOfMatcher([operatorMatcher(":"), operatorMatcher(";")])
            RhsStatement
        ])
        #(#
        # for-loops
        # e.g.
        for Variable: variableType < UpperBoundExclusive
            ... use Variable from 0 to ceil(UpperBoundExclusive) - 1 ...
        # or
        for Variable: variableType <= UpperBoundInclusive
            ... use Variable from 0 to floor(UpperBoundInclusive) ...
        # TODO: support starting value, or just variable names
        Variable; variableType = 5
        for @lock Variable < UpperBoundExclusive
            ... use Variable from 0 to ceil(UpperBoundExclusive) - 1 ...
        # starting at number in the for loop
        for Variable: StartingValue, Variable < UpperBoundExclusive
            ... use Variable from StartingValue ...
        #)#
        ForLoop: oneOfMatcher([
            sequenceMatcher([
                # TODO: do we even need `keywordMatcher`, `identifierMatcher`, and `operatorMatcher`?
                #       can't we just use `singleTokenMatcher` and be done with it?
                keywordMatcher("for")
                VariableDeclaration
                oneOfMatcher([operatorMatcher("<"), operatorMatcher("<=")])
                Expression
                Block
            ])
            sequenceMatcher([
                keywordMatcher("for")
                listMatcher(FunctionArgument)
                keywordMatcher("in")
                Expression
                Block
            ])
        ])
        FunctionName: singleTokenMatcher(LowerCamelCase)
        FunctionDeclaration: oneOfMatcher([
            # `fnName(Args): retType`, `fnName?(Args): retType`, or
            # `fnName(Args)?: retType`, also allowing `;` instead of `:`.
            sequenceMatcher([
                FunctionName
                FunctionArgsWithReturnType
            ])
            # `fnName: fn(Args): retType` or similar
            sequenceMatcher([
                FunctionName
                oneOfMatcher([operatorMatcher(":"), operatorMatcher(";")])
                FunctionType
            ])
        ])
        FunctionArgsWithReturnType: sequenceMatcher([
            listMatcher(FunctionArgument)
            optionalMatcher(operatorMatcher("?"))
            oneOfMatcher([operatorMatcher(":"), operatorMatcher(";")])
            TypeElement
        ])
        FunctionDefinition: oneOfMatcher([
            # fnName(Args...): returnType
            #   BlockStatements
            sequenceMatcher([FunctionDeclaration, Block])
            # (Args...): returnType
            #   BlockStatements
            sequenceMatcher([FunctionArgsWithReturnType, Block])
            # fnName(Args...): Statement
            sequenceMatcher([
                FunctionName 
                FunctionArgsWithReturnType
                RhsStatement
            ])
            # (Args...): Statement
            sequenceMatcher([
                FunctionArgsWithReturnType
                RhsStatement
            ])
        ])
        FunctionArgument: oneOfMatcher([
            VariableDefinition
            VariableDeclaration
            FunctionDefinition
            FunctionDeclaration
        ])
        FunctionCall: sequenceMatcher([FunctionName, AtomicStatement])
        # TODO: templates, or maybe preprocess these into lowerCamelCase types with hooks
        FunctionType: sequenceMatcher([
            optionalMatcher(identifierMatcher("fn"))
            FunctionArgsWithReturnType
        ])
        # TODO: templates, but see above.
        NonFunctionType: oneOfMatcher([
            bracketMatcher(
                # set types, e.g., `[int]` or `[str]` and even
                # nested set types e.g., `[[int]]` or `[[[str]]]`.
                TypeElement
            )
            sequenceMatcher([
                # store types, e.g., `str[int]` or `dbl[str]`
                # as well as array types, e.g., `str[]` or `dbl[]`.
                # TODO: i don't think this works for more complicated types, e.g.,
                #       str[][int] which should be a store of `int` to `str[]` 
                LowerCamelCase
                bracketMatcher(
                    # present if we're a store type, absent if an array type:
                    optionalMatcher(TypeElement)
                )
            ])
            # simple type, e.g., `int` or `dbl` or `myClassType`
            LowerCamelCase
        ])
        RhsStatement: oneOfMatcher([
            AtomicStatement,
            sequenceMatcher([AtomicStatement, AnyOperator, RhsStatement]),
        ])
        AtomicStatement: oneOfMatcher([
            VariableName
            FunctionCall
            parenthesesMatcher(RhsStatement)
            listMatcher(DefinedArgument)
        ])
        ClassName: sequenceMatcher([
            LowerCamelCase
            optionalMatcher(TemplateArguments)
        ])
        ExtendParentClasses: sequenceMatcher([
            keywordMatcher("extend")
            listMatcher(ClassName)
        ])
        ClassDefinition: sequenceMatcher([
            ClassName
            oneOfMatcher([
                operatorMatcher(":")
                doNotAllow(operatorMatcher(";"), "Classes cannot be writable.")
                doNotAllow(operatorMatcher("?;"), "Classes cannot be nullable/writable.")
                doNotAllow(operatorMatcher("?:"), "Classes cannot be nullable.")
            ])
            optionalMatcher(ExtendParentClasses)
            parenthesesMatcher(repeatMatcher([ClassStatement], Until: operatorMatcher("}")))
        ])
        ClassStatement: oneOfMatcher([
            VariableDefinition
            VariableDeclaration
            FunctionDefinition
            FunctionDeclaration
            ClassMethod
        ])
        ClassMethod: sequenceMatcher([
            oneOfMatcher([
                operatorMatcher("::")
                operatorMatcher(";;")
                operatorMatcher(";:")
                operatorMatcher(":;")
            ]),
            oneOfMatcher([
                FunctionDefinition
                FunctionDeclaration
                # TODO: prefix ! and !!, as well as postfix !
            ])
        ])
        EndOfInput: tokenMatcher(
            ::match(Index;, Array: token[]): Index >= Array count()
        )
    ]

    ::match(Index;, Array: token[], GrammarMatcher): bool
        # ensure being able to restore the current token index if we don't match:
        Snapshot: Index
        Matched: what GrammarMatcher
            (TokenMatcher):
                TokenMatcher match(Index;, Array)
            (GrammerElement):
                My Elements[GrammarElement] match(Index;, Array)
            (Token):
                Index < Array count() and Array[Index++] == Token
        if not Matched
            Index = Snapshot
        return Matched
}()

# TODO: actually compiling code will require going through the TokenMatchers
# in a specific order to avoid running through all options to see what fits.
# OR, maybe we can parse a statement into tokens, and then do a `what Statement`
# with case `RhsStatement` etc., where the hash only takes into account the sequence
# of tokens but not the actual token content.  we'd also need to ignore repeated fields,
# i.e., only count them once.
# TODO: support for labeling token matchers, e.g. "parentClassNames" and "classBlock"
# or probably don't need to label token matchers if we build the logic for transpilation
# inside of the grammar elements.

# a list encompasses things like (), (GrammarMatcher), (GrammarMatcher, GrammarMatcher), etc.,
# but also lists with newlines if properly tabbed.
listMatcher(GrammarMatcher): parenthesesMatcher(repeatMatcher([
    GrammarMatcher
    CommaOrBlockNewline
])

sequence: tokenMatcher {
    ;;renew(My Array; grammarMatcher[]): Null

    ::match(Index;, ToMatch Array: token[]): bool
        for GrammarMatcher: in My Array
            if not Grammar match(Index;, ToMatch Array, GrammarMatcher)
                return False
        return True
}

# TODO: add an argument to parentheses matcher for the type of parentheses, e.g., () [] {}
# TODO: make `block` a type of token as well.
parenthesesMatcher: tokenMatcher {
    ;;renew(My GrammarMatcher): Null

    ::match(Index;, Array: token[]): bool
        CurrentToken: Array[Index]
        if currentToken != parenthesesToken
            return False

        InternalIndex; index = 0
        PartialMatch: Grammar match(InternalIndex;, CurrentToken InternalTokens, My GrammarMatcher)
        if not PartialMatch
            return False

        # need to ensure that the full content was matched inside the parentheses:
        if InternalIndex < CurrentToken InternalTokens count()
            return False
        
        ++Index
        return True
}

# TODO: make this a function which returns either `repeatInterruptible` and `repeatTimes`
# this is essentially the definition for repeatInterruptible:
repeatMatcher: tokenMatcher {
    # until `Until` is found, checks matches through `Array` repeatedly.
    # note that `Until` can be found at any point in the array;
    # i.e., breaking out of the array early (after finding `Until`) still counts as a match.
    # if you need to ensure a non-breakable sequence is found before `Until`,
    # use the `sequence` token matcher inside `Array`.
    i(My Until: GrammarMatcher = EndOfInput, My Array: GrammarMatcher[]): i()
    ;;renew(My Until: GrammarMatcher = EndOfInput, My Array: GrammarMatcher[]): Null

    ::match(Index;, ToMatch Array: token[]): bool
        if Index >= Array count()
            return False

        while True
            # TODO: is there a better syntax here that doesn't put `X: x` in front of `in`?
            #       it'd be nice to avoid parentheses if necessary, although that does make it clear
            #       that we have reference-like abilities.
            #       maybe `in My Array, GrammarMatcher:` or `iterate My Array, GrammarMatcher:`
            #           or `with My Array, GrammarMatcher:`
            #       or `for GrammarMatcher:, in My Array`
            for GrammarMatcher: in My Array
                # always check the escape sequence, Until:
                if Grammar match(Index;, ToMatch Array, My Until)
                    return True
                # note that it's ok to call `Grammar` here despite
                # the self-referential nature of the grammar, because
                # this logic would essentially be written in a .cc file
                # whereas `Grammar` would be defined in the .h file.
                if not Grammar match(Index;, GrammarMatcher)
                    return False
}
```

TODO: support internationalization.  do we really require Upper/lower+CamelCase for variables/functions?
or is the syntax unambiguous enough to not need them?

# tokenizer

TODO

# transpiling

Every variable instance has two different storage layouts, one for "only type" and one for
"dynamic type."  "Only-type" variables require no extra memory to determine what type they are.
For example, an array of `i32` has some storage layout for the `array` type, but
each `i32` element has "only type" storage, which is 4 consecutive bytes, ensuring that the
array is packed tightly.  "Dynamic-type" variables include things like objects and instances
that could be one of many class types (e.g., a parent or a child class).  Because of this,
dynamic-type variables include a 64 bit field for their type at the start of the instance,
acting much like a vtable.  However, the type table includes more than just method pointers.

```
// C++ code
typedef u64 typeId;

struct variableType {
    string Name;
    typeId TypeId;
};

typedef array<variableType> argumentsType;

struct overloadType {
    typeId InstanceTypeId; // 0 if this is not a method overload
    argumentsType Input;
    argumentsType Output;
};

struct overload {
    typeId InstanceTypeId; // 0 if this is not a method overload
    argumentsType Input;
    argumentsType Output;

    void *FunctionPointer;
};

struct overloadMatcher {
    array<overload> Overloads;

    // TODO: can `reference` be a no-copy no-move type class?
    // TODO: can we make this an `arrayElementReference` under the hood with type erasure?
    constNullableReference<overload> match(const overloadType &OverloadType) const;
};

// C++ code: info for a type
struct typeInfo {
    typeId Id;
    string Name;

    // Class types have variables, methods, and class functions defined in here:
    array<variableType> Fields;

    // A function typeInfo should have nonempty Overloads:
    overloadMatcher OverloadMatcher;
};
```

TODO: storage for dynamic types, can we create wrapper classes with enough memory and
cast to them (i.e., without allocation)?  need to know the possible memory layouts beforehand,
i.e., all possible child classes.  since we should know
all imports ahead of time, however, we could just get the largest child implementation and use that.
(maybe check if one child uses a lot more memory than other siblings and push to a pointer.)
for scripts that extend a class, we might fit in as much as we can to the wrapper classes
memory but then use a pointer to the rest.  it's ok if scripts take a performance hit (pointer dereference).

TODO: a general purpose "part of your memory is here, part of your memory is there" class
almost sounds like virtual memory with mappings.  that should probably be non-standard/non-core.

TODO: discuss having all instance methods in some special virtual table, e.g., possibly 
with additional reflection information (for things like `@for method in mutators(myClass)`
macro code).
we also may need to have a fallback table for functions that are defined locally.
or ideally, we just rely on the global functions so we don't have to specify the vtable
(unless we're overriding things).

## object format

We'll use the following example hm-lang class and other functions for transpilation examples.

```
exampleClass: {
    A; f32
    B; f32

    ;;renew(My X: i32, My Y: i32):
        My A = X - Y
        My B = X + Y

    ::readonlyMethod(Z: i32): i32
        My X * My Y - Z

    ;;writableMethod(Q: f32): f32
        My A *= Q
        My B *= 1.0 / (1.0 + abs(Q))
        My A * My B
}

exampleFunction(X: i64, A: dbl): {Y: i64, B: dbl}
    {Y: X - 1, B: A * X}
```

## C API

```
// example_class.h
typedef struct example_class {
    float A;
    float B;
    int32_t X;
    int32_t Y;
}   example_class_t;

typedef struct example_class_renew_input_X_Y {
    int32_t X;
    int32_t Y;
}   example_class_renew_input_X_Y_t;

typedef struct example_class_readonly_method_input_Z_t {
    int32_t Z;
}   example_class_readonly_method_input_Z_t;

typedef struct example_class_writable_method_input_Q_t {
    float Q;
}   example_class_writable_method_input_Q_t;

void example_class_renew_X_Y(example_class_t *My, example_class_renew_input_X_Y_t input);
int32_t example_class_readonly_method_input_Z_output_i32(
    const example_class_t *My, example_class_readonly_method_input_Z_t input
);
float example_class_writable_method_input_Q_output_f32(
    example_class_t *My, example_class_writable_method_input_Q_t input
);

typedef struct example_function_input_A_X_t {
    double A;
    int64 X;
}   example_function_input_A_X_t;

typedef struct example_function_output_B_Y_t {
    double B;
    int64 Y;
}   example_function_output_B_Y_t;

example_function_output_B_Y_t example_function_input_A_X_output_B_Y(
    example_function_input_A_X_t input
);
```

# compiling

If your code compiles, we will also format it.

If there are any compile errors, the compiler will add some special
comments to the code that will be removed on next compile, e.g.,
`#@!$("there's a syntax problem here ^")`

## metaprogramming

TODO: we'd like to provide ways to define custom block functions like `if X $(...)`,
e.g., `whenever Q $(...)`.

# implementation

## global functions via class methods

```
# hm-lang
myClass: {
    ::readonlyMethod(Int): null
    ;;mutatingMethod(Int): null
}

# C++
void hm::user::readonlyMethod(readonlyRef<hm::myClass> MyClass, readonlyRef<bigInt> Int);
void hm::user::mutatingMethod(mutatingRef<hm::myClass> MyClass, readonlyRef<bigInt> Int);
```

## types specified locally or remotely

We'll want to support types (a `u64`) being declared remotely or locally.
E.g., when creating an array of `i64`, we don't want to take up room in the array
for the same `i64` type on each array element.  Conversely, if the array element type
has child classes, then we need to keep track of the type on each array element.
(We need to do this unless the array has an `@only` annotation on the internal type.)

TODO: more discussion
