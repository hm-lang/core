# general syntax

## variable and function names

Identifiers in hm-lang are very important.  The capitalization (or lack thereof)
of the first letter indicates whether the identifier is a variable or a function.
Since we think of functions as verb-like, they are `lowerCamelCase` identifiers, e.g.,
`makeToast` or `runMarathon`.  On the other hand, variables are names, and we think
of them as proper nouns (like names), e.g., `Sam` or `MaxArraySize`, so they are
`UpperCamelCase` identifiers.  Class names are `lowerCamelCase`, since they
act more functions than variables; e.g., you can convert one class instance into
another class's instance, like `int(MyNumberString)` which converts `MyNumberString`
(presumably a `string` type) into a big integer.

There are a few reserved keywords, like `if`, `else`, `with`, `return`, `assert`,
`throw`, `catch`, 
which are function-like but will consume the rest of the statement, even without parentheses.
E.g., `return X + 5` will return the value `(X + 5)` from the enclosing function, whereas
something like `sin X + 5` will give the value `sin(X) + 5` due to operator precedence.

Most ASCII symbols are not allowed inside identifiers, e.g., `*`, `/`, `&`, etc., but
most importantly in contrast to most other languages, underscores (`_`) are also not allowed.
In hm-lang, underscores are considered subscripts, which allow access to an array,
e.g., `Numbers_3` is equivalent to `Numbers[3]`, which gives the fourth element in the array
called `Numbers` (since arrays are zero-indexed).

TODO: also support unicode in identifiers.

## blocks

### tabs vs. spaces

Blocks of code are made out of lines at the same indent level; an indent is four spaces.
No more than 7 levels of indent are allowed, e.g., lines at 0, 4, 8, 12, 16, 20, 24 spaces.
If you need more indents, refactor your code.

### line continuations

Lines can be continued at a +2 indent from the originating line, though there are some
special rules here when parentheses are involved.  If an open parenthesis is encountered at
the end of the line, and the next lines are only at +1 indent, then we assume an array
is being constructed.  If any `:` or `;` are encountered in what otherwise might be a
line-continued array, we assume we're creating an object/map.  If the line after an open
parenthesis is at +2 indent, then we assume we are just continuing the line and not creating
an array.

```
SomeVariable := someVeryLongFunctionNameBecauseItIsGoodToBeSpecific(10)
        + 3             # indent at +2 ensures that 3 is added into SomeVariable.
        - OtherVariable # don't keep indenting +2, keep at +2 from original.

ArrayVariable := [
    # Array elements are at indent +1 from an open parenthesis, trailing commas optional:
    1
    2
    3
    4
]

ObjectVariable := {
    SomeValue: 100
    OtherValue: "hi"
}

NotAnArray := (
        Continuing + The + Line + AtPlus2Indent - (
                Nested * Parentheses / Are + Ok
                - Too
        )
)
```

Note that the close parenthesis must be at the same indent as the line of the open parenthesis.
The starting indent of the line is what matters, so a close parenthesis can be on the same
line as an open parenthesis.

If an operator (e.g., `*`, `&&`, `+`, `/=`, etc.) begins the line with +Y indent, with Y >= 1,
and the identifier which follows the operator is at +X indent (where X is greater than Y),
then the line's indent will be counted from that identifier, i.e., at +X indent, not the
operator's indent (+Y).  Here we have some +2 indent examples:

```
SomeLineContinuationExampleVariable :=
        OptionalExpressionExplicitlyAtPlusTwoIndent
    +   5 - someFunction(
                AnotherOptionalExpression
            +   NextVariable
            -   CanKeepGoing
            /   Indefinitely
        )

AnotherLineContinuationVariable := CanOptionallyStartUpHere
    +   OkToNotHaveAPreviousLineStartingAtPlusTwoIndent * (
                KeepGoingIfYouLike
            -   HoweverLong
        )
```

Note that line continuations must be at least +2 indent, but can be more if desired:

```
ExamplePlusThreeIndent
    :=      Hello
        +   World
        -   Continuing
```

Unless there are parentheses involved, all indents for subsequent line continuations
should be the same.

### block parentheses and commas

You can use `$(` ... `)` to define a block inline, or similarly with the other parentheses
(i.e., `$[` ... `]` and `${` ... `}`.  The parentheses block "operator" is grammatically
the same as a standard block, i.e., going to a new line and indenting to +1.
This is useful for short `if` statements, e.g., `if SomeCondition $(doSomething())`.

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
    # NOTE: not recommended, since it's easy to accidentally skip reading
    # the statements that aren't first:
    print("toggling shutoff"), shutdown()

# block parentheses version
if SomeCondition $( print("toggling shutoff"), shutdown() )
```

If the block parentheses encapsulate content over multiple lines, note that
the additional lines need to be tabbed to +1 indent to match the +1 indent given by `$(`.
Multiline block parentheses are useful if you want to clearly delineate where your blocks
begin and end, which helps some editors navigate more quickly to the beginning/end of the block.
TODO: probably want a compile error if there's content after `$(` on a multiline block,
since it looks cleaner.

```
# multiline block parentheses via `$(`, though this is not required.
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
character that is not `{`, `[`, or `(`.  All characters after an EOL comment are
ignored; the compiler will start parsing on the next line.  A mid-line comment begins 
with `#(`, `#[`, or `#{`, followed by any character that is not a `#`, and ends with
the corresponding parenthesis and hash symbol, `)#`, `]#`, or `}#`, *on the same line*.
All characters within the parentheses are ignored by the compiler.  Multiline comments
begin with `#(#`, `#[#`, or `#{#`, and end with the corresponding `#)#`, `#]#`, or `#}#`
*at the same tab indent*.  This means you can have nested multiline comments, as long as
the nested symbols are at a new tab stop, and they can even be broken (e.g., an unmatched
closing operator `#)#` as long as it is indented from the symbols which started the
multiline comment), although this is not recommended.  To qualify as a multiline comment
(either to open or close the comment), nothing else is allowed on the line before or after
(besides spaces), otherwise an error is thrown.  All characters on all lines in between
the multiline comment symbols (e.g., `#(#` to `#)#`) are ignored.

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
* `u128` : unsigned integer which can hold values from 0 to 2^128 - 1, inclusive
* `u256` : unsigned integer which can hold values from 0 to 2^256 - 1, inclusive
* `u512` : unsigned integer which can hold values from 0 to 2^512 - 1, inclusive
* `count` : `i64` under the hood, intended to be >= 0 to indicate the amount of something.
* `index` : signed integer, `i64` under the hood.  for indexing arrays starting at 0.
* `ordinal` : signed integer, `i64` under the hood.  for indexing arrays starting at 1.

and similarly for `i8` to `i512`, using two's complement.  For example,
`i8` runs from -128 to 127, inclusive, and `u8(i8(-1))` equals `255`.

## casting between types

For the most type, hm-lang attempts to make casting between types simple and safe.
However, there are times where it's better to be explicit about the program's intention.
For example, if you are converting between two numbers, but the number is *not*
representable in the other type, the run-time will throw an error.  Therefore you'll
need to be explicit about rounding when casting floating point numbers to integers,
unless you are sure that the floating point number is an integer.  Even if the float
is an integer, the maximum floating point integer is larger than most fixed-width integer
types (e.g., `u32` or `i64`), so errors can be thrown in that case.  The big-integer type
`int` will not have this latter issue, but may throw depending on memory constraints.

```
# Going from a floating point number to an integer should be done carefully...
X: dbl = 5.43
Q: int = X                      # RUN-TIME ERROR, throws since `X` is not representable as an integer
Y: int = X round Down           # Y = 5.  equivalent to `X floor()`
Z: int = X round Up             # Z = 6.  equivalent to `X ceil()`.
W: int = X round Stochastically # W = 5 with probability 57%, or 6 with probability 43%.
R: int = X round()              # R = 5.  rounds to closest integer, breaking ties at half
                                #         to the integer larger in magnitude.

# Note, representable issues arise for conversions even between different integer types.
A: u32 = 1234
Q: u8 = A               # RUN-TIME ERROR, `A` is not representable as a `u8`.
B: u8 = A & 255         # OK, communicates intent and puts `A` into the correct range for a `u8`.
C: u8 = A clamp(0, 255) # OK, also communicates intent
```

TODO: casting to a complex type, e.g., `(int|str)(SomeValue)` will pass through `SomeValue`
if it is an `int` or a `str`, otherwise try `int(SomeValue)` if that is allowed, and finally
`str(SomeValue)` if that is allowed.  If none of the above are allowed, the compiler will
throw an error.

## types of types

Every variable has a reflexive type which describes the object/primitive that is held
in the variable.  This can be accessed via the `::Is` field on instances, and the `->Is`
field on functions/classes.  If comparing against a known type, i.e., to check if some
variable is an integer, it is highly recommended to use the `::is` method, e.g., 

```
X; dbl|int = 4
...
if X is(int)
    print("X is an integer")
else if X is(dbl)
    print("X is a double")
```

TODO: maybe we don't want `X Is` syntax since it looks like not-a-function, when
`X is` does, and it is.
This reads a little better than something like `if X Is == int`, but the latter also is
valid hm-lang (and does what you expect).  Some more examples:

```
vector3 := {X; dbl, Y; dbl, Z; dbl}

Vector3 := vector3(X: 1.2, Y: -1.4, Z: 1.6)

print(Vector3::Is)                      # prints `is->vector3`
print(vector3->Is == Vector3::Is)       # this prints true

VectorIs: is = Vector3::Is

# things can get recursive:
print(VectorIs::Is)         # type of a type, resolves to `is->is->vector3`.

# creates a dynamical instance of vector3; the compiler doesn't know it's a vector3.
# even though we know it comes from a vector3, `VectorIs` is generically an `is`.
# TODO: probably can make the compiler a little bit smart about this, based on the
# assignments that could be possibly made to VectorIs.  in this case, it's const,
# with no other assignment options besides Vector3::Is, so it must be a vector3 type.
AnotherVector3 := VectorIs new(X: 5, Y: 6, Z: -7)
```

TODO: flesh out `is` class a bit more.
```
is~t := {
    # Type instances have a `new` method which allows you to construct an
    # instance of the class.
    ::new(...Args): t
}
```

TODO: types of functions, shouldn't really have `new`.

# operators and precedence

Operator priority.

TODO: add : , ; ?? postfix/prefix ?
TODO: add `as`.  `X as Y` is equivalent to `Y: X` but `as` binds less strongly than the type `:`/`;`
declaration, so that we can do things like `X: int as Y` for output destructuring which will declare
`X` as an `int` variable and then be used for the return value of `Y` from the function.

| Precedence| Operator  | Name                      | Type/Usage        | Associativity |
|:---------:|:---------:|:--------------------------|:-----------------:|:-------------:|
|   0       |   `()`    | parentheses               | grouping: `(A)`   | ??            |
|           |   `[]`    | parentheses               | grouping: `[A]`   |               |
|           |   `{}`    | parentheses               | grouping: `{A}`   |               |
|           | `\\x/y/z` | library module import     | special: `\\a/b`  |               |
|           | `\/x/y/z` | relative module import    | special: `\/a/b`  |               |
|           |   `::`    | const prefix              | unary:  `::A`     |               |
|           |   `;;`    | mutable prefix            | unary:  `;;A`     |               |
|           |   `;:`    | mutability template       | unary:  `;:A`     |               |
|   1       |   ` `     | function call             | on fn: `a B`      | RTL           |
|   2       |   `::`    | impure read scope         | binary: `A::B`    | LTR           |
|           |   `;;`    | impure read/write scope   | binary: `A;;B`    |               |
|           |   `:>`    | read-only class variable  | binary: `A:>B`    |               |
|           |   `;>`    | writeable class variable  | binary: `A;>B`    |               |
|           |   `->`    | new output argument name  | unary:  `->A`     |               |
|           |   `->`    | new namespace/class scope | binary: `A->B`    |               |
|           |   ` `     | implicit member access    | binary: `A B`     |               |
|           |   `_`     | subscript/index/key       | binary: `A_B`     |               |
|           |   ` `     | implicit subscript        | binary: `A (B)`   |               |
|           |   `!`     | postfix moot = move+reset | unary:  `A!`      |               |
|   3       |   `^`     | superscript/power         | binary: `A^B`     | RTL           |
|           |   `**`    | also superscript/power    | binary: `A**B`    |               |
|           |   `--`    | unary decrement           | unary:  `--A`     |               |
|           |   `++`    | unary increment           | unary:  `++A`     |               |
|           |   `~`     | template/generic scope    | binary: `a~b`     |               |
|   4       |   `<>`    | bitwise flip              | unary:  `<>A`     | RTL           |
|           |   `-`     | unary minus               | unary:  `-A`      |               |
|           |   `+`     | unary plus                | unary:  `+A`      |               |
|           |   `!`     | prefix boolean not        | unary:  `!A`      |               |
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
|   8       |   `==`    | equality                  | binary: `A==B`    | LTR           |
|           |   `!=`    | inequality                | binary: `A!=B`    |               |
|   9       |   `&&`    | logical AND               | binary: `A&&B`    | LTR           |
|           |  `\|\|`   | logical OR                | binary: `A\|\|B`  |               |
|           |   `??`    | nullish OR                | binary: `A??B`    |               |
|   10      | `$(   )`  | block parentheses         | grouping: `$(A)`  | ??            |
|           | `$[   ]`  | block parentheses         | grouping: `$[A]`  |               |
|           | `${   }`  | block parentheses         | grouping: `${A}`  |               |
|   11      |   `=`     | assignment                | binary: `A = B`   | LTR           |
|           |  `???=`   | compound assignment       | binary: `A += B`  |               |
|           |   `<->`   | swap                      | binary: `A <-> B` |               |
|   12      |   `,`     | comma                     | binary/postfix    | LTR           |


TODO: discussion on and/not/or.  those should all be keywords/overloads for `&&/prefix !/||`.
TODO: discussion on `~`
needs to be RTL for `array~array~int` processing as `array~(array~int)`, etc.

## function calls

Function calls are assumed whenever a function identifier (i.e., `lowerCamelCase`)
occurs before an atomic expression.  E.g., `print X` where `X` is a variable name or other
primitive constant (like `5`), or, using parentheses, `anyFunctionName(Any + Expression / Here)`.
Function calls bind strongly, so that `sin X + 3` is equivalent to `(sin(X)) + 3`
and `tan Y * 3` is equivalent to `(tan(Y)) * 3`, as well as `sin X^2` being equivalent
to `(sin(X))^2`.  It is highly recommended (possibly to be enforced by the compiler)
to write expressions the other way around, e.g., `3 * tan Y`, `3 + sin X`, and
`sin(X)^2` so that the order of operations is more clear to developers.  Multiple function
calls are associated right-to-left, so `sin cos tan X` is equivalent to `sin(cos(tan(X)))`.

Note that methods defined on classes can be prefaced by member access (e.g., ` `, `::`, or `;;`),
and that path will still be used for function specification, even though member access has
less precedence.  So for example, if some instance `MyClassInstance` has a field `MyField`
which itself has a method `print`, then `MyClassInstance::MyField::print("Carrots")` 
(or `MyClassInstance MyField print "Carrots"`) will use the `print` function specified
on `MyField` rather than the global `print` function, even though the function call has
higher precedence than member access.  E.g., the compiler sees
`MyClassInstance::MyField::(print "Carrots")`, but knows to use
`MyClassInstance::MyField::print` when running the internal `(print "Carrots")`.

It is recommended to use parentheses where possible, to help people see the flow more easily.
E.g., `someFunction SomeInstance SomeField someMethod() FinalField` looks pretty complicated.
This would compile as `(someFunction(SomeInstance)::SomeField::someMethod())::FinalField`,
and including these parentheses would help others follow the flow.

Deep dive!  Function calls are higher priority than member access because `someMethod() FinalField`
would compile as `someMethod( ()::FinalField )` otherwise, which would evaluate to `someMethod(Null)`
since `()::FinalField` is accessing the `FinalField` field on the object `()`, for which there is
no field value (i.e, a null field value).  In addition, `X_someFunction 3` would compile as
`(X_someFunction)[3]`, which is almost certainly not what is desired, unless `X` is a map with
*function* keys.

## new-namespace `->` or class-scope operators `->`, `:>`, and `;>`

The operator `->` can be used in three ways: (1) as a unary operator for an output-only argument
name in a function declaration or function call, (2) for creating default-named variables in a
new/existing namespace, and (3) as an indicator for class scoping for static/class functions.

We'll discuss more in the function-call section, but here are some examples of (1):

```
# function declaration + definition, uses `Output` as an output variable via `->Output`.
myFunction(Input: int, ->Output; str):
    Output = str(Input)

# function calling:
Output := myFunction(Input: 301)  # Output = "301"
# equivalently, defines an immutable `Output` in this scope:
myFunction(Input: 301, ->Output: str)

# argument renaming is also possible; this defines `MyOutput` in this scope:
myFunction(Input: 445, ->MyOutput: str as Output)
```

Examples of (2):
`New->Int`, `Old->String`, `Input->Array`, or `Output->Rune`, where the LHS is the namespace
and the RHS is the variable name (typically the `UpperCamelCase` version of the `lowerCamelCase` type).
This allows you to pass in default-named arguments into functions, while avoiding argument/variable
name shadowing, which is not allowed in hm-lang.  For example:

```
someFunction(Input->Index): null
    # `Input->Index` is a default-named variable of type `index`, but we refer to it
    # within this scope using `Input->Index`.
    even(Index): bool
        return Index % 2 == 0
    # you can define other namespaces inline as well, like `Another` here:
    for Another->Index: index < Input->Index
        if even(Another->Index)
            print(Another->Index)
        
X: index = 100
someFunction(X)     # note that we don't need to call as `someFunction(Index: X)`
```

You can use the same namespace for multiple variables, e.g., `Input->Rune` and `Input->String`,
as long as the variable names don't overlap.  You can also use the namespace operator inside
of functions to declare new variables, but its utility is mostly to avoid argument renaming.
Like the member access operators below, the namespace operator binds left to right.

For the final way (3), we use `->` for class functions, and `:>` and `;>` work here as well but for
impure class functions.  Class functions are like "static" class methods in C++, and here
are some examples:

```
exampleClass := {
    # this pure function does not require an instance, and cannot use instance variables:
    ->someStaticFunction(Y; int): int
        Y /= 2
        return Y!

    # this function does not require an instance, and cannot use instance variables,
    # but it can read (but not write) global variables (or other files) due to `:>`:
    :>someStaticImpureFunction(): int
        YString := read(File: "Y")
        return int(?YString) ?? 7

    # this function does not require an instance, and cannot use instance variables,
    # but it can read/write global variables (or other files) due to `;>`:
    ;>someStaticImpureFunctionWithSideEffects(Y: int): null
        write(Y, File: "Y")
}
```

See the classes section for more clarification and comparison to member access operators.

## member access operators `::`, `;;`, and ` ` as well as subscripts `_`

We use `::`, `;;`, and ` ` (member access) for accessing variables or functions that belong to
another object.  The `::` operator ensures that the RHS operand is read only, not write,
so that both LHS and RHS variables remain constant.  Oppositely, the `;;` scope operator passes
the RHS operand as writable, and therefore cannot be used if the LHS variable is immutable.
The implicit member access operator ` ` is equivalent to `::` when the LHS is an immutable variable
and `;;` when the LHS is a mutable variable.

Subscripts `_` are not an allowed character for identifiers (e.g., variable or function names);
a subscript acts as an operator for indexing containers like arrays, sets, maps, and tensors.
E.g., for a map `Database; int_str`, then `Database_"Fred" = 3` sets the value at key "Fred" to 3.
Or if we declare an array `Array; int_`, then `Array_3 = 100` sets the fourth value of the array
to 100 (arrays are zero-indexed), so that `Array == [0, 0, 0, 100]`.  When an expression
is used to key the index, you may also use an implicit subscript, which occurs when we have
a non-function LHS followed by parentheses, e.g., `Array[Some + Expression / Here]`.
Some examples:

```
someClass := {X: dbl, Y: dbl, I; str_}
SomeClass ;= someClass(X: 1, Y: 2.3, I: ["hello", "world"])
print(SomeClass::I)     # equivalent to `print(SomeClass I)`.  prints ["hello", "world"]
print(SomeClass::I_1)   # prints "world"
print(SomeClass I_1)    # also prints "world", ` ` (member access) and `_` bind LTR.
SomeClass;;I_4 = "love" # the fifth element is love.
SomeClass::I_7 = "oops" # COMPILE ERROR, `::` means the array should be immutable.
SomeClass;;I[7] = "no problem"
```

For class methods, `;;` (`::`) selects the overload that mutates (keeps constant) the class
instance, respectively.  For example, the `array` class has overloads for sorting, (1) which
does not change the instance but returns a sorted copy of the array (`::sort(): this`), and
(2) one which sorts in place (`;;sort(): null`).  The ` ` (member access) operator will use
`::` if the LHS is an immutable variable or `;;` if the LHS is mutable.  Some examples in code:

```
# there are better ways to get a median, but just to showcase member access:
getMedianSlow(Array: array~int): int
    if Array size() == 0
        throw "no elements in array, can't get median."
    # make a copy of the array, but no longer allow access to it (via `@hide`):
    Sorted->Array := @hide Array sort()   # same as `Array::sort()` since `Array` is immutable.
    # note that we need parentheses here around the desired array index
    # since `//` binds less strongly than `_`:
    return Sorted->Array[Sorted->Array size() // 2]

# sorts the array and returns the median.
getMedianSlow(Array; array~int): int
    if Array size() == 0
        throw "no elements in array, can't get median."
    Array sort()    # same as `Array;;sort()` since `Array` is mutable.
    return Array _ (Array size() // 2)
```

Note that if the LHS is immutable, you will not be able to use a `;;` method.
To sum up, if the LHS is mutable, you can use `;;` or `::`, and ` ` (member access) will
effectively be `;;`.  If the LHS is immutable, you can only use `::` and ` `, which are equivalent.

Subscripts have the same binding strength as member access operators since they are conceptually
similar operations.  This allows for operations like `++A_3` meaning `++(A_3)` and
`--A B C_3` equivalent to `--(((A;;B);;C)_3)`.  Member access binds stronger than exponentation
so that operations like `A B_C^3` mean `((A::B)_C)^3`.  Note that parentheses break up implicit
member access, so that we can distinguish between implicit member access, e.g., `Object Field`,
and implicit subscript, e.g., `Array(Index)`.  The former becomes `Object::Field` (or
`Object;;Field` depending on the context), while the latter becomes `Array_Index`.

Note that `something() NestedField` becomes `(something())::NestedField` due to
the function call having higher precedence.  (You can also use destructuring if you want
to keep a variable for multiple uses: `{NestedField} := something()`.)
Similarly, using a function on the RHS of a subscript operation will call the function
since it has higher priority.  E.g., `Array_someFunction 3` becomes `Array_(someFunction(3))`.

## prefix and postfix exclamation points `!`

The operator `!` is always unary (except when combined with equals for not equals,
e.g., `!=`).  It can act as a prefix operator "not", e.g., `!A`, pronounced "not A",
or a postfix operator on a variable, e.g., `Z!`, pronounced "Z mooted".  In the first
example, prefix `!` calls the `::!This: bool` method defined on `A`, which creates a
temporary value of the boolean opposite of `A` without modifying `A`.  In the second
case, it calls a built-in method on `Z`, which moves the current data out of `Z` into
a temporary instance of whatever type `Z` is, and resets `Z` to a blank/default state.
This is a "move and reset" operation, or "moot" for short.  Overloads for prefix `!`
should follow the rule that, after e.g., `Z!`, checking whether `Z` evaluates to false,
i.e., by `!Z`, should return true.

Note, it's easier to think about positive boolean actions sometimes than negatives,
so we allow defining either `::!!This: bool` or `::!This: bool` on a class, the former
allowing you to cast a value, e.g., `A`, to its positive boolean form `!!A`, pronounced
"not not A."  Note, you cannot define both `!` and `!!` overloads for a class, since
that would make things like `!!!` ambiguous.

## superscripts/exponentiation

Note that exponentiation -- `^` and `**` which are equivalent --
binds less strongly than function calls and member access.  So something like `sin X^2` will be
equivalent to `(sin(X))^2`).  It is more clear to use the following syntax:
use `anyFunction(AnyExpression)^Power`, which is equivalent to `(anyFunction(AnyExpression))^Power`.

## bitshifts `<<` and `>>`

The notation `A << B`, called "bitshift left", means to multiply `A` by `2^B`.  For example, 
`A << 1 == A * 2`, `A << 2 == A * 4`, and `A << 3 == A * 8`.  Conversely, "bitshift right"
`A >> B` means to divide `A` by `2^B`.  Typically, we use bitshifts `<<` and `>>`
only for fixed-width integers, so that `A >> 5 == A // 32`, but there are overloads
for other types that will do the expected full division.  For floats, e.g., 16.0 >> 5 == 0.5.
Note that `A << 0 == A >> 0 == A`, and that negating the second operand is the same
as switching the operation, i.e., `A << B == A >> -B`.

Bitshifts have a higher precedence than multiplication and division because they are
"stronger" operations: `100 << 3 == 800` whereas `100 * 3 == 300`, and the difference widens
as the second operand increases; similarly for division, bitshift right `>>` is "stronger"
than division at reducing the first operand via the second operand.
Thus `7 * 31 >> 3` groups as `7 * (31 >> 3) == 21` (and not as `(7 * 31) >> 3 == 27`),
and `105 // 5 << 2` groups as `105 // (5 << 2) == 5` and not `(105 // 5) << 2 == 84`.

Looking the other direction, bitshifts have lower precedence than exponentiation because
exponents are generally stronger -- as long as the first operand, the base, of the exponentiation is
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
so `Q := X < Y > Z` instantiates `Q` as a boolean, not as this internal class.

## and/or/xor operators

The `or` operation `X || Y` technically has type `X is | Y is`.  If `X` evalutes to
truthy (i.e., `!!X == True`), then the return value of `X || Y` will be `X`.
Otherwise, if `X` evaluates to falsy (i.e., `!!X == False`), then the return value
of `X || Y` will be `Y`.  This allows for JavaScript syntax like `(X || Y) toString()`.
Note in a conditional, e.g., `if X || Y`, we'll always cast to boolean implicitly
(i.e., `if bool(X || Y)` explicitly).
TODO: consider doing `X is | Y is | null` and checking if `Y` is truthy before returning it.
this might be less suprising than the javascript behavior.

Similarly, the `and` operation `X && Y` has type `X is | Y is`.  If `X` evaluates to
truthy (i.e., `!!X == True`), then the return value of `X && Y` will be `Y`.
Otherwise, if `X` evaluates to falsy (i.e., `!!X == False`), then the return value
of `X && Y` will be `X`.  Again, in a conditional, we'll cast `X && Y` to a boolean,
which will be truthy if and only if `!!(X && Y)`.
TODO: consider doing `X is | Y is | null` and checking if `Y` is truthy before returning `X`.
this might be less suprising than the javascript behavior.

The `xor` operation `X >< Y` has type `X is | Y is | null`, and will return `Null`
if both `X` and `Y` are truthy or if they are both falsy.  If just one of the operands
is truthy, the result will be the truthy operand.  An example implementation:

```
xor(X: ~x, Y: ~y): x|y|null
    XIsTrue := bool(X)
    YIsTrue := bool(Y)
    if (XIsTrue && YIsTrue) || (!XIsTrue && !YIsTrue)
        return Null
    if XIsTrue
        return X
    else
        return Y
```

## assignment operators

TODO: `??=`.
TODO: discussion on `<->` being swap.  swap as a function would have required `swap(X;, Y;)`
to be consistent.

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

Y = 123     # COMPILER ERROR, Y is immutable and thus non-reassignable.
Y += 3      # COMPILER ERROR, Y is immutable and thus deeply constant.
```

Mutable/reassignable/non-constant variables can use `VariableName = Expression`
after their first initialization, but they must be declared with a `;` symbol.

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

Note that we use `;` and `:` as if it were an annotation on the variable name (rather
than the type) so that we don't have to worry about complex types like a mutable
array of a constant integer.  Constant variables are deeply constant, and mutable
variables are modifiable/reassignable, and we only have to think about this
(as programmers using the language) at the level of the variable itself,
not based on the type of the variable.  The underlying type is the same for both
constant and mutable variables (i.e., a mutable type), but the variable is only
allowed to mutate the memory if it is declared as a mutable variable with `;`.

## nullable variable types

To make it easy to indicate when a variable can be nullable, we reserve the question mark
symbol, `?`, placed after the variable name.  E.g., `X?: int` declares a variable `X` that
can be an integer or null.  The default value for an optional type is `Null`.
For an optional type with more than one non-null type, we use `Y?: someType|anotherType`.
In either case, you can use `;` instead of `:` to indicate that the variable is mutable.
Note that if you are defining the variable inline (e.g., with `:=` or `;=`), you should
prefix the operator with a `?`, e.g., `X ?:= nullableResult(...)`.  It is a compiler error
if a declared variable is nullable but `?` is not used, since we want the programmer to be
aware of the fact that the variable could be null, even though the program will take care
of null checks automatically and safely.

TODO: discussion here.
Note that different overloads may be used for `X ?:= myFunction(...)` vs. `X := myFunction(...)`,
but it might be nice to not write boiler-plate code (e.g., `if X == Null ${throw error("...")}`)
everywhere.  would we allow `X := myFunction` but throw a run-time error if X is null?
maybe we need an explicit run-time check, `X := myFunction(...) ?? throw`.

TODO: double check all nullable variable boolean operations.  we maybe should ensure
checking against `X != Null` first, then any subsequent checks, e.g., `X != 0`.

One of the cool features of hm-lang is that we don't require the programmer
to check for null on a nullable type before using it.  The executable will automatically
check for null on variables that can be null.  If a function can be null, the executable
will return null if the function is null, or otherwise execute the function.
This is also helpful for method chaining on classes (see more on those below).
If your code calls a method on an instance that is null, a null will be
returned instead (and the method will not be called).

```
# define a class with a method called `someMethod`:
someClass := { ::someMethod(): int }

Nullable?; someClass = Null

Value ?:= Nullable someMethod()  # `Value` has type `int|null` now, so it needs to be defined with `?`

# eventually we want to support things like this, where the compiler
# can tell if the type is nullable or not:
if Nullable != Null
    NonNullValue := Nullable someMethod()   # `NonNullValue` here must be `int`.
```

It is not allowed to implicitly cast from a nullable type to a non-nullable type,
e.g., `Value := Nullable someMethod()`.  The compiler will require that we define
`Value` with `?:=`, or that we explicitly cast via whatever ending type we desire,
e.g., `Value := int(Nullable someMethod())`.  Note that `whateverType(Null)` is
the same as `whateverType()`, and number types (e.g., `int()` or `flt()`)  default
to 0.

Optional functions are defined in a similar way (cf. section on nullable functions),
with the `?` just after the function name, e.g., `someFunction?(...Args): returnType`.

TODO: how do we declare a function of multiple types?  e.g.,
`someFunction: ((Dbl): string) | ((Dbl, Name: string): int)`.
This might have some usage for function overloads.  Maybe we disallow these and require
declare the overloads individually.

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

# to make an object variable immutable, use := when defining:
Vector2 := (X: 3.75, Y: 3.25)
# or you can use `:` with an explicit type specifier and then `=`:
Vector2: (X: dbl, Y: dbl) = (X: 3.75, Y: 3.25)
# then these operations are invalid:
Vector2 X += 3          # COMPILER ERROR, variable is immutable, field cannot be modified
Vector2 = (X: 1, Y: 2)  # COMPILER ERROR, variable is immutable, cannot be reassigned
```

You can define a type/interface for objects you use multiple times.

```
# a plain-old-data class with 3 non-reassignable fields, X, Y, Z:
vector3 := (X: dbl, Y: dbl, Z: dbl)

# you can use `vector2` now like any other type, e.g.:
Vector3 := vector3(X: 5, Y: 10)
```

We also allow type definitions with mutable fields, e.g. `(X; int, Y; dbl)`.
Depending on how the variable is defined, however, you may not be able to change
the fields once they are set.  If you define the variable with `;`, then you
can reassign the variable or modify the mutable fields.  But if you define the
variable with `:`, the object is deeply constant, regardless of the field definitions.

```
# mixMatch has one mutable field and one immutable field:
mixMatch := (Mut; dbl, Imm: dbl)

# when defined with `;`, the object is mutable and reassignable.
MutableMix; mixMatch = (Mut: 3, Imm: 4)
MutableMix = mixMatch(X: 6, Y: 3)   # OK, MutableMix is mutable and thus reassignable
MutableMix Mut += 4                 # OK, MutableMix is mutable and this field is mutable
MutableMix Imm -= 1                 # COMPILE ERROR, MutableMix is mutable but this field is immutable
                                    # if you want to modify the `Imm` field, you need to reassign
                                    # the variable completely.

# when defined with `:`, the object is deeply constant, so its fields cannot be changed:
ImmutableMix: mixMatch = (X: 5, Y: 3)
ImmutableMix = mixMatch(X: 6, Y: 3) # COMPILE ERROR, ImmutableMix is immutable, thus non-reassignable
ImmutableMix Mut += 4               # COMPILE ERROR, ImmutableMix is immutable, thus deeply constant
ImmutableMix Imm -= 1               # COMPILE ERROR, ImmutableMix and this field are immutable

# NOTE that in general, calling a function with variables defined by `;` is a different
# overload than calling with `:`.  Mutable argument variables imply that the arguments are
# being passed by reference and will be mutated inside the function.  Data classes do not have
# these overloads, so they will throw compile errors:
MyMixMatch := mixMatch(X; 5, Y; 3)  # COMPILE ERROR: mixMatch has no overloads for `(X;, Y;)`
# see section on mutable/immutable arguments for more information.
```

### automatic deep nesting

We can create deeply nested objects by adding valid identifiers with consecutive `:`.  E.g.,
`(X: Y: 3)` is the same as `{X: {Y: 3}}`.

TODO: this might make overloads more complicated; how do we choose the overload if we are calling
with e.g., `{X: {Y: 3}}` vs. `{X: {Y: 3, Z: 4}}`.

## temporarily locking mutable variables

You can also make a variable non-reassignable and deeply constant
for the remainder of the current block by using `@lock` before the variable name.
Note that you can modify it one last time with the `@lock` annotation, if desired.

```
X; int = 4  # defined as mutable and reassignable

if SomeCondition
    @lock X = 7 # locks X after assigning it to the value of 7.
                # For the remainder of this indented block, you can use X but not reassign it.
                # You also can't use mutable, i.e., non-const, methods on X.
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
DateString; str = "2023-01-01"

# after this line, `DateString` can't be accessed anymore.
Date := date(@hide DateString)

# note in some circumstances you may also want to include `!` to avoid copying the variable,
# if the underlying class makes use of that same type variable internally, e.g.:
Date := date(@hide DateString!)
# see discussion on `moot` for more information.
```

# functions

Functions are named using `lowerCamelCase` identifiers.
 ```
# declaring a function with no arguments that returns a big integer
v(): int

# setting/defining/initializing the function usually requires an indent+1 block following:
v(): int
    return 600

# but in simple cases like this you can also define inline:
v() := 600

# inline, but with explicit type
v(): int = 600

# inline with explicit type on the return value
v() := int(600)

# function with X,Y double-precision float arguments that returns nothing
v(X: dbl, Y: dbl): null
    print("X = ${X}, Y = ${Y}, atan(Y, X) = ${\\math atan(X, Y)}")
    # Note this could also be defined more concisely using $$,
    # which also prints the expression inside the parentheses with an equal sign and its value,
    # although this will print `\\math atan(X, Y) = ...`, e.g.:
    # print("$${X}, $${Y}, $${\\math atan(X, Y)}")

# Note that it is also ok to use parentheses around a function definition,
# but you should use the block parentheses notation `$(`.
# TODO: see if we need to use `$(` or can get away with `(`.
#       this looks a bit like `str[...]` which is ok if we can determine that `...`
#       is a block to be executed sequentially.  but then we'd need `:= str[...]`
#           excite(Times: int): str {
#               return "hi!" * Times
#           }
#       maybe we allow it because we have classes defined without `${`.
excite(Times: int): str ${
    return "hi!" * Times
}

# You can also define functions inline with the `$(` ... `)` block operator.

oh(Really; dbl): dbl ${ Really *= 2.5, return 50 + Really }
```

## calling a function

You can call functions with arguments in any order.  Arguments must be specified
with the named identifiers in the function definition.  The only exception is
if the argument is default-named (i.e., it has the same name as the type), then you
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

### eliding parentheses for single-argument function calls

We also allow calling functions without parentheses for a single argument, like this:

```
print(String): null     # definition, using an default-named string, e.g., `String: string`.

# example calls:
print("Hello, world!")  # with parentheses, and without explicitly naming the variable
print "Hello, world!"   # without
```

To specify multiple arguments, one must use parentheses to avoid ambiguity.
However, if you are specifying a single, named argument, you can avoid using parentheses.

```
v(X: dbl, Int): null    # definition

v 100       # executes v(X: 0.0, Int: 100)
v X: 5      # executes v(X: dbl(5), Int: 0)

# COMPILE ERROR: need parentheses here, since commas bind less strongly than function-spaces:
v 100, X: 10
```

Note, even though `return` looks like a function (since it is `lowerCamelCase`), it behaves
as a control statement (like `if` or `for`).  So it doesn't need to follow these laws;
it consumes the remainder of the line for the return value.
E.g., `return f X + 3` is the same as `return(f(X) + 3)`.

### functions as arguments

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

### default-name arguments in functions

For functions with a single argument where the variable name doesn't matter,
you can use default-named variables.  For primitive types, the default-name identifier
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
the default-named argument for a function is `fn`.  When *declaring* a function
with a function argument, the default name must be used (`fn`); when *defining*
a lambda function for use inside a function, the default name can be omitted.

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

# or you can create a default-named function yourself:
q(;;fn(): bool
    return random() > 0.5
)   # will print one of the above due to randomness.
# equivalent to `q(;;() := random() > 0.5)`

# when defining a lambda (not declaring it), you can omit the name:
q(() := True)

# or you can do multiline with a name-omitted lambda:
X; bool
q(::():
    return X
)
# equivalent to `q(::() := X)`
# also equivalent to `q(::(): ${X})`
```

### constant versus mutable arguments

Functions can be defined with mutable or immutable arguments, e.g., via
`MutableArgument; mutableType` and `ImmutableArgument: immutableType` in the
arguments list.  This choice has two important effects: (1) you can modify
mutable argument variables inside the function definition and (2) any
modifications to a mutable argument variable inside the function block persist
in the outer scope.  In C++ terms, arguments declared as `:` are passed as
constant reference, while arguments declared as `;` are passed as reference
(or as temporaries).  Overloads can be defined for both, since it is clear which
is desired based on the caller using `:` or `;`.  Some examples:

```
# this function passes by reference and will modify the external variable
check(Arg123; string): string
    Arg123 += "!!??"    # OK since Arg123 is defined as mutable via `;`.
    print(Arg123)
    return Arg123

# this function passes by constant reference and won't allow modifications
check(Arg123: string): string
    print(Arg123)
    return Arg123 + "??!!"

MyValue; string = "immutable"
check(Arg123: MyValue)  # prints "immutable!!??"
print(MyValue)          # prints "immutable"
check(Arg123; MyValue)  # prints "immutable??!!"
print(MyValue)          # prints "immutable??!!"
```

Note that if you try to call a function with an immutable variable argument,
but there is no overload defined for it, this will be an error.  Similarly
for mutable variable arguments.

```
onlyImmutable(A: int): str
    return str(A) * A

MyA ;= 10
onlyImmutable(A; MyA)  # COMPILE ERROR, no mutable overload for `onlyImmutable(A;)`

print(onlyImmutable(A: 3))      # OK, prints "333"
print(onlyImmutable(A: MyA))    # OK, prints "10101010101010101010"

onlyMutable(B; int): str
    Result := str(B) * B
    B /= 2
    return Result

MyB ;= 10
onlyMutable(B: MyB)             # COMPILE ERROR, no immutable overload for `onlyMutable(B:)`
# same for something like `onlyMutable(B: 10)`.

print(onlyMutable(B; MyB))      # OK, prints "10101010101010101010"
print(onlyMutable(B; MyB))      # OK, prints "55555"
```

Note there is an important distinction between variables defined as mutable inside a block
versus inside a function argument list.  Mutable block variables are never reference types.
E.g., `B ;= A` is always a copy of `A`, so `B` is never a reference to the variable at `A`.
For a full example:

```
referenceThis(A; int): int
    B ;= A  # B is a copy of A
    A *= 2
    B *= 3
    return B

A ;= 10
print(referenceThis(A;))    # prints 30, not 60.
print(A)                    # A is now 20, not 60.
```

You are allowed to have default parameters for mutable arguments, and a suitable temporary
will be created for each function call so that a reference type is suitable.  In the same
way, you can call the mutable overload for a function with a temporary, e.g., via `B; 17`,
and the temporary variable will be discarded after the function call.

```
fn(B ;= int(3)): int
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
MyB ;= 10
print(fn(B; MyB))   # returns 13
print(MyB)          # MyB is now 13 as well.
print(fn(B; 17))    # MyB is unchanged, prints 20
```

Note that a mooted variable will automatically be considered a mutable argument
unless otherwise specified.  However, this will create a temporary and not modify
the variable in the external scope.

```
over(Load: int): str
    return str(Load)

over(Load; int): str
    return str(++Load)

Load ;= 100
print(over(Load!))  # calls `fn(Load; Load!)` with a temporary, prints 101
print(Load)         # Load = 0 because it was mooted, and was not modified inside the function

Load = 100
print(over(Load: Load!))    # calls `fn(Load)` with a const temporary, prints 100
print(Load)                 # Load = 0 because it was mooted

Load = 100
print(over(Load; Load!))    # calls `fn(Load;)` with a temporary, prints 101
print(Load)                 # Load = 0 because it was mooted

# for reference, without mooting:
Load = 100
print(over(Load;))          # calls `fn(Load;)` with the reference, prints 101
print(Load)                 # Load = 101 because it was passed by reference
```

The implementation in C++ might look something like this:

```
// these function overloads are defined for `fn(Str;)` and `fn(Str)`:
void fn(string &String);        // reference overload for `fn(Str;)`
void fn(string &&String);       // temporary overload for `fn(Str;)`
void fn(const string &String);  // constant reference just for `fn(Str:)`
```

The reason why we don't want to enable a third overload option for temporaries (i.e., `t &&T`)
is that the calling syntax would be confusing.  You might declare the function like this,
`over(Load! int): str`, but then calling `over(Load: ALoad!)` or `over(Load; BLoad!)`
would look like you're trying to call `over(Load:;)` respectively.  When you're not
trying to rename, `over(Load!)` looks fine, but `over(Load! ALoad!)` looks busy.
Instead, documentation should make it clear what the mutable argument is being mutated for;
is it (1) being modified for use outside the function, (2) being taken by the internal scope
(and thus mooted in the outside scope), or (3) being swapped with some other value in the
internal scope.  Cases (2) and (3) make more sense for class methods than standard functions,
as there is likely an instance variable that could be reset (case 2) or swapped out (case 3).
We could document the behavior using annotations, e.g., `@modifying`, `@taking`, or `@swapping`,
e.g., `over(@modifying Load; int)` in the above example.

If you want to define both overloads, you can use the template `;:` (or `:;`) declaration
syntax.  There will be some annotation/macros which can be used while before compiling,
e.g., `@mutable`/`@immutable` to determine if the variable is mutable or not.

```
myClass~t := {
    X; t

    ;;take(Other->X; t):
        X = Other->X!
    ;;take(Other->X: t):
        X = Other->X

    # maybe something like this?
    ;;take(Other->X;: t):
        X = @mootOrCopy Other->X
        # TODO: how complicated should the preprocessor be?  maybe `@if(@mutable Z, Z!, Z)` might be better.
        # `@mootOrCopy Z` can expand to `@if @mutable Z ${Z!} @else ${Z}`
        # or maybe we can do something like `X = Other->X!:`
}
```

Alternatively, we can rely on some boilerplate that the language will add for us, e.g.,

```
myClass~t := {
    X; t

    # these are added automatically by the compiler since `X; t` is defined.
    ;;x(T; t): ${ X<->T }
    ;;x(T: t): ${ X = T }

    # so `take` would become:
    ;;take(Other->X;: t):
        x(Other->X;:)
}
```

### nullable arguments

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
someFunction(Y: int): flt
    return 2.3 * flt(Y)

# nullable argument (case 3):
someFunction(Y?: int): string
    return "why not ${Y}"

# default argument (case 4):
someFunction(Y := 3): i64
    return i64(Y)
```

Note that mutable arguments `;` are distinct overloads, which indicate either mutating
the external variable, taking it, or swapping it with some other value, depending on
the behavior of the function.  Temporaries are also allowed, so defaults can be defined
for mutable arguments.

TODO: check if this breaks class instantiation assumptions.

What are some of the practical outcomes of these overloads?  Suppose
we define present and missing argument overloads in the following way:

```
overloaded(): dbl
    return 123.4
overloaded(Y: int): string
    return "hi ${Y}"
```

The behavior that we get when we call `overloaded` will depend on whether we
pass in a `Y` or not.  But if we pass in a null `Y`, then we also will end up
calling the overload that defined the missing argument case.  I.e.:

```
Y?; int = ... # Y is maybe null, maybe non-null

# the following calls `overloaded()` if Y is Null, otherwise `overloaded(Y)`:
Z := overloaded(Y?) # also OK, but not idiomatic: `Z := overloaded(Y?: Y)`
# Z has type `dbl|string` due to the different return types of the overloads.
```

The reason behind this behavior is that in hm-lang, an argument list is conceptually an object
with various fields, since an argument has a name (the field name) as well as a value (the field value).
An object with a field that is `Null` should not be distinguishable from an object that
does not have the field, since `Null` is the absence of a value.  Thus, if we count up
the number of fields in an object using `size()`, we'll get results like this:
`object()::size() == 0`, `{Y: Null}::size() == 0`, and `{Y: 5}::size() == 1`.

Note that when calling a function with a nullable variable/expression, we need to
indicate that the field is nullable if the expression itself is null (or nullable). 
Just like when we define nullable variables, we use `?:=` or `?;=`, we need to use
`?:` or `?;` (or some equivalent) when passing a nullable field.  For example:

```
someFunction(X?: int): int
    return X ?? 1000

# when argument is not null:
someFunction(X: 100)    # OK, expression for X is definitely not null
someFunction(X?: 100)   # ERROR! expression for X is definitely not null

# when argument is an existing variable:
X ?;= Null
print(someFunction(X?))  # can do `X?: X`, but that's not idiomatic.

# when argument is a new nullable expression:
someFunction(X?: someNullishFunction())  # REQUIRED since someNullishFunction can return a Null
someFunction(X: someNullishFunction())   # ERROR! someNullishFunction is nullable, need `X?:`.

# where someNullishFunction might look like this:
::someNullishFunction()?: int
    return if SomeCondition $( Null ) else $( 100 )
```

We also want to make it easy to chain function calls with variables that might be null,
where we actually don't want to call an overload of the function if the argument is null.

```
# in other languages, you might check for null before calling a function on a value.
# this is also valid hm-lang but it's not idiomatic:
X ?:= if Y != Null ${overloaded(Y)} else ${Null}

# instead, you should use the more idiomatic hm-lang version.
# putting a ? before the argument name will check that argument;
# if it is Null, the function will not be called and Null will be returned instead.
X ?:= overloaded(?Y)

# either way, X has type `string|null`.
```

You can use prefix `?` with multiple arguments; if any argument with prefix `?` is null,
then the function will not be called.

This can also be used with the `return` function to only return if the value is not null.

```
doSomething(X?: int): int
    Y ?:= ?X * 3    # Y is Null or X*3 if X is not Null.
    return ?Y       # only returns if Y is not Null
    #[ do some other stuff ]#
    ...
    return 3
```

### output arguments

Arguments can be declared as input-only, output-only, or both input and output (IO).
Input-only arguments are guaranteed not to change the value of any backing variable
in the outside scope.  They are passed essentially by constant reference (e.g., C++
`const t &`) but will be copied-on-write (COW) if they are modified inside the function.
Output-only arguments are just the return values of the function, i.e., the fields
of an output object.  These can be thought of as pointers from the caller's scope that
the function is allowed to modify the pointed-at value of, write-only (no read).  IO
arguments are of course readable and writeable variables from the caller's scope.
Here are some simple examples of input and output variables:
TODO: do we want Output and IO arguments to essentially be (special) pointers?
      We may want to re-write the MMR section for this.

```
# this function has an input variable `Input: int` and an output variable `Str; str`:
fn(Input: int): str
    return "hi" * Input
# alternative, equivalent definition:
fn(Input: int, ->Str; str):
    Str = "hi" * Input

# when calling the function, you can specifically declare/initialize the output variable
# via `->` with no prefix, like this:
fn(Input: 3, ->Str:)    # equivalent to `fn(Input: 3, ->Str: str)` due to default naming.
print(Str)              # prints "hihihi"

# you can make the newly initialized output variable mutable for later via an explicit `;`
fn(Input: 4, ->Str;)    # equivalent to `fn(Input: 4, ->Str; str)` due to default naming.
print(Str)              # prints "hihihihi"
Str += "yo"             # Str is now "hihihihiyo"

# alternatively, you can pre-define the output variable and then avoid using `;` or `:`.
Str ;= str("asdf")      # note it needs to be mutable.
fn(Input: 2, ->Str)     # note there is no `;` or `:` here.
print(Str)              # prints "hihi"

# note that you can also immediately define the output variable, as usual:
Str := fn(Input: 1)     # Str = "hi"
```

Here are some more complicated examples which include IO variables:

```
# === function definition ===========================
# TODO: do we keep this or require people to switch to `Io;`?
# this function rounds down a dbl and returns the integer and fractional parts via
# `RoundDown` and `Io` respectively:
fraction(In: string, Io: dbl): (Io: dbl, RoundDown: int)
    print(In)
    RoundDown := Io round(Down)
    return {Io: Io - RoundDown, RoundDown}

# equivalent definition using mutable reference syntax:
fraction(In: string, Io; dbl): (RoundDown: int)
    print(In)
    RoundDown := Io round(Down)
    Io -= RoundDown
    return {RoundDown}

# equivalent definition using mutable reference and output argument syntax:
fraction(In: string, Io; dbl, ->RoundDown; int):
    print(In)
    RoundDown = Io round(Down)
    Io -= RoundDown
# note that we can't have variable shadows, so this would throw a compile error:
# `fraction(In: str, Io: dbl, ->Io; dbl, ->RoundDown; int)`

# === function calling ==============================
# we can call with variables that get defined inline like this.
{Io: dbl, RoundDown: int} = fraction(In: "hello!", Io: 1.234)
# note `Io` and `RoundDown` are both immutable outside the function scope.
# you could make `Io` and `RoundDown` mutable via the following:
{Io; dbl, RoundDown; int} = fraction(In: "hello!", Io: 1.234)
RoundDown += 3  # OK, `RoundDown` is mutable.

# we can call with pre-existing variables, using MMR, like this:
In := "hello!"
Io ;= 1.234     # note `;` so it's mutable.
RoundDown; int
fraction(In, Io;, ->RoundDown)

# we can call with variables that get defined inline like this, besides `Io`, which is MMR.
Io ;= 1.234     # note `;` so it's mutable.
fraction(In: "hello!", Io;, ->RoundDown; int)

# This should not be supported: `fraction(In: "hello!", Io: 1.234, ->Io: dbl, ->RoundDown: int)`
# Because inside the function, we might have ambiguous expectations for how `Io` behaves
# if it is read after it is written inside the function.

# === calling the function with variable renaming ===
# with pre-existing variables, using MMR syntax:
Greeting := "hello!"
InputOutput ;= 1.234     # note `;` so it's mutable.
# just like when we define an argument for a function, the newly scoped variable goes on the left,
# so too for destructuring return arguments.  this one uses the default type of `RoundDown`:
{IntegerPart; RoundDown} = fraction(In: Greeting, Io; InputOutput)
# or if you want to put it on the right, you use the following TypeScript-friendly syntax:
{RoundDown as IntegerPart;} = fraction(In: Greeting, Io; InputOutput)
# you can specify the return type of the renamed variable like this:
{RoundDown as IntegerPart; int} = fraction(In: Greeting, Io; InputOutput)
# or like this: TODO: does this work ok or is it breaking some type syntax here?
{IntegerPart; int(RoundDown)} = fraction(In: Greeting, Io; InputOutput)

# with pre-existing variables, using MMR and output argument syntax:
Greeting := "hello!"
InputOutput ;= 1.234     # note `;` so it's mutable.
IntegerPart; int
fraction(In: Greeting, Io; InputOutput, ->IntegerPart as RoundDown)
# equivalent:
fraction(In: Greeting, Io; InputOutput, RoundDown: ->IntegerPart)

# we can call with variables that get defined inline like this, besides `Io`, which is MMR.
InputOutput ;= 1.234     # note `;` so it's mutable.
fraction(In: "hello!", Io; InputOutput, ->IntegerPart: int as RoundDown)
# TODO: equivalent?
fraction(In: "hello!", Io; InputOutput, RoundDown: ->IntegerPart: int)

# destructuring
Io ;= 1.234
{RoundDown:} = fn(In: "hello", Io;)

# automatic de-nesting or automatic destructuring
# when there is a single-field object (SFO) with the correct name, we can do automatic de-nesting,
# which is equivalent to the `{RoundDown:} = ...` syntax above:
Io ;= 1.234
RoundDown := fn(In: "hello", Io;)
# TODO: can we define the `;` inline, e.g., `Io; int(3)`?  i think it may be inconsistent with other syntax

# if the field name doesn't match, we won't do automatic de-nesting:
Io ;= 1.234
Result := fn(In: "hello", Io;)
# `Result` is an object because the output variables were named, so we'll get `RoundDown` here.
# note that `Io` will not be a field on `Result` because we got it via the `;` argument reference:
print(Result RoundDown)

# note that when there is no match in names, we'll get the full return object:
Result := fn(In: "hello", Io: 1.234)
# `Result` has nested fields based on the function's outputs:
print(Result Io)
print(Result RoundDown)

# here there's no match in names, so we'll return an object,
# and using `->` will reduce the number of fields in the return object.
Result := fn(In: "Hello", Io: 1.234, ->RoundDown: int)
# `RoundDown` is taken out of the `Result` object here, so there's only `Io` left:
print(Result Io)
# TODO: do we automatically put in a `Io` on the output if `Io` is const on the arguments?
# or do we throw an error and require passing in `Io` as a reference `;`?

# TODO: think about what overload matching means with SFOs, or with return variables whose names
# don't match any return fields.
```

For nested object return types, there is some syntactic sugar for dealing with them.

```
nest(X: int, Y: str): (W: (Z: (A: int), B: str, C: str))
    return (W: (Z: (A: X), B: Y, C: Y * X))

# defines `A`, `B`, and `C` in the outside scope:
(W: Z: A:, W: B:, W: C:) = nest(X: 5, Y: "hi")
print(A)    # 5
print(B)    # "hi"

# equivalently:
(W: (Z: A:, B:, C:)) = nest(X: 5, Y: "hi")

# for renaming the variables, you can do something like this:
Q; int
R; str
S: str
(Q as W: Z: A, R as W: B, S as W: C) = nest(X: 3, Y: "whoa")

# or, inline:
(Q; int as W: Z: A, R; str as W: B, S: str as W: C) = nest(X: 3, Y: "whoa")
# equivalently, spread out a bit to aid in reading:
(
    Q; int as W: Z: A
    R; str as W: B
    S: str as W: C
) = nest(X: 3, Y: "whoa")
```

TODO: make return values part of an `object`.  If return is null, then `Output` is Null (empty object). 
if return is a single variable (e.g., `hello(Int): str), then `Output` populates a default-named field
with the instance (e.g., `Output Str`).  If the return is multiple variables, `Output` has all those fields.
Casting `object` to boolean, e.g., via `if Output $( doSomething() )` will check first to see if `Output` is a
single-field object (SFO).  If so, then we'll cast that field to boolean.  Otherwise, we'll throw a compile error,
requesting users to be more specific.
Don't actually use `object` behind the scenes, except in dynamic programming or cases where it's ambiguous,
since creating objects will incur overhead, but just for organization.

### dynamically determining arguments for a function

We allow for dynamically setting arguments to a function by using the `call` type,
which has a few fields: `Input` and `Output` for the arguments and return values
of the function, as well as optional `Warning` and `Error` fields for any issues
encountered when calling the function.  These fields are named to imply that the function
call can do just about anything (including fetching data from a remote server).

```
# TODO: split into `arguments` and `returns` if we modify MMR with `!!` -> `;`.
# i.e., `fn(Arguments): returns`.  `arguments` include the output field names in a set.
# `arguments~(output limits str|symbol) := { Input; any_str, Output; _output }` and
# `returns~(output limits str|symbol) := any_output`
# TODO: discussion of `limits` and `extends` for template types.
call := {
    Input; any_str
    Output; any_str
    Warning?; string
    # TODO: consider letting this be `error|string` type, or requiring `error` only:
    Error?; string

    # note you can have an arbitrary variable name here via `~Name`,
    # and the variable name string can be accessed via `@Name`.  TODO: switch to `@@Name`
    # TODO: we probably want a more narrow fix here since we'd only not know the name of
    # a variable in these contexts where we're passing in a `~Name: ~t` style argument.
    # maybe something with ~.  maybe `@~Name`
    # TODO: figure out how remote server call would work with an impure function.
    #       this is probably impossible.  no way to keep track of how an impure function
    #       would have called things (i.e., read a value off the local disk, written it later, etc.)
    # TODO: maybe rethink how `if` statements can work with block parentheses `$( ... )`
    # i.e., for MMR-style input -> output variables.
    # NOTE: use before the function call
    io(~Name!; ~t): null
        Output_@Name = t()
        Input_@Name = Name!
}
```

Now, `call` is very generic, so `Input` and `Output` can be filled with any fields (or none),
and this logic is encapsulated in the type `object`.  The `object` type can be thought of as
a collection of nullable fields; if all fields are null, then the object acts like a null.
If only one field is defined, it is a "single field object", or `sfo` type.  The fields have
keys that are symbols (they can be identified by letters/words in code, like `Object FieldKey`),
and values that have any type.

When passing in a `call` instance to actually call a function, the `Input` field will be treated
as constant/read-only after being cast to the field types specified by the chosen function overload.
(E.g., if an argument type is `string` but a number is passed in, it will be converted to a string.)
The `Output` field will be considered "write-only", except for the fact that we'll read what fields
are defined in `Output` to determine which overload to use.  This allows you to define "default
values" for the output fields, which won't get overwritten if the function doesn't write to them.
TODO: fields defined in both input and output might not be read-only inputs, they probably will get
moved over to output for MMR purposes.  so we probably don't want to guarantee that `Input` is read-only

Let's try an example:

```
# define some function to call:
someFunction(X: int): string
    return "hi" * X
# this is an ok overload because the return type is different.
# however, this is not recommended, since the arguments are named the same.
# note that the earliest overload that matches will be default.
someFunction(X: string): int
    return X size()

MyString: string = someFunction(X: 100)     # uses the first overload
MyInt: int = someFunction(X: "cow")         # uses the second overload
WhatIsThis := someFunction(X: "asdf")       # WARNING! calls first overload

# TODO: this might be easier to understand if we used @in/@out arguments

# example which will use the default overload:
Call; call
Call Input X = 2
# TODO: switch to new `arguments/returns` approach
# use `Call` with `;` so that `Call;;Output` can be updated.
someFunction(Call;)
print(Call Output)  # prints {String: "hihi"}

# define a default value for the Input object's field to get the other overload:
Call; call
Call Input X = "hello"
Call Output Int = -1
someFunction(Call;)
print(Call Output)  # prints {Int: 5}

# dynamically determine the function overload:
Call; call = if someCondition()
    {Input: X: 5, Output; String; "?"}
else
    {Input: X: "hey", Output; Int; -1}

someFunction(Call;)
print(Call Output)  # will print {String: "hihihihihi"} or {Int: 3} depending on `someCondition()`.
```

Note that `call` is so generic that you can put any fields that won't actually
be used in the function call.  This can be dangerous, since errors won't be thrown
if you have more fields defined than needed to call the function.  For example,
with the above `someFunction` overloads, the first one is default, so it will cast
a string value of the input field `X` to an integer if both (or neither) output
fields are defined (`Int` and `String`), like this:

```
Call ;= call(Input: X: "4", Output: {Int: 0, String: ""})
someFunction(Call;)
print(Call)  # prints {Input: {X: 4}, Output: {Int: 0, String: "hihihihi"}}
```

If run-time checks and throwing errors are desired, one should use the more specific
`myFn->call` type, with `myFn` the function you want arguments checked against.

```
# throws a compile-time error (if input and output are completely specified at the same time)
# or a run-time error (if input and output are separately defined):
Call ;= someFunction->call(Input: X: "4", Output: {Int: 0, String: ""})  # error!!
# the above will throw a compile-time error, since two values for Output are defined.

# this is ok:
Call2 ;= someFunction->call(Input: X: "4", Output: Int: 0)
# this is also ok, but will cast X to int right away, and will throw an error when defining `Call3`
# if `X` is not a integer-like string rather than when calling someFunction (like `call` would).
Call3 ;= someFunction->call(Input: X: "4", Output: String: "")
# also ok:
Call4 ;= someFunction->call(Input: X: 4, Output: String: "")
```

Note that it's also not allowed to define an overload for the `call` type yourself.
This will give a compile error, e.g.:
TODO: explain reasoning here.

```
# COMPILE ERROR!!  you cannot define an overload for `call`!
someFunction(Call;): null
    print(Call Input X)
```

TODO: probably can allow defining an overload for a `call` argument that is not default-named,
i.e., something like `MyCall; call` is ok.
TODO: discuss the `object` type, which allows you to build up function arguments.
e.g., `Object; object = {Hello: "World"}`.
TODO: the `object` type is recursive, too.  need to think of a good way to handle weird stuff here,
or requesting subfields of a field that was not an object.

If you want the output field to be determined in the normal way (by checking what is
using the function's return value), you can also use `myFn->input` as a way to create
type-safe `object` for the `Input` field of a `myFn->call`, i.e., the correctly typed
arguments to any overload of `myFn`.

TODO: `myFn->output` probably doesn't exist in a type-safe way, since the output is
determined by the input and chosen overload.

```
myFn(Times: int, String): null
    for Int: int < Times
        print(String)
myFn(Dbl: dbl): dbl
    return 5 * Dbl

Input; myFn->input = {}
if SomeCondition
    Input Dbl = 123.3
else
    Input = {Times: 50, String: "Hello"}
# other fields defined on `Input` would give compiler errors, or mismatched fields,
# e.g., `Input = {Times: 1, Dbl: 1.4}` would give an error.

Result ?:= myFn(Input)   # Result can be `null|dbl`
```

TODO: i think it would be best not to throw a run-time error if the arguments
don't match, e.g., if they are overspecified, especially for these dynamically
built arguments.

Note: You *can* create your own overload of a function with the `object` type as
input, but it *cannot* be default-named `Object`.  This is for the same reason that we don't
allow overloading a function with a `call` argument.  For example:

```
myFunction(Object):             # COMPILER ERROR, this is not allowed.
    print(Object DidIDefineThis)
    print(Object MaybeThisToo)

myFunction(Cool: object):       # OK
    print(Cool DidIDefineThis)
    print(Cool MaybeThisToo)

# prints "True" and then "Null" since `Cool MaybeThisToo` is not defined.
myFunction(Cool: {DidIDefineThis: True, X: 5})
```

## redefining a function

To declare a reassignable function, use `;` after the arguments.

```
greetings(Noun: string); null
    print "Hello, ${Noun}!"

# you can use the function:
greetings(Noun: "World")

# or redefine it:
# option 1:
greetings = (Noun: string): null
    print "Hi, ${Noun}!"
# option 2:
greetings = "Greetings, ${$Noun}!"
# NOT OK: COMPILE ERROR: this looks like a redeclaration of the function, rather than a reassignment:
# also not ok if `;` is changed to `:`.
greetings(Noun: string); null
    print "Overwriting?"
```

It needs to be clear what function overload is being redefined (i.e., having the same function signature),
otherwise you're just creating a new overload (and not redefining the function).

## nullable functions

The syntax for declaring a nullable/optional function is to put a `?` after the function name
but before the argument list.  E.g., `optionalFunction?(...Args): returnType` for a non-reassignable
function and swapping `:` for `;` to create a reassignable function.
When calling a nullable function, unless the function is explicitly checked for non-null,
the return type will be nullable.  E.g., `X ?:= optionalFunction(...Args)` will have a
type of `returnType|null`.  Nullable functions are checked by the executable, so the
programmer doesn't necessarily have to do it.

A nullable function has `?` before the argument list; a `?` after the argument list
means that the return type is nullable.  The possible combinations are therefore the following:

* `normalFunction(...Args): returnType` is a non-null function
  returning a non-null `returnType` instance.

* `nullableFunction?(...Args): returnType` is a nullable function,
  which, if non-null, will return a non-null `returnType` instance.
  Conversely, if `nullableFunction` is null, trying to call it will return null.
  You can also declare this as `nullableFunction?: (...Args): returnType`.

* `nullableReturnFunction(...Args)?: returnType` is a non-null function
  which can return a nullable instance of `returnType`.  You can declare
  this as `nullableReturnFunction(...Args): returnType|null` as well.

* `superNullFunction?(...Args)?: returnType` is a nullable function
  which can return a null `returnType` instance, even if the function is non-null.
  I.e., if `superNullFunction` is null, trying to call it will return null,
  but even if it's not null `superNullFunction` can still return null.
  You can also declare this as `superNullFunction?: (...Args)?: returnType`
  or `superNullFunction?: (...Args): returnType|null)`.


Some examples:

```
# creating an optional method in a class:
parent := {
    ;;reset(This X: dbl, This Y: dbl) := Null

    ::optionalMethod?(Z: dbl); int
}

Example ;= parent(X: 5, Y: 1)

# define your own function for optionalMethod:
Example optionalMethod(Z: dbl); int
    return floor(Z * This X)   # note that you can use `This` to access the Example instance.

Example optionalMethod(2.15)    # returns 10

# or set it to Null:
Example optionalMethod = Null

Example optionalMethod(3.21)    # returns Null

# child classes can define a "method" that overrides the parent's optional function:
child := extend(parent) {
    ::optionalMethod(Z: dbl); int
        return ceil(X * Y * exp(-Z))
}

Child ;= child(X: 6, Y: 2)

Child optionalMethod(0)     # returns 12

# but note that unless the method is protected, users can still redefine it.
Child optionalMethod(Z: dbl): int
    return floor(Z)

Child optionalMethod(5.4)   # returns 5

# also note that while the child method appears to always be defined,
# we cannot stop a Null from being assigned here.  this is because
# if the instance is passed into a function which takes a parent class,
# that function scope can reassign the method to Null (since the parent
# class has no restrictions).
Child optionalMethod = Null

# therefore the executable will always check for Null before calling the method:
Child optionalMethod(1.45)  # returns Null
```

## function overloads

Functions can do different things based on which arguments are passed in.
For one function to be an overload of another function, it must be defined in the same file,
and it must have different argument names or return values.  You can also have different
argument modifiers (i.e., `;` and `:` are different overloads, as are nullable types, `?`).

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

# note this is a different overload, since it must be called with `Say;`
greetings(Say; string): null
    Say += " wow"
    print "${Say}, world..."

MySay ;= "hello"
greetings(Say; MySay)   # prints "hello wow, world..."
print(MySay)            # prints "hello wow" since MySay was modified
```

Note also, overloads must be distinguishable based on argument **names**, not types.
Name modifiers (i.e., `;`, `:`, and `?`) also count as different overloads.

```
fibonacci(Times: int): int
    Previous ;= 1
    Current ;= 0
    for Count: int < Times
        NextPrevious := Current
        Current += Previous
        Previous = NextPrevious
    return Current

fibonacci(Times: dbl): int
    GoldenRatio: dbl = (1.0 + \\math sqrt(5)) * 0.5
    OtherRatio: dbl = (1.0 - \\math sqrt(5)) * 0.5
    return int{Round: (GoldenRatio^Times - OtherRatio^Times) / \\math sqrt(5)}
# COMPILE ERROR: function overloads of `fibonacci` must have unique argument names, not argument types.

# NOTE: if the second function returned a `dbl`, then we actually could distinguish between
# the two overloads.  This is because default names for each return would be `Int` and `Dbl`,
# respectively, and that would be enough to distinguish the two functions.  The first overload
# would still be default in the case of a non-matching name (e.g., `Result := fibonnaci(Times: 3)`),
# but we could determine `Int := fibonacci(Times: 3)` to definitely be the first overload and
# `Dbl := fibonacci(Times: 7.3)` to be the second overload.
```

There is the matter of how to determine which overload to call.  We consider
only overloads that possess all the specified input argument names.  If there are some
unknown names in the call, we'll check for matching types.  E.g., an unnamed `4.5`, 
as in `fn(4.5)` or `fn(X, 4.5)`, will be checked for a default-named `Dbl` since `4.5`
is of type `dbl`.  Similarly, if a named variable, e.g., `Q` doesn't match in `fn(Q)`
or `fn(X, Q)`, we'll check if there is an overload of `fn` with a default-named type
of `Q`; e.g., if `Q` is of type `animal`, then we'll look for the `fn(Animal):` overload.

NOTE: we cannot match a function overload that has more arguments than we supplied in
a function call.  If we want to allow missing arguments in the function call, the declaration
should be explicit about that; e.g., `fn(X?: int) := ...` or `fn(X := 0) := ...`.
Similarly, we cannot match an overload that has fewer arguments than we supplied in the call.

Output arguments are similar, and are also matched by name.  This is pretty obvious with
something like `X := calling(InputArgs...)`, which will first look for an `X` output name
to match.  If there is no `X` output name, then the first non-null, default-named output
overload will be used.  E.g., if `calling(InputArgs...): dbl` was defined before
`calling(InputArgs...): str`, then `dbl` will win.  For an output variable with an explicit
type, e.g., `X: dbl = calling(InputArgs...)`, this will look for an overload with output
name `X` first, *then* look for the overload with a default-named `Dbl: dbl` type.
This might be surprising, but we want to enable easy conversions between types which can
implicitly convert to each other, so that we don't always need to be explicit about converting
iterators to containers, etc.  If there is no default-named `Dbl: dbl` output type, we'll look
for the first non-null, default-named output overload.  For function calls like
`X := dbl(calling(InputArgs...))`, we will lose all `X` output name information because
`dbl(...)` will hide the `X` name.  In this case, we'll look for an overload with a
default-named `Dbl: dbl` type output, then any non-null, default-named output overload.
For calls like`X := calling(InputArgs...) Dbl`, we will explicitly pick a default-named
`Dbl: dbl` output overload or throw a compiler error if none exists.

TODO: we probably need an `{@hide X:, Y: str}` here to ensure we match the correct overload.
Note that if the output type is a data class, e.g., `{X: dbl, Y: str}`, then we
want to support destructuring, e.g., `{Y: str} = calling(InputArgs...)` and using
`Y` on subsequent lines as desired.  In this case, the output fields essentially count
as extra arguments, and we again choose the overload that has the fewest number of additional
arguments (or output fields).
TODO: discussion on how `fn(): {Int}` and `fn(): int` are considered the same overload.
We don't want to distinguish between `Y: str = calling(InputArgs...)` and `{Y: str} = ...`.
TODO: discussion on how destructuring looks different than defining a lambda function due
to the extra `:` after the parentheses.  e.g., `(X: int, Y: str) = someFunction(Z)` is
destructuring, but `(X: int, Y: str) := someFunction(Z)` is defining a lambda.
we should throw if users do `X: int := someFunction(Z)` since it looks somewhat ambiguous,
so make sure to include parentheses if a lambda is desired.

We also allow calling functions with any dynamically generated arguments, so that means
being able to resolve the overload at run-time.

## function templates/generics

You can create template functions which can work for a variety of types
using the syntax `~(type1, type2, ...)` or `~someType` (for just one generic type) 
after a function name but before the argument list.  We also allow defining templates
inline for convenience.  e.g., `logger(T: ~t): t`, using `~` the first time you see
the type (reading left to right).
TODO: could do `logger(@infer t: type, T: t): t` if we don't want `~` for it.
I prefer the efficiency of `logger(T: ~t): t`, though.

```
# declaration equivalent to `logger ~(t) (T): t` or `logger(T: ~t): t`:
logger ~t (T): t
    print("got ${T}")
    return T

vector3 := (X: dbl, Y: dbl, Z: dbl)
Vector3 := vector3(Y: 5)
Result := logger(Vector3)   # prints "got vector3(X: 0, Y: 5, Z: 0)".
Vector3 == Result           # equals True

# implicit type request:
OtherResult := logger(5)    # prints "got 5" and returns the integer 5.

# explicit generic request:
DblResult := logger ~dbl (5)    # prints "got 5.0" and returns 5.0

# explicit type request:
AnotherDblResult := logger(dbl(4))  # prints "got 4.0" and returns 4.0
# can also use `logger dbl(4)` above or `logger dbl 4` with function spaces.
```

## pure functions and functions with side effects

Functions that are completely deterministic based on their inputs are called pure functions,
whereas functions with side effects, i.e., relying on memory in other locations to determine
the result, or modifying some memory elsewhere when executed, are not pure.  (External memory
in this situation means memory corresponding to variables not passed in to the function,
or memory in other locations, e.g., on the hard disk.)  Pure functions are defined with the
syntax `functionName(...Args): returnType`, whereas impure functions *must be defined* with a
preceding `;;` if they modify memory outside themselves or `::` if they access memory for
reading but not writing.  Of course, `;;` functions can also read external memory as well.

For examples of pure functions: `fn(X: dbl) := X^2` is pure, as well as most mathematical
functions (`sin`, `cos`, `atan`, etc.).

Examples of impure functions include reading data from a file, which is non-deterministic,
since another program or thread could have modified the file in between reads.  Such a function
would be preceded with `::` since it does not modify the file data, e.g., 
`::read(File): string`, whereas a function that writes data to a file could be
`;;write(File, String): null` to indicate the function can modify memory outside itself.
Random functions are of course impure as well.  Class methods are considered impure,
since they use data within the class instance to determine the result of the function call.

```
# we want to pass in a function here:
check(fn(Int): bool, Int): int
    Result := if fn(Int)
        Int / 2
    else
        2 * Int + 1
    print(Result)
    return Result

# but suppose we have a class which has a method that looks like this function:
exampleClass := {
    CheckTimes: int

    ;;someMethod(Int): bool
        ++CheckTimes
        return (CheckTimes % 2) >< (Int % 2)
}

ExampleClass; exampleClass

# if we allow calling this instance's method in the `check` function, we get
# results that depend on how many times the method has already been called:
check(ExampleClass someMethod, 3)    # returns 7
check(ExampleClass someMethod, 3)    # returns 1
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
the `:=` symbol.  Variables defined within the class body are known as instance
variables, and functions defined within the class body can have three types:
(1) class instance methods (or methods for short), (2) class functions (i.e., static
methods in C++), and (3) instance functions.

Class methods (1) can access instance variables and call other class methods,
and must be defined with a `::` or `;;` prefixed on the function name.
Mutating methods -- i.e., that modify the class instance, `This`, i.e., by modifying
its values/variables -- must be defined with a `;;` before the method name.
Non-mutating methods must be defined with `::` and can access variables but not modify them.
We'll use the notation `someClass;;someMutatingMethod()` to refer to mutating methods
and `someClass::someMethod()` to refer to non-mutating methods.  Calling a class
method does not require the `;;` or `::` prefix, but it is allowed, e.g.,

```
SomeClass ;= someClass("hello!")
SomeClass someMethod()      # ok
SomeClass::someMethod()     # also ok
SomeClass someMutatingMethod()  # ok
SomeClass;;someMutatingMethod() # also ok
```

Note that you can overload a class method with both constant `::` and mutable `;;` versions;
in that case it's recommended to be explicit and use `::` or `;;` instead of ` ` (member access).
See the section on member access operators for how resolution of ` ` works in this case.

And of course, class methods can also be overridden by child classes (see section on overrides).

TODO: maybe switch class functions to `_`, `_::` and `_;;` so we allow `->` for output variables.

Class functions (2) can't depend on any instance variables, and are declared using one
of the operators `->`, `:>`, or `;>`, depending on whether the function is pure, impure (read only),
or impure (read/write).  They are called with the syntax `someClass->somePureStaticFunction(...Args)`,
`someClass:>someReadOnlyImpureStaticFunction(...Args)`, and similarly for `;>` (impure read/write).
Like with pure/impure functions and methods, accessing global-scoped variables for reading requires
being declared with `:>`, or ';>` if global variables need to be read/write.
TODO: we probably can allow calling using `->` at all times, but it might be good to be specific
with `:>` and `;>`.

Instance functions (3) must be pure functions; they can't depend on any instance variables,
but they can be different from instance to instance.  They are defined without `;;`, `::`,
`->`, `:>`, or `;>`, and they cannot be overridden by child classes but they can be overwritten.  I.e.,
if a child class defines the instance function of a parent class, it overwrites the parent's
instance function; calling one calls the other.

Class constructors are methods (1) which are defined using `;;reset(Args...)`,
which also allow you to reset the class instance as long as the variable is mutable.
The first `;;reset` method defined in the class is also the default constructor,
which will be called with default-constructed arguments (if any) if a default
instance of the class is needed.  It is a compiler error if a `;;reset()` method
(with no arguments) is defined after other `;;reset` methods (with arguments).

When defining methods or functions of all kinds, note that you can use `this`
to refer to the current class instance type.

TODO: do we need "block parentheses" notation here, e.g., `${`?  i don't think so, because
`${` should only be required for places where a parentheses can mean something else.
here there is no ambiguity, we are creating an object, but because the LHS is lowerCamelCase,
we are creating a class.

```
exampleClass := {
    # class instance variables can be defined here:
    X; int

    # classes must be resettable to a blank state, or to whatever is specified
    # as the starting value based on a `reset` function.  this is true even
    # if the class instance variables are defined as immutable.
    ;;reset(X; int): null
        This X = X!
    # or short-hand: `;;reset(This X: int) := Null`
    # adding `This` to the arg name will automatically set `This X` to the passed in `X`.

    # class methods can be defined as well.
    # this one does not change the underlying instance:
    ::doSomething(Int): int
        return X + Int

    # this method mutates the class instance, so it is prefaced with `;;`:
    ;;addSomething(Int): null
        X += Int

    # this pure function does not require an instance, and cannot use instance variables:
    ->someStaticFunction(Y; int): int
        Y /= 2
        return Y!

    # this function does not require an instance, and cannot use instance variables,
    # but it can read/write global variables (or other files) due to `;>`:
    ;>someStaticImpureFunctionWithSideEffects(Y: int): null
        write(Y, File: "Y")

    # this function does not require an instance, and cannot use instance variables,
    # but it can read (but not write) global variables (or other files) due to `:>`:
    :>someStaticImpureFunction(): int
        YString := read(File: "Y")
        return int(?YString) ?? 7

    # class instance functions can be defined here; this is a *pure function*
    # that cannot depend on instance variables, however.  it can be set 
    # individually for each class instance, unlike a static class function (`->`, `:>`, and `;>`).
    somePureFunction(): null
        print("hello!")
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

Note that we recommend using named fields for constructors rather than static
class functions to create new instances of the class.  This is because named fields
are self descriptive and don't require named static functions for readability.
E.g., instead of `MyDate := dateClass->fromIsoString("2020-05-04")`, just use
`MyDate := dateClass(IsoString: "2020-05-04")` and define the
`;;reset(IsoString: string)` method accordingly.


## unicode/localization support

We intend hm-lang to support all languages, and so the upper/lower-case requirements
may seem a bit strange in other alphabets.  To set a custom "UpperCamelCase" default name
for an instance of the class, use this notation:

```
örsted := {
    # define a custom default name:
    This := Örsted
    ... other class methods ...
}

# Now we can use `Örsted` to mean a default-named variable of the class örsted:
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
defined as `This;;mutatingMethod(): returnType`
or `This::nonMutatingMethod(): returnType`.  Mutating methods follow the "mutate"
visibility in the table above, and non-mutating methods follow
the "access" visibility in the table above.

To put into words -- `@public` methods can be called by anyone, regardless
of whether the method modifies the class instance or not.  `@protected`
methods which modify the class instance cannot be called by non-friends,
but constant methods can be called by anyone.  `@private` methods which
modify the class instance can only be called by module functions, and
constant methods can be called by friends.

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
example := {
    @visibility
    X; str = "hello"
}
W = example()
W X += ", world"
print(W X)  # prints "hello, world"

# expands to this:
example := {
    @invisible
    X; str

    # getter: calls an external function with X, which can
    #         avoid a copy if the function argument is immutable.
    @visibility
    ::x(fn(Str): ~t) := fn(X)

    # copy.  returns a copied value of `X`.  this method
    # has lower priority than the no-copy getter above.
    @visibility
    ::x() := X

    # move+reset (moot)
    @visibility
    ;;x()!: str
        return X!

    # swapper: swaps the value of X with whatever is passed in
    #          returns the old value of X.
    @visibility
    ;;x(Str;):
        X <-> Str

    # modifier: allows the user to modify the value of X
    #           without copying it, using references.
    @visibility
    ;;x(fn(Str;): ~t) := fn(X;)
}
W = example()
W x(fn(Str;)
    Str += ", world"
)
W x((Str) := print(Str))
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
justCopyable := {
    ::someVar(): int
        return 1000

    #{#
    # the following becomes automatically defined:
    ::someVar(fn(Int): ~t): t
        SomeVar := someVar()
        return fn(SomeVar)
    #}#
}

# a class with a getter method gets a copy method automatically:
justGettable := {
    @invisible
    SomeVar; int

    ::someVar(fn(Int): ~t) := fn(SomeVar)

    #{#
    # the following becomes automatically defined:
    ::someVar(): int
        return someVar(fn(Int) := Int)
    #}#
}

# a class with a swapper method gets a modifier and move+reset method automatically:
justSwappable := {
    @invisible
    SomeVar; int

    @visibility
    ;;someVar(Int;): null
        SomeVar <-> Int
        # you can do some checks/modifications on SomeVar here if you want,
        # though it's best not to surprise developers.  a default-constructed
        # value for `SomeVar` (e.g., in this case `Int := 0`) should be allowed
        # since we use it in the modifier to swap out the real value into a temp.
        # if that's not allowed, you would want to define both the swapper
        # and modifier methods yourself.

    #(#
    # the following modifier becomes automatically defined:
    ;;someVar(fn(Int;): ~t): t
        Temporary; int
        # swap SomeVar into Temporary:
        someVar(Temporary;)     # could also write `SomeVar <-> Temporary`
        T := fn(Temporary;)
        # swap Temporary back into SomeVar:
        someVar(Temporary;)
        return T!

    # and the following move+reset method becomes automatically defined:
    ;;someVar()!: t
        Temporary; int
        # swap SomeVar into Temporary:
        someVar(Temporary;)     # could also write `SomeVar <-> Temporary`
        return Temporary!
    #)#
}

# a class with a modifier method gets a swapper and move+reset method automatically:
justModdable := {
    @invisible
    SomeVar; int

    ;;someVar(fn(Int;): ~t): t
        T := fn(SomeVar;)
        # you can do some checks/modifications on SomeVar here if you want,
        # though it's best not to surprise developers
        return T!

    #(#
    # the following swapper becomes automatically defined:
    ;;someVar(Int;): null
        someVar(;;(Old->Int;): null
            Int <-> Old->Int
        )

    # and the following move+reset method becomes automatically defined:
    ;;someVar()!: t
        Result; int
        someVar(;;(Int;): null
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
For one parent, `extend(parentClassName)`. Multiple inheritance is
allowed as well, e.g., `extend(parentOne, parentTwo, ...)`.
We can access the current class instance using `This`,
and `this` will be the current instance's type.  Thus, `this` is
the parent class if the instance is a parent type, or a subclass if the instance
is a child class.  E.g., a parent class method can return a `this` type instance,
and using the method on a subclass instance will return an instance of the subclass.

We can access member variables or functions that belong to that the parent type,
i.e., without subclass overloads, using the syntax `parentClassName::Variable` or
`parentClassName::someMethod()`.  Use `;;` to access variables or methods that will
mutate the underlying class instance, e.g., `parentClassName;;Variable = 3`.

Some examples:

```
animal := {
    ;;reset(This Name: string): null

    # define two methods on `animal`: `speak` and `go`.
    # these are "abstract" methods, i.e., not implemented by this base class.
    ::speak(): null
    ::goes(): string

    # this method is defined, so it's implemented by the base class.
    # derived classes can still change it, though.
    ::escape(): null
        print "${Name} ${goes()} away!!"

    # a method that returns an instance of whatever the class instance
    # type is known to be.  e.g., an animal returns an animal instance,
    # while a subclass would return a subclass instance:
    ::clone(): this
        return this(Name)
}

snake := extend(animal) {
    # if no `reset` functions are defined,
    # child classes will inherit their parent `reset()` methods.

    ::speak(): null
        print "hisss!"
    ::goes(): string
        return "slithers"

    # no need to override `clone`, since we can create a snake using a name.
}

Snake := snake(Name: "Fred")
Snake escape()  # prints "Fred slithers away!!"

cat := extend(animal) {
    # here we define a `reset` method, so the parent `reset` methods
    # become hidden to users of this child class:
    ;;reset(): null
        # can refer to parent methods using class name:
        animal;;reset(Name: "Cat-don't-care-what-you-name-it")

    ::speak(): null
        print "hisss!"
    ::goes(): string
        return "saunters"

    ::escape(): null
        print "CAT ESCAPES DARINGLY!"

    # the parent `clone()` method won't work, so override:
    ::clone(): this
        # cats are essentially singletons, that cannot have their own name;
        return this()
}

Cat := cat()
Cat escape()    # prints "CAT ESCAPES DARINGLY!"
```

All abstract base classes also provide ways to instantiate using lambda functions.
All abstract methods must be defined for the instance to be created, and if a
`reset` method is defined on the parent, any arguments passed into the first reset
(i.e., which is the default constructor) should be defined for the lambda class.

```
WeirdAnimal := animal(
    Name: "Waberoo"
    ::speak(): null
        print "Meorooo"
    ::goes() := "meanders"
    ::escape(): null
        # to call the parent method `escape()` in here, we can use this:
        animal::escape()
        print "${Name} ${goes()} back..."
        # or we can use this:
        animal::escape()
)

WeirdAnimal escape()    # prints "Waberoo ... meanders ... meanders back ... meanders away!!"
```

TODO: explain that lambda functions that are defined as methods on the class
can use the `This` of the class, but they must be declared as `;;someLambdaMethod(...Args); returnType`
if they want to call any modifying methods of the class or change any instance variables
and `::someLambdaMethod(...Args); returnType` to use other const methods or const variables.

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
SomeAnimal goes()   # prints "slithers"
SomeAnimal = cat()
SomeAnimal goes()   # prints "saunters"
```

This is less surprising than the C++ behavior.  But in cases where users want to gain back
the no-dynamically-allocated class instances, we have a `@only` annotation that can be used
on the type.  E.g., `SomeVariable: @only someType` will ensure that `SomeVariable` is
stack allocated (non-dynamically).  If defined with `;`, the instance can still be modified,
but it will be sliced if some child instance is copied to it.  To prevent confusion, we
enforce that upcasting (going from child to parent) must be done *explicitly*.  For example:

```
mythologicalCat := extend(cat) {
    # extra field which will get sliced off when converting from
    # mythologicalCat to cat:
    Lives ;= 9
}

Cat; @only cat
MythologicalCat ;= mythologicalCat()

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
someExample := {
    Value: int
    ;;reset(Int): null
        This Value = Int
    ::to(): ~t
        return t(Value)
}

SomeExample := someExample(5)

# you can use type inference here based on variable taking the return value:
ToString: string = SomeExample to()

# or you can explicitly ask for the type
ToDbl := SomeExample to ~(dbl) ()

# or you can explicitly ask like this:
To64 := i64(SomeExample to())
```

## generic/template classes

To create a generic class, you put the expression `~(types...)` after the
class identifier.  You can use these new types for any class inheritance
from a parent which is a generic/template class.

```
# create a class with two generic types, `key` and `value`:
genericClass~(key, value) := {
    ;;reset(This Key: key, This Value: value): null
}
# also equivalent:
# genericClass := (This Key: ~key, This Value: ~value)

# creating an instance using type inference:
ClassInstance := genericClass(Key: 5, Value: "hello")
 
# creating an instance with template/generic types specified:
OtherInstance := genericClass ~(key: dbl, value: string) (Key: 3, Value: 4)
# note the passed-in values will be converted into the correct type.
```

TODO:
You can also have virtual generic methods on generic classes, but we may
have to think of a clever way to achieve this.  E.g., C++ does not allow this...
We might have to pass a list of unary and binary (two-operand) operation
functions into the method which know how to handle the other data.
```
generic~t := {
    Value; t

    ::method(U: ~u): u
        OtherT: t = Value * (U + 5)
        return U + OtherT
}

Generic := generic~string
Generic Value = "hello"
print(Generic method(2))    # prints ("3" * 7)

==>
// Put result into first argument, based on operands in second and third slots:
typedef std::function<void(void *, void *, void *)> binaryFunction;
typedef std::function<void(void *)> createOrDeleteFunction;

template <class t>
class generic {
    t Value_;
public:
    void method(
        void *Input_U,
        void *Output_U,
        binaryFunction add_u_and_bigInt,
        binaryFunction multiply_t_and_uStar,
        binaryFunction add_u_and_t,
        createOrDeleteFunction allocate_uStar,
        createOrDeleteFunction cleanup_uStar
    ){
        t OtherT;
        {   
            bigInt _BigInt_ = 5;
            void *_U_plus_5_;
            allocate_uStar(_U_plus_5_);
            add_u_and_bigInt(_U_plus_5_, Input_U, (void *)&_BigInt_);

            multiply_t_and_uStar((void *)&OtherT, (void *)&Value_, _U_plus_5_);
            cleanup_uStar(_U_plus_5_);
        }

        add_u_and_t(Output_U, Input_U, (void *)&OtherT);
    }
};

// TODO: definitions for binary functions and calling method in C++
```

## common class methods

All classes have a few compiler-provided methods which cannot be overridden.

* `;;!: this` creates a temporary with the current instance's values, while
    resetting the current instance to a default instance -- i.e., calling `reset()`.
    Internally, this swaps pointers, but not actual data, so this method
    should be faster than copy for types bigger than the processor's word size.
* `::Is: is` provides the class type.  This makes it easy to determine
    what the current type of a combo-type variable is at run-time.  E.g.,
    TODO: we should probably make this `is` instead of `Is`
    TODO: discuss how `consider`'s `case` statements cast their arguments
    to the same type as the type of the `consider` statement.
    ```
    X; null|int|dbl_
    X = [1.2, 3.4]
    consider X Is
        case null
            print("X was Null")
        case int
            print("X was an integer")
        case dbl_
            print("X was an array of double")
    ```
    Note that the `type` type can be created directly from the type
    constructors, e.g. `null` and `int`, e.g., `type(int) == 5 Is`.
    Note also, the `type` type also can be printed nicely, e.g.,
    `print("asdf" Is)` prints "str", and `str(1 Is) == "int"`.
    TODO: figure out how to get around the problem of an object with
    a field defined as `is` or `Is`.  Maybe we need to use string access,
    e.g., `Object "MyField" = 3`

## adding methods to existing classes

You can declare and define methods in classes with the standard syntax,
e.g., `someClass := {::someMethod(Int): string}`.  But you can also define
new methods on a class outside of the class definition, even in a different file.
For example:

```
#=== some-class.hm ===
someClass := {
    ;;reset(@private This Str: str): null

    ::someMethod(Int): string
        return Str * Int
}

#=== other-file.hm ===
{someClass} := \/some-class

# define a new method on `someClass` from some-class.hm
someClass::newMethod(String): int
    for Int: int < 100
        if someClass::existingMethod == String
            return Int
    return -1

SomeClass := someClass("hi")
print(SomeClass newMethod("hihi"))  # should print 2
print(SomeClass newMethod(""))      # should print 0
print(SomeClass newMethod("hey"))   # should print -1
```

Note that the new method cannot access variables/methods inside the class that
aren't visible to the file where the new method is being created (e.g., `other-file.hm`).
Also, the new method is *not* visible to any other file, even if `other-file.hm` is
imported by that file.

TODO: discussion about how these sorts of package private methods might be good ways to define
the implementation of the class, or some lower-level methods that might make the class definition
too difficult to grasp at once.

## singletons

Defining a singleton class is quite easy, using the `singleton` function.  Like `extend`,
this goes after a `lowerCamelCase` or `UpperCamelCase` declaration with `:=`, and any
arguments inside of `singleton` are the parent classes that the singleton extends.  Standard
singletons use `UpperCamelCase` since they are defining the variable and what it does:

```
AwesomeService := singleton(parentClass1, parentClass2, #[etc.]#) {
    UrlBase := "http://my/website/address.bazinga"
    ::get(Id: string): awesomeData 
        Json := Http get("${UrlBase}/awesome/${Id}") 
        return awesomeData(Json)
}
```

Using `lowerCamelCase` on the LHS actually defines an abstract singleton.  These are useful
when you want to be able to grab an instance of the concrete child-class but only through
the parent class reference.

```
### screen.hm ###
screen := singleton() {
    ;;draw(Image, Vector2): null
    ;;clear(Color := color->Black)
}
### implementation/sdl-screen.hm ###
SdlScreen := singleton(\/../screen screen) {
    ;;draw(Image, Vector2): null
        # actual implementation code:
        SdlSurface draw(Image, Vector2)

    ;;clear(Color := color->Black)
        SdlSurface clear(Color)
}
### some-other-file.hm ###
# this is an error if we haven't imported the sdl-screen file somewhere:
Screen; screen
Screen clear color(R: 50, G: 0, B: 100)
```

You get a run-time error if multiple child-class singletons are imported/instantiated
at the same time.

# aliases

Aliases enable writing out logic with semantically similar descriptions, and 
are useful for gently adjusting programmer expectations.  The hm-lang formatter will
substitute the preferred name for any aliases found, and the compiler will only
warn on finding aliases.
TODO: maybe combine formatter/compiler.

Aliases can be used for simple naming conventions, e.g.:

```
options := mask(
    oneOf(AlignInheritX := 0, AlignCenterX, AlignLeft, AlignRight)
    @alias InheritAlignX := AlignInheritX
)

Options := options->InheritAlignX   # converts to `options->AlignInheritX` on next format.
```

Aliases can also be used for more complicated logic and even deprecating code.

```
myClass := {
    ;;reset(This X; int) := null

    # This was here before...
    # ;;myDeprecatedMethod(DeltaX: int): null
    #     X += DeltaX

    # But we're preferring direct access now:
    @alias ;;myDeprecatedMethod(DeltaX: int): null
        X += DeltaX
}

MyClass ;= myClass(X: 4)
MyClass myDeprecatedMethod(DeltaX: 3)   # converts to `MyClass X += 3` on next format.
```

While it is possible, it is not recommended to use aliases to inline code.

TODO: we probably want to allow `alias`es to stick around, e.g., for extra names
with options.  maybe use `rewrite` or `inline` instead of `alias`.

# modules

Every file in hm-lang is its own module, and we make it easy to reference
code from other files to build applications.  All `.hm` files must be known
at compile time, so referring to other files gets its own special notation.
The operator `\/` begins a file-system search in the current directory.
and two backslashes becomes a search for a library module, e.g., `\\math`.

Subsequent subdirectories are separated using forward slashes, e.g.,
`\/relative/path/to/file` to reference the file at `./relative/path/to/file.hm`,
and `..` is allowed between backslashes to go to the parent directory relative
to the current directory, e.g., `\/../subdirectory_in_parent_directory/other/file`.
Note that we don't include the `.hm` extension on the final file, and we allow
underscores in file and directory names.

For example, suppose we have two files, `vector2.hm` and `main.hm` in the same
directory.  Each of these is considered a module, and we can use backslashes
to invoke logic from these external files.

```
# vector2.hm
vector2 := {
    ;;reset(This X: dbl, This Y: dbl): null

    ::dot(Vector2: vector2) := X * Vector2 X + Y * Vector2 Y
}

# main.hm
Vector2Module: hm = \/vector2    # .hm extension must be left off.
Vector2 := Vector2Module vector2(X: 3, Y: 4)
print(Vector2)
# TODO, support destructuring in the future:
# {vector2} := \/vector2
```

You can use this `\/` notation inline as well, which is recommended
for avoiding unnecessary imports.  It will be a language feature to
parse all imports when compiling a file, regardless of whether they're used,
rather than to dynamically load modules.  This ensures that if another imported file
has compile-time errors they will be known at compile time, not run time.

```
# importing a function from a file in a relative path:
print(\/path/to/relative/file functionFromFile("hello, world!"))

# importing a function from the math library:
Angle := \\math atan2(X: 5, Y: -3)
```

To import a path that has special characters, just use the special characters
inline after the `\/`, e.g., `\/sehr/übel` to reference the file at `./sehr/übel.hm`.
For a path that has spaces (e.g., in file or directory names), use parentheses to
surround the path, e.g., `\\[library/path/with spaces]` for a library path or 
`\/[relative/path/with a space/to/a/great file]` for a relative path.  Or you can
use a backslash to escape the space, e.g., `\\library/path/with\ spaces` or
`\/relative/path/with\ a\ space/to/a/great\ file`.  Other standard escape sequences
(using backslashes) will probably be supported.

Note that while `\\math atan(X, Y)` might look like grammatically like `\\math(atan(X, Y))`,
which would be wrong for operator precedence, we instead take the entire import as
if it was an UpperCamelCase identifier.  E.g., `\\math` acts like one identifier, `Math`,
so `\\math atan(X, Y)` resolves like `Math atan(X, Y)`, i.e., member access or drilling down
from `Math := \\math`.  Similarly for any relative import; `\/relative/import/file someFunction(Q)`
correctly becomes like `File someFunction(Q)` for `File := \/relative/import/file`.

## file access / file system

TODO: how does file access work with the reference pattern
E.g., we need to make sure that functions which access the file system are declared with `;;` or `::`.

# errors and asserts

hm-lang tries to make errors easy, automatically creating subclasses of error for each module,
e.g., `map.hm` has a `map->error` type which can be caught using `catch error` or `catch map->error`.
Use `throw errorType("message ${HelpfulVariableToDebug}")` to throw a specific error, or 
`throw "message ${HelpfulVariableToDebug}"` to automatically use the correct error subclass for
whatever context you're in.  Note that you're not able to throw a module-specific error
from another module (e.g., you can't throw `map->error` from the `array.hm` module), but you can
*catch* module-specific errors from another module (e.g., `catch map->error` from the `array.hm`
module).  Of course, you can throw/catch explicitly defined errors from other modules, as long as
they are visible to you (see section on public/protected/private visibility).

The built-in `assert` statement will throw if the rest of the statement does not evolve to truthy.
As a bonus, when throwing, all values will be logged to stderr as well for debugging purposes.

```
assert SomeVariable == ExpectedValue    # throws if `SomeVariable != ExpectedValue`,
                                        # printing to stderr the values of both `SomeVariable`
                                        # and `ExpectedValue` if so.

assert SomeClass method(100)        # throws if `SomeClass method(100)` is not truthy,
                                    # printing value of `SomeClass` and `SomeClass method(100)`.

assert SomeClass otherMethod("hi") > 10     # throws if `SomeClass otherMethod("hi") <= 10`,
                                            # printing value of `SomeClass` as well as
                                            # `SomeClass otherMethod("hi")`.
```

It is not allowed to use `assert` inside an expression; it must be at the start of a statement,
since it is a "greedy" keyword that consumes the rest of the statement.  For customization of
the error, you can add an optional indented line that includes an error, some class instance
that inherits from error, or an error message specifically.  This is mostly helpful when
you're checking a logical expression that isn't self documenting.  Be careful not to double
indent here, since that would mean line continuation (i.e., of the `assert` statement).

```
# custom error:
assert This Might Not Be Self Explanatory
    myCustomError("should have tried X instead of Y")

# default error:
assert SomeString endsWith(")")
    `expected nothing else on the line
     after the parenthetical.`

# probably not intentional, parses as `Whatever Expression "this is a ..."`:
assert WhateverExpression
        "this is a double indented line, doesn't count as the assert error message"
```

Note that `assert` logic is always run, even in non-debug code.  To only check statements
in the debug binary, use `debug->assert`, which has the same signature as `assert`.  Using
`debug->assert` is not recommended, except to enforce the caller contract of private/protected
methods.  For public methods, `assert` should always be used to check arguments.  Note also
that `assert` will throw the correct error subclass for the module that it is in;
`debug->assert` will throw a `debug->error` to help indicate that it is not a production error.

TODO: try/catch/finally 

# standard container classes (and helpers)

```
container~t := {
    ;;insert(T): null

    # returns true iff the element was present in the container before being removed:
    ;;remove(T): bool

    ::contains(T): bool

    ::size(): index
}
```

## arrays

An array contains a list of elements in contiguous memory.  You can
define an array explicitly using the notation `array~elementType`
for the type `elementType`, or implicitly with the subscript operator, `_`
(AKA "key" or "indexing" operator), using the notation `elementType_`.
E.g. `MyArray: int_` or `MyArray: array~int` for an immutable integer array.
The mutable versions of course use `;` instead of `:`.

Side note: as we will see, the subscript operator is usually a binary operator, i.e.,
requiring two operands, `A _ B`, read "A subscript B".  We make an exception for the
array type not to require a second operand -- in fact, adding one would create a
different type, the map type.

The default-named version of an array does not depend on the element type;
it is always `Array`.  Example usage and declarations:

```
# this is an immutable array:
MyArray: dbl_ = [1.2, 3, 4.5]       # converts all to dbl
MyArray append(5)   # COMPILE ERROR: MyArray is immutable
MyArray_1 += 5      # COMPILE ERROR: MyArray is immutable

# mutable integer array:
Array; int_    # declaring a mutable, default-named integer array
Array append(5)     # now Array == [5]
Array_3 += 30       # now Array == [5, 0, 0, 30]
Array_4 = 300       # now Array == [5, 0, 0, 30, 300]
Array_2 -= 5        # now Array == [5, 0, -5, 30, 300]

# mutable string array:
StringArray; string_ = ["hi", "there"]
print(StringArray pop())    # prints "there".  now StringArray == ["hi"]
```

The default implementation of `array` might be internally a contiguous deque,
so that we can pop or insert into the beginning at O(1).  We might reserve
`stack` for a contiguous list that grows in one direction only.

```
# some relevant pieces of the class definition
array~t := extend(container~t, container~{Index, T}) {
    # swapper, sets the value at the index, returning the old value in the reference.
    # if the swapped in value is Null but the array value wasn't Null, the array
    # will shrink by one, and all later indexed values will move down one index.
    ;;_(Index, T?;): null

    ::!!This: bool
        return size() > 0

    # modifier, allows access to modify the internal value via reference.
    # passes the current value at the index into the passed-in function by reference (`;`).
    # if Index >= the current size(), then the array size is increased (to Index + 1)
    # and filled with default values so that a valid reference can be passed into the callback.
    ;;_(Index, fn(T;): ~u): u

    # getter, which returns a Null if index is out of bounds of the array:
    ::_(Index, fn(T?): ~u): u

    # getter, which never returns Null, but will throw if index is out of bounds of the array:
    ::_(Index, fn(T): ~u): u

    # Note: You can use the `;:` const template for function arguments.
    # e.g., `myArray~t := extend(array~t) ${ ;:_(Index, fn(T;:): ~u) := array;:(Index, fn) }`
    
    # nullable modifier, which returns a Null if index is out of bounds of the array.
    # if the reference to the value in the array (`T?;`) is null, but you switch to
    # something non-null, the array will expand to that size (with default values
    # in between, if necessary).  if you set the reference to Null and it wasn't
    # Null before, then the array will shrink by one, and all later index values
    # will move down one.
    ;;_(Index, fn(T?;): ~u): u

    # non-nullable modifier, which will increase the size of the array (with default values)
    # if you are asking for a value at an index out of bounds of the array.
    ;;_(Index, fn(T;): ~u): u

    ::size(): count

    ;;append(T): null

    ;;pop(Index: index = -1): t

    # returns a copy of this array, but sorted:
    ::sort(): this

    # sorts this array in place:
    ;;sort(): null
    ...
}
```

### fixed-size arrays

We declare an array with a fixed number of elements using the notation
`elementType_FixedSize`, where `FixedSize` is a constant integer expression (e.g., 5)
or a variable that can be converted to the `count` type.  Fixed-size array elements
will be initialized to the default value of the element type, e.g., 0 for number types.

Under the hood, fixed-size arrays *are* standard arrays, but they will throw for any
operation that changes the array size.  hm-lang attempts to throw compiler errors where
possible (i.e., by deleting methods like `pop` or `append`), but there may be runtime
errors (e.g., `Array[X] = 3` where `X` is unknown by the compiler).  Like regular arrays,
fixed-size arrays are zero-indexed, so the first element in a fixed-size array
`Array` is `Array_0`.

Fixed-size arrays can be passed in without a copy to functions taking
an array as an immutable argument, but will be of course copied into a 
resizable array if the argument is mutable.  Some examples:

```
# immutable array of size 4
Int4: int_4 = [-1, 5, 200, 3450]
# mutable array of fixed-size 3:
Vector3; dbl_3 = [1.5, 2.4, 3.1]
print("Vector3 is {${Vector3_0}, ${Vector3_1}, ${Vector3_2}}")

# a function with a mutable argument:
doSomething(CopiedArray; dbl_): dbl_2
    # you wouldn't actually use a mutable array argument, unless you did
    # some computations using the array as a workspace.
    # PRETENDING TO DO SOMETHING USEFUL WITH CopiedArray:
    return [CopiedArray pop(), CopiedArray pop()]

# a function with an immutable argument:
doSomething(ConstArray: dbl_): dbl_2
    return [ConstArray_-2, ConstArray_-1]

# copies Vector3, of course:
print(doSomething(CopiedArray: Vector3))    # prints [3.1, 2.4]
# can bring in Vector3 by reference (i.e., no copy) here:
print(doSomething(ConstArray: Vector3))     # prints [3.1, 2.4]
```

There may be optimizations if the fixed-array size is known at compile-time,
i.e., being defined on the stack rather than the heap.  But when the fixed
size is unknown at compile time, the fixed-size array will be defined on the heap:

```
# note you can use an input argument to define the return type's
# fixed-array size, which is something like a generic:
range(Int): int_Int
    assert Int >= 0
        "can't have a fixed negative-size array"
    Result; int_Int
    for I: int < Int
        Result_I = I
    return Result

print(range(10))    # prints [0,1,2,3,4,5,6,7,8,9]
```

### possible implementation

In hm-lang:

```
fixedSizeArray~t := extend(array~t) {
    @hide pop
    @hide insert
    @hide erase
    @hide append
    @hide shift
    # TODO: double check this syntax.
    @for (Array;;method) in array~t
        # TODO: update `Call` stuff
        ;;method(Call; method->call): null
            SizeBefore := size()
            Array;;method(Call)
            assert size() != SizeBefore
}
```

In C++, we might do something like this:

```
template <class t>
class fixedSizeArray {
    array<t> Internal;
public:
    const array<t> *operator -> () const {
        return &Internal;
    }
    fixedSizeArrayModifier<t> modify() {
        return fixedSizeArrayModifier<t>(&Internal);
    }
};

template <class t>
class fixedSizeArrayModifier {
    array<t> *Internal;
    count InitialSize;
public:
    fixedSizeArrayModifier(array<t> *Array)
    :   Internal(Array),
        InitialSize(Array->size())
    {}

    NO_COPY(fixedSizeArrayModifier)
    NO_MOVE(fixedSizeArrayModifier)

    ~fixedSizeArrayModifier() {
        if (Internal->size() != InitialSize) {
            throw fixedSizeArrayError("Fixed-size array was modified");
        }
    }

    array<t> *operator -> () {
        return &Internal;
    }
};
```

## maps

A map can look up, insert, and delete elements by key quickly (ideally amortized
at `O(1)` or at worst `O(lg(N)`).  You can use the explicit way to define a map, e.g.,
`VariableName: map~(key: keyType, value: valueType)`,
or you can use the implicit method with the subscript operator (`_`),
`VariableName: valueType_keyType`.  You can read the operator `_` as "keyed by",
e.g., `valueType_keyType` as "`valueType` keyed by `keyType`".  For example,
for a map from integers to strings, you can use: `MyMap: string_int`.
The default name for a map variable is `Map`, regardless of key or value type.
Note that while an array can be thought of as a map from the `index` type to
whatever the array element type is, `elementType_index` indicates a map type,
not an array type.  The array type, `elementType_` would be useful for densely
packed data (i.e., instances of `elementType` for most indices), while the map
type `elementType_index` would be useful for sparse data.

To define a map (and its contents) inline, use this notation:

```
# map from string to ints:
EmployeeIds: int_string = [
    # option 1: `X: Y` syntax
    "Jane": 123
    # option 2: `{Key: X, Value: Y}` syntax
    {Key: "Jane", Value: 123}
    # option 3: `[X, Y]` syntax, ok if key and value types are different
    ["Jane", 123]
]
# note that commas are optional if elements are separated by newlines,
# but required if elements are placed on the same line.

# equivalent definition would occur with this first line:
# `EmployeeIds := map~(key: string, value: int) [`
```

Maps require a key type whose instances can hash to an integer or string-like value.
E.g., `dbl` and `flt` cannot be used, nor can types which include those (e.g., `array ~dbl`).

```
DblDatabase; dbl_int       # OK, int is an OK key type
DblDblDatabase; dbl_dbl    # COMPILE ERROR, dbl is an invalid key type.
```

However, we allow casting from these prohibited types to allowed key types.  For example:

```
NameDatabase; string_int
NameDatabase_123 = "John"
NameDatabase_124 = "Jane"
# TODO: probably want to make rounding behavior explicit, e.g., `123.4::round(Stochastically)`:
print(NameDatabase_123.4)   # prints "John" with 60% probability, "Jane" with 40%.

# note that the definition of the key is an immutable array:
StackDatabase; string_(int_)    # parentheses are grammatically unnecessary,
                                # since subscripts go right to left.
                                # i.e., `StackDatabase; string_int_` would be ok too.
StackDatabase_[1,2,3] = "stack123"
StackDatabase_[1,2,4] = "stack124"
# TODO: probably need to do something like `[1.0, 2.0, 3.1];;;round(Stochastically)` here,
# where `;;;` means to loop over elements of array and apply the method on the RHS.
print(StackDatabase_[1.0, 2.0, 3.1])    # prints "stack123" with 90% probability, "stack124" with 10%
# things get more complicated, of course, if all array elements are non-integer.
# the array is cast to the key type (integer array) first.
StackDatabase_[2.2, 3.5, 4.8] = "odd"
# result could be stored in [2, 3, 4], [2, 3, 5], [2, 4, 4], [2, 4, 5],
#                           [3, 3, 4], [3, 3, 5], [3, 4, 4], [3, 4, 5]
# but the key is decided first, then the map is added to.
```

Note: when used as a map key, objects with nested fields become deeply constant,
regardless of whether the internal fields were defined with `;` or `:`.
I.e., the object is defined as if with a `:`.  This is because we need key
stability inside a map; we're not allowed to change the key or it could
change places inside the map and/or collide with an existing key.

Some relevant pieces of the class definition:

```
map~(key, value) := extend(container~key, container~{Key, Value}, container~value) {
    # always returns a non-null type, adding
    # a default-initialized value if necessary:
    # returns a copy of the value at key, too.
    ;;_(Key): value

    # getter, which will create a default value instance if it's not present at Key.
    ;;_(Key, fn(Value): ~t): t

    # returns a Null if key is not in the map.
    ::_(Key)?: value

    # getter, which will pass back a Null if the key is not in the map.
    ::_(Key, fn(Value?): ~t): t

    # sets the value at the key, returning the old value:
    ;;_(Key, Value;): value

    # modifier, allows access to modify the internal value via reference.
    # passes the current value at the key into the passed-in function by reference (`;`).
    # the return value of the passed-in function will become the new value at the key.
    # if a value at `Key` is not already present, a default `Value` will be created.
    ;;_(Key, fn(Value;): ~t): t

    # nullable modifier, which returns a Null if the key is not in the map.
    # if the Value wasn't Null, but becomes Null via the passed-in function,
    # the key will be deleted from the map.  conversely, if the value was Null, but
    # the passed-in function turns it into something non-null, the key/value will be added
    # to the map.
    ;;_(Key, fn(Value?;): ~t): t

    # getter and modifier in one definition, with the `;:` "template mutability" operator:
    # will throw for the const map (`::`) if Key is not in the map.
    ;:_(Key, fn(;:Value): ~t): t

    # nullable getter/modifier in one definition, with the `;:` template mutability operator:
    ;:_(Key, fn(;:Value?): ~t): t

    ::size(): count

    ;;pop(Key): value
}
```

The default map type is `insertionOrderedMap`, which means that the order of elements
is preserved based on insertion; i.e., new keys come after old keys when iterating.
Other notable maps include `keyOrderedMap`, which will iterate over elements in order
of their sorted keys, and `unorderedMap`, which has an unpredictable iteration order.
Note that `keyOrderedMap` has `O(lg(N))` complexity for look up, insert, and delete,
while `insertionOrderedMap` has some extra overhead but is `O(1)` for these operations,
like `unorderedMap`.

```
@private
indexedMapValue~t := {NextIndex; index, PreviousIndex; index, Value; t}

insertionOrderedMap~(key, value) := extend(map) {
    KeyIndices; @private @only unorderedBiMap~(key, value: index)
    IndexedValues; @private @only unorderedMap~(
        key: index
        value: indexedMapValue~value
    ) = [{key: 0, value: {NextIndex: 0, value(), PreviousIndex: 0}}]
    NextAvailableIndex; @private index = 1

    # creates a default value if not present at the key to pass in to the modifier:
    ;;_(Key, fn(Value;): ~t): t
        Index ?:= KeyIndices_(Key)
        return if Index != Null
            modifyAlreadyPresent(Index, fn)
        else
            needToInsertThenModify(Key, fn)

    ::_(Key, fn(Value?): ~t): t
        Index ?:= KeyIndices_(Key)
        return if Index != Null
            assert Index != 0
            IndexedValues_(Index, ::(IndexedValue: indexedMapValue~value): t
                return fn(IndexedValue Value)
            )
        else
            fn(Null)
    
    ::forEach(Loop->fn(Key, Value): forLoop): null
        Index ;= IndexedValues_0 NextIndex
        while Index != 0
            Key := KeyIndices_Index
            {Value} := IndexedValues_Index
            ForLoop := Loop->fn(Key, Value)
            if ForLoop == forLoop->Break
                break
            Index = IndexedValues_Index NextIndex
        # mostly equivalent to using nested functions to avoid copies:
        # `ForLoop := KeyIndices_(Value: Index, ::(Key?):
        #      assert Key != Null
        #      return IndexedValues_(Index, ::(IndexedMapValue?):
        #          assert IndexedMapValue != Null
        #          return Loop->fn(Key, IndexedMapValue Value)
        #      )
        #  )
        # `
        # which isn't idiomatic, but maybe what we want to do internally
        # if we don't have const references, to avoid copies of Key and Value instances.

    # modifier for a keyed value not yet in the map, need to insert a default first:
    @private
    ;;needToInsertThenModify(Key, fn(Value;): ~t): t
        NewIndex := AvailableIndex++ || reshuffle()
        KeyIndices_Key = NewIndex
        PreviouslyLastIndex := IndexedValues_0 PreviousIndex
        IndexedValues_0 PreviousIndex = NewIndex
        IndexedValues_PreviouslyLastIndex NextIndex = NewIndex
        IndexedValues_NewIndex = {
            PreviousIndex: PreviouslyLastIndex
            NextIndex: 0
            Value: value()
        }
        return modifyAlreadyPresent(NewIndex, fn)

    # modifier for an already indexed value in the map:
    @private
    modifyAlreadyPresent(Index, fn(Value;): ~t): t
        assert Index != 0
        assert IndexedValues has(Index)
        return IndexedValues_(Index, ::(IndexedValue; indexedMapValue~value): t
            return fn(IndexedValue Value;)
        )
}
```

## sets

A set contains some elements, and makes checking for the existence of an element within
fast, i.e., O(1).  Like with map keys, the set's element type must satisfy certain properties
(e.g., integer/string-like).  The syntax to define a set is `VariableName: set~elementType`
to be explicit or `VariableName: _elementType` using the subscript operator `_` on the
opposite side of the array type (i.e., the array looks like `arrayElementType_`).
The default-named variable name for a set is `Set`.

```
set~t := extend(container~t) {
    # returns true if the passed-in element is present in the set.
    ::_(T): bool

    ::size(): count

    # add an element to the set:
    ;;+=(T;): null

    # TODO: generalize with iterator or container:
    # union this set with another set:
    ;;+=(Set: set~t): null

    # remove an element from the set, if present
    ;;-=(T): null

    ;;pop(): t

    ...
}
```

Like the keys in hash maps, items added to a set become deeply constant,
even if the set variable is mutable.

TODO: discussion on `insertionOrderedSet` and `unorderedSet`, if we want them.

TODO: there should be a fast way to define a set that doesn't get mistaken for a map
or an array.  e.g., `{"hello", "world"}` and even `{X, Y}` for predefined `X, Y`.
maybe `_{X, Y}` but that could get overloaded with something before it.

TODO: make it easy to pass in a set as an argument and return a map with e.g. those keys.
  maybe this isn't as important as it would be if we had a dedicated object type.
```
fn(PickFrom: ~t_str, Fields: _str): t_str
    return Fields map(::(Field: str) := mapElement(Field, PickFrom_Field))

fn~{o, k from keys(o)}(PickFrom: o, Keys: k): pick(o, k)
    return pick(PickFrom, Keys)
```

TODO: dedicated object type and convenience methods.  e.g., `keys(O)` and `keys(o)`,
`pick(o, k)` and `pick(O, K)`, as well as `addMethod`, `addField`, etc.
TODO: discuss how `in` sounds like just one key from the set of keys (e.g., `k in keys(o)`)
and `from` selects multiple (or no) keys from the set (`k from keys(o)`).

## iterator

```
# TODO: no-copy iterator might require passing in the actual container in each
# instance.  e.g., `;;next(fn(T?): ~u, Container: container~t): u`
iterator~t := {
    # next value via getter function:
    ;;next(fn(T?): ~u): u

    # peak via no-copy getter function:
    ::peak?(fn(T?): ~u): u

    # present only if underlying container supports removing the current element (at `peak()`)
    # returns the element, or null if no current element.
    # TODO: figure out a nice syntax for this method to automatically
    # be defined IF replace is defined, and to return a null in replace,
    # but allow it to be overridden if remove is defined separately.
    # TODO: maybe require this for all containers that we use, so that things work "as expected"
    ;;remove?()?: t

    # present only if underlying container supports inserting a new element (before `peak()`)
    # TODO: maybe require this for all containers that we use, so that things work "as expected"
    ;;insert?(T): null

    # replaces the element at `next()` based on the passed-in value,
    # and the iterator should increment (i.e., skip the value you just passed in).
    # if there was an element at this point in the container, and a null
    # is passed into `replace`, then the element (and its former location)
    # should be deleted out of the container.  if `next()` is Null but the passed-in
    # reference is not, then the new value should be added to the container.
    # the value that *was* at `next()` will be swapped into the reference.
    ;;replace?(T?;): null
}
```

For example, here is a way to create an iterator over some number of indices:

```
range~t := extend(iterator~t) {
    @private
    NextIndex: t = 0

    ;;reset(StartAt: t = 0, This LessThan: t = 0): null
        NextIndex = StartAt

    ;;next()?: t
        if NextIndex < LessThan
            Result := NextIndex
            ++NextIndex
            return Result
        return Null

    ::peak() := if NextIndex < LessThan
        NextIndex 
    else
        Null
}

for (Index: index) in range(LessThan: 10)
    print Index
# prints "0" to "9"
```

TODO: how does this work with the "no pointers" framework?

For example, here is an array iterator:

```
arrayIterator~t := extend(iterator~t) {
    # to avoid creating a pointer, we need to pass in the array;
    # move the array in to avoid copying.
    ;;reset(This Array!: t_, This NextIndex: index = 0) := Null
    # TODO: see if there's a better syntax for this:
    # To take an Array and return the Array back, no-copy, use the `with @holding` syntax:
    # e.g., 
    #   MyArray; int_ = [1,2,3,4]
    #   with Iterator ;= iterator @holding(MyArray)
    #       for (Int: int) in Iterator
    #           ...
    #   print(MyArray)
    #   # MyArray is now back to [1,2,3,4] unless there were changes during iteration,
    #   # but in any case, without a copy,

    ;;next()?: t
        ???
}
```

Or should we define iterators on the container itself?  E.g.,

```
array~t := {
    # const iteration, with no-copy if possible:
    ::forEach(fn(T): forLoop): null
        for Index: index < size()
            # use the no-copy getter, here:
            # explicit:
            ForLoop := This_(Index, fn)
            # implicit:
            ForLoop := fn(This_Index)
            if ForLoop == forLoop->Break
                break

    # no-copy iteration, but can mutate the array.
    ;;forEach(fn(T;): forLoop): null
        for Index: index < size()
            # do a swap on the value based on the passed in function:
            # explicit:
            ForLoop := This_(Index, fn)
            # implicit:
            ForLoop := fn(This_Index;)
            if ForLoop == forLoop->Break
                break

    # mutability template for both of the above:
    ;:forEach(fn(;:T): forLoop): bool
        for Index: index < size()
            if fn(;:This_Index) isBreak()
                return True
        return False
}
```

# standard flow constructs / flow control

We have a few standard control statements or flow control keywords in hm-lang.

TODO -- `return`
TODO -- description, plus `if/else/else if` section

## conditional expressions

Conditional statements including `if`, `else if`, `else`, as well as `consider` and `case`,
can act as expressions and return values to the wider scope.  This obviates the need
for ternary operators (like
`X = doSomething() if Condition else DefaultValue` in python, or
`int X = Condition ? doSomething() : DefaultValue;` in C/C++).
In hm-lang, we borrow from Kotlin the idea that
[`if` is an expression](https://kotlinlang.org/docs/control-flow.html#if-expression)
and write something like:

```
X := if Condition
    doSomething()
else
    calculateSideEffects(...) # ignored for setting X
    DefaultValue

# now X is either the result of `doSomething()` or `DefaultValue`.
# note, we can also do this with the `$(` `)` operator to indicate blocks, e.g..

X := if Condition $( doSomething() ) else $( calculateSideEffects(...), DefaultValue )
```

Note that ternary logic short-circuits operations, so that calling the function
`doSomething()` only occurs if `Condition` is true.  Also, only the last line
of a block can become the RHS value for a statement like this.

TODO: more discussion about how `return` works vs. putting a RHS statement on a line.
TODO: add a way to get two variables out of this, e.g.,
```
{X, Y} := if Condition
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
# ERROR, NOT WHAT YOU WANT:
Q ?:= if Condition
        What + IndentTwice
# actually looks to the compiler like:
Q ?:= if Condition What + IndentTwice
```

Which will give a compiler error since there is no internal block for the `if` statement.

### consider-case statements

TODO: non-ternary example with numbers.
TODO: fallThrough keyword and behavior when multiple cases are in a row.
TODO: consider doing `case Option1, Option2` instead of `case Option1 \n case Option2` with implied fallThrough.
TODO: explain how `case` values are cast to the same type as the value being `consider`-ed.

You can use RHS expressions for the last line of a `case` block to return a value
to the original scope.  In this example, `X` can become 5, 7, or 100, with various
side effects (i.e., printing).

```
X := consider String
    case "hello"
        print("hello to you, too!")
        5
    case "world"
        print("it's a big place")
        7
    default
        100

# Note again that you can use `$(` ... `)` block operators to make these inline.
# Try to keep usage to code that can fit legibly on one line:
Y := consider String $( case "hello" $(5), case "world" $(7), default $(100) )
```

Implementation details:  For strings, at compile time we do a fast hash of each 
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

Similarly, any class that supports a compile-time fast hash with a salt can be
put into a consider-case statement.  Floating point classes or containers thereof
(e.g., `dbl` or `flt_`) are not considered *exact* enough to be hashable, but
hm-lang will support fast hashes for classes like `int`, `i32`, and `u64_`.

```
myHashableClass := {
    Id: u64
    Name; string

    # we allow a generic hash builder so we can do cryptographically secure hashes
    # or fast hashes in one definition, depending on what is required.
    # TODO: This should probably already be defined for classes with exact fields!
    ::hash(Builder; ~builder):
        Id hash(Builder;)
        Name hash(Builder;)
}

# note that defining `hash(~Builder;)` automatically defines a `fastHash` like this:
# fastHash(MyHashableClass, Salt: ~salt): salt
#   Builder := \\hash fast(Salt)
#   MyHashableClass hash(Builder;)
#   return Builder build()

MyHashableClass := myHashableClass(Id: 123, Name: "Whatever")

consider MyHashableClass
    case myHashableClass(Id: 5, Name: "Ok")
        print("5 OK")
    case myHashableClass(Id: 123, Name: "Whatever")
        print("great!")
```

Note that if your `fastHash` implementation is terrible (e.g., `fastHash(Salt) := Salt`),
then the compiler will error out after a certain number of attempts with different salts.

TODO: figure out a way to do a fast hash on a `set` or `map`, maybe need to sort
the keys first so the insertion order doesn't matter.  or we require that people
sort their maps/sets first so that insertion order doesn't matter.  maybe make
it a compile-time error to `consider` a set or map that hasn't been sorted just
above; give convenience annotations like `Map alreadySorted()` or `Set insertionOrdered()`
which are allowed inside a consider-case statement.  and, of course `Map;;sort()` is also allowed.
TODO: discussion on return types, e.g., `map::alreadySorted(): this` technically returns a reference,
unless the `map` instance is a temporary.

## for loops

```
# for-loop with counter that is immutable inside the for-loop's block:
for Value: int < 10
    # Value goes from 0 to 9 by 1;
    # Value is not allowed to be mutated (defined with `:`).
    # trying to mutate it throws a compiler error.
    print Value
# prints "0" to "9" on separate newlines.

# for-loop whose counter can be modified inside the block.
# not recommended, since it's a bit harder to reason about.
for Special; int < 4
    print("A:${Special}")
    ++Special
    print("B:${Special}")
    ++Special
# prints "A:0", "B:1", "A:3", "B:4"     # notice skip from B:1 to A:3 as `Special`
# increments on its own because of the for-loop iteration logic.  Note also the
# possibly undesired behavior that `Special` becomes >= 4 inside the for-loop block.

# for-loop iterating over non-number elements:
vector2 := {X: dbl, Y: dbl}
Array: vector2_ = [{X: 5, Y: 3}, {X: 10, Y: 17}]
for (Vector2: vector2) in Array
    print(Vector2)

# when using `in`, we need to use parentheses around the iterand,
# but we don't need to be explicit with the type if the variable name is
# the default name for the variable type:
for (Vector2) in Array
    print(Vector2)
```

# printing and echoing output

TODO: allow tabbed print output.  e.g., `\n` inside of a `string()` method
will tab to the correct indent.

# enums and masks

## enumerations

We can create a new type that exhaustively declares all possible values it can take.
The syntax is `lowerCamelCase := enumerate` followed by a list of named values
(each an `UpperCamelCase` identifier), with optional values they take.  Enumerations
are mutually exclusive -- no two values may be held simultaneously.  See
masks for a similar class type that allows multiple options at once.

For example:

```
# TODO: should definitions be `False := 0` and `True := 1` ?
# Enums use a similar syntax as maps when being defined,
# except that the left-hand side must be an UpperCamelCase identifier:
bool := enumerate(
    False: 0
    True: 1
)
```

Enums provide a few extra additional properties for free as well, including the number of
values that are enumerated via the method `size(): count`, the min and max values
`min(): index`, `max(): index`, and some convenience methods on any instance of the enumeration.

```
Test: bool = False  # or `Test := bool->False`

# use `isUpperCamelCaseName()` to check for equality:
if Test isTrue()
    print "test is true :("
if Test isFalse()
    print "test is false!"

# get the size (number of enumerated values) of the enum:
print "bool has ${bool->size()} possibilities:"
# get the lowest and highest values of the enum:
print "starting at ${bool->min()} and going to ${bool->max()}"
```

Because of this, it is a bit confusing to create an enum that has `Size` as an
enumerated value name, but it is not illegal, since we can still distinguish between the
enumerated value (`enumName->Size`) and total number of enumerated values (`enumName->size()`).

Also note that the `size()` method will return the total number of
enumerations, not the number +1 after the last enum value.  This can be confusing
in case you use non-standard enumerations (i.e., with values less than 0):

```
sign := enumerate(
    Negative: -1
    Zero: 0
    Positive: 1
)

print "sign has ${sign->size()} values" # 3
print "starting at ${sign->min()} and going to ${sign->max()}"  # -1 and 1

weird := enumerate(
    X: 1
    Y: 2
    Z: 3
    Q: 9
)

print(weird->size())    # prints 4
print(weird->min())     # prints 1
print(weird->max())     # prints 9
```

### Testing enums with lots of values

Note that if you are checking many values, a `consider-case` statement may be more useful
than testing each value against the various possibilities.  Also note that you don't need
to explicitly set each enum value.

```
option := enumerate(
    Unselected
    NotAGoodOption
    ContentWithLife
    BetterOptionsOutThere
    BestOptionStillComing
    OopsYouMissedIt
    NowYouWillBeSadForever
)

print "number of options should be 7:  ${option->size()}"

Option1 := option->ContentWithLife

# avoid doing this if you are checking many possibilities:
if Option1 isNotAGoodOption()
    print "oh no"
else if Option1 isOopsYouMissedIt()
    print "whoops"
...

# instead, consider doing this:
consider Option1
    case NotAGoodOption
        print "oh no"
    case BestOptionStillComing
        print "was the best actually..."
    case Unselected
        fallThrough
    default
        print "that was boring"
```

Note that we don't have to do `option->NotAGoodOption` (and similarly for other options)
along with the `case` keyword.  The compiler knows that since `Option1` is of type `option`,
that you are looking at the different values for `option` in the different cases.

### Default value of an enumeration

Defaults to whatever value is 0 (the first, if no values are specified).

If no value is zero, then the first specified value is default.

## masks

Masks are generalized from enumerations to allow multiple values held simultaneously.
Each value can also be thought of as a flag or option, which are bitwise OR'd together
(i.e., `|`) for the variable instance.  Under the hood, these are unsigned integer types.
Trying to assign a negative value will throw a compiler error.

TODO: allow specifying the integer type backing the mask (or enum), e.g., `enumerate~i64(...)`

```
# the mask is required to specify types that are powers of two:
nonMutuallyExclusiveType := mask(
    # `None: 0` is automatically added as a mask option.
    X: 1
    Y: 2
    Z: 4
    T: 8
)
# it is a compile error if a mask has a power of two that is not the next one higher,
# since that makes the number of values and how to lay them out harder to reason about

# has all the same static methods as enum, though perhaps they are a bit surprising:
nonMutuallyExclusiveType->size() == 16
nonMutuallyExclusiveType->min() == 0
nonMutuallyExclusiveType->max() == 15   # = X | Y | Z | T

Options ;= nonMutuallyExclusiveType()
Options isNone()    # True.  note there is no `hasNone()` method, since that doesn't
                    #        make sense -- either the mask is None or it has something.
Options |= X        # TODO: make sure it's ok to implicitly add the mask type here.
Options |= nonMutuallyExclusiveType Z   # explicit mask type

Options hasX()  # True
Options hasY()  # False
Options hasZ()  # True
Options hasT()  # False
Options isX()   # False, since Options is a combo of X and Z

Options = T
Options isT()   # True, since Options is now solely T
```

## interplay with `oneOf`

We can also create a mask with one or more `oneOf` fields, e.g.:

```
options := mask(
    oneOf([AlignCenterX, AlignLeft, AlignRight])
    oneOf([AlignCenterY, AlignTop, AlignBottom])

    oneOf([FontVerySmall, FontSmall, FontNormal := 0, FontLarge, FontVeryLarge])
)
```

It is a compiler error to assign multiple values from the same `oneOf`:

```
Options; options = AlignCenterX | AlignRight     # COMPILER ERROR!
```

Note that internally, an `OR` combination of the `oneOf` values may actually be valid;
it may be another one of the `oneOf` values in order to save bits.  Otherwise, each
new value in the `oneOf` would need a new power of 2.  For example, we can represent
`oneOf([AlignCenterX, AlignLeft, AlignRight])` with only two powers of two, e.g.,
`AlignCenterX = 4`, `AlignLeft = 8`, and `AlignRight = 12`.  Because of this, there
is special logic with `|` and `&` for `oneOf` values in masks.

```
Options2; options = AlignCenterX
Options2 |= AlignRight    # will clear out existing AlignCenterX/Left/Right first before `OR`ing
if Options2 & AlignCenterX
    print "this will never trigger even if AlignCenterX == 4 and AlignRight == 12."
```

You can also explicitly tell the mask to avoid assigning a power of two to one of the
`oneOf` values by setting it to zero (e.g., `oneOf([..., Value := 0, ... ])`.
For example, the font size `oneOf` earlier could be represented by 3 powers of two, e.g.,
`FontVerySmall = 16`, `FontSmall = 32`, `FontLarge = 64`, `FontVeryLarge = 96`.
Note that we have the best packing if the number of non-zero values is 3 (requiring 2 powers of two),
7 (requiring 3 powers of two), or, in general, one less than a power of two (i.e., `2^P - 1`,
requiring `P` powers of two).

## named value-combinations

Masks can also include shortcuts for various combinations using the `:=` operator, e.g.:

```
myMask := mask(
    X
    Y
    XAndY := X | Y
)
```

# lifetimes and closures

## lifetimes of variables and functions

Variable and function lifetimes are usually scoped to the block that they
were defined in.  Initialization happens when they are encountered, and
descoping/destruction happens in reverse order when the block ends.  With
functions we have to be especially careful when they are impure (i.e.,
defined with a prefix `::` or `;;` for read-only and read-write functions).
If an impure function's lifetime exceeds that of any of its hidden
inputs' lifetimes, we'll get a segfault or worse.

Let's illustrate the problem with an example:

```
# define a re-definable function.  we also allow it to be impure:
;;liveItUp(String); index
    return String size()

if SomeCondition
    SomeIndex; index = 9
    # redefine:
    ;;liveItUp(String); index
        return String size() + ++SomeIndex

    print liveItUp("hi")    # this should print 12
    print liveItUp("ok")    # this should print 13

print liveItUp("no")        # should this print 14 or 2??
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
not allowed.
TODO: maybe we want to allow an impure function to "take" a variable into its own
private scope so that we could return liveItUp here, but that would require some
new syntax.

You can define variables and use them inside impure functions (either `;;` or `::`),
but they must be defined *before* the impure function is first declared.  So this
would be allowed:

```
SomeIndex; index = 9
;;liveItUp(String); index
    return String size()

if SomeCondition
    # redefine:
    ;;liveItUp(String); index
        return String size() + ++SomeIndex

    print liveItUp("hi")    # this should print 12
    print liveItUp("ok")    # this should print 13

print liveItUp("no")        # prints 14
```

Similarly, returning a function from within a function is breaking scope:

```
# this produces a COMPILER error:
nextGenerator(Int; int): ;;(): int
    return ;;():
        return ++Int    # ERROR! lifetime of Int does not exceed returned function!
```

The `Int` here only lives inside of the block when `nextGenerator` is called,
and so it will not be there for the returned function.

TODO: discussion here: should we allow functions to be passed in as if they are pointers?
i think this is fine, but maybe there are some edge cases that allow pointers to escape.
need to double check...

```
# function that takes a function as an argument and returns a function
# this closure is ok because the returned function has internals that have greater lifetimes
# than itself.  i.e., the input function will be destroyed only after the output function
# is descoped.  note that because the returned function is impure, it is prefixed with `::`.
wow(Input->fn(): string): ::(): int
    # example usage:
    # someFn() := "hey"
    # otherFn := wow(someFn)
    # print(otherFn()) # 3
    return (): int
        return Input->fn() size()
```

## the "no pointers" rule

To modify a value that is held by another class instance, e.g., the element of
an array, we can use references (i.e., via passing in with `;`).  However, we
are not allowed to grab a hold of the reference for longer than the
duration of the function call/scope.  We use some syntax sugar in order
to make this not look clumsy.

```
ArrayArray; int__ = [[1,2,3], [5]]
# to modify the array held inside the array, we can use this syntax:
ArrayArray_0 append(4)  # now ArrayArray == [[1,2,3,4], [5]]
# but under the hood, this is converted to something like this:
ArrayArray_(0, (Array; int_): null
    Array append(4)
)
```

This function approach avoids lifetime issues that can arise with pointers,
because the function cannot escape with a reference to the element held
by the class instance.  In languages with pointers, like C++, you could
e.g. get a pointer to an element of an array, and later try to modify
the element via the pointer.  The element might not exist anymore
(e.g., the array was shrunk), and in the worst case, the array
might not even exist (e.g., the array was descoped).

In this way, ownership of another variable is very strict in hm-lang.
Only one object can modify the memory at some location (i.e., of a variable).

## handling system callbacks

We want to allow a `caller`/`callee` contract which enables methods defined
on one class instance to be called by another class instance, without being
in the same nested scope.  (This is strictly regulated, since it is an exception
to the "no pointers" rule.)  The `caller` which will call the callback needs
to be defined before the `callee`, e.g., as a singleton or other instance.
When the `callee` is descoped, it will deregister itself with the `caller`
internally, so that the `caller` will no longer call the `callee`.

```
# caller := { Callees; _(ptr~callee~t), runCallbacks(T: t): for (Ptr) in Callees $(Ptr call(T)) }
# TODO: we probably don't want to allow ; on a type, since ; implies input an output logic,
# it's not technically a type but syntactic sugar on the variable input/output logic.
audio := singleton(caller~(sample_;)) {
    # this `audio` class will call the `call` method on the `callee` class.
    # TODO: actually show some logic for the calling.

    # amount of time between samples:
    DeltaT: flt
}

audioCallee := extend(callee~(sample_;)) {
    Frequency; flt = 440
    Phase; flt = 0

    # TODO: find a way to make it clear this is a fixed-size array
    # but that it doesn't matter what size it is.  in C++, this would
    # be a template type, e.g., `sample_~N`, but we don't actually
    # want a templated method here.
    ;;call(Array; sample_): null
        for Index: index < Array size()
            # TODO: maybe implicitly use `\\math Pi` inside the `\\math sin` function,
            # but only if `Pi` is not defined elsewhere.  i.e., `\\math` becomes a scope
            # which everything is looked up within.
            Array_Index = sample(Mono: \\math sin(2 * Pi * Phase))
            Phase += Frequency * Audio DeltaT

    # automatically defined by the callee class:
    ;;hangUp(): null  
}

someFunction(): null
    Callee; audioCallee
    Callee Frequency = 880
    Audio call(Callee;)
    sleep Seconds: 10
    # `Audio hangUp(Callee;)` automatically happens when `Callee` is descoped.
```


# grammar/syntax

TODO: discussion on parentheses -- we'd like all to be equal () == {} == [], but
our notation might be difficult to distinguish arrays from arguments.
Maybe we assume that we're creating an argument object, unless we are explicitly
casting to an array.

```
# this could be an object with arguments, e.g., {X: X, Int: 100, Y: Y}:
QuestionableChoices := {X, 100, Y}
activate(QuestionableChoices)           # calls `activate` with the arguments object.
NowAnArray := int_(QuestionableChoices) # makes an array of [X, 100, Y]
```

TODO: () or {} or [] are equivalent to Null, or a null object or empty args list.

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
grammarElement := enumerate(
    # "TypeElement" to avoid overload with type/Type
    TypeElement
    FunctionType
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
    EndOfInput
)

tokenMatcher := {
    ;;reset(This Name: str = "") := Null
    # don't have to restore the token array Index to the correct state,
    # consume as many tokens as you like here.  make sure to go
    # through "Grammar match(...)" in order to restore the Index
    # in case of a bad match.  @private so that only Grammar can call.
    @private
    ::match(Index;, Array: token_): bool
}

grammarMatcher := tokenMatcher | grammarElement | token

Grammar := singleton() {
    @private
    Elements: tokenMatcher_grammarElement = [
        TypeElement: oneOf([
            FunctionType
            NonFunctionType
        ])
        VariableName: UpperCamelCase
        VariableDeclaration: sequence([
            VariableName
            optional(operator("?"))
            oneOf([operator(":"), operator(";")])
            TypeElement
        ])
        VariableDefinition: oneOf([
            # VariableName: type = ...
            sequence([VariableDeclaration, operator("="), RhsStatement])
            # VariableName := ...
            sequence([
                VariableName
                oneOf([
                    operator("?;=")
                    operator("?:=")
                    operator(";=")
                    operator(":=")
                ])
                RhsStatement
            ])
        ])
        FunctionName: LowerCamelCase
        FunctionDeclaration: oneOf([
            # `fnName(Args): retType`, `fnName?(Args): retType`, or
            # `fnName(Args)?: retType`, also allowing `;` instead of `:`.
            sequence([
                FunctionName
                FunctionArgsWithReturnType
            ])
            # `fnName: fn(Args): retType` or similar
            sequence([
                FunctionName
                oneOf([operator(":"), operator(";")])
                FunctionType
            ])
        ])
        FunctionArgsWithReturnType: sequence([
            list(FunctionArgument)
            optional(operator("?"))
            oneOf([operator(":"), operator(";")])
            TypeElement
        ])
        FunctionDefinition: oneOf([
            # fnName(Args...): returnType
            #   BlockStatements
            sequence([FunctionDeclaration, Block])
            # (Args...): returnType
            #   BlockStatements
            sequence([FunctionArgsWithReturnType, Block])
            # fnName(Args...) := Statement
            sequence([
                FunctionName 
                list(FunctionArgument)
                oneOf([
                    operator("?;=")
                    operator("?:=")
                    operator(";=")
                    operator(":=")
                ])
                RhsStatement
            ])
            # (Args...) := Statement
            sequence([
                list(FunctionArgument)
                oneOf([
                    operator("?;=")
                    operator("?:=")
                    operator(";=")
                    operator(":=")
                ])
                RhsStatement
            ])
        ])
        FunctionArgument := oneOf([
            VariableDefinition
            VariableDeclaration
            FunctionDefinition
            FunctionDeclaration
        ])
        FunctionCall: sequence([FunctionName, AtomicStatement])
        # TODO: templates, or maybe preprocess these into lowerCamelCase types with hooks
        FunctionType: sequence([
            optional(oneOf([
                operator("::")
                operator(";:")
                operator(":;")
                operator(";;")
            ]))
            optional(identifier("fn"))
            FunctionArgsWithReturnType
        ])
        # TODO: templates, but see above.
        NonFunctionType: oneOf([
            sequence([
                # set types, e.g., `_int` or `_str` and even
                # nested set types e.g., `__int` or `___str`.
                operator("_")
                TypeElement
            ])
            sequence([
                # map types, e.g., `str_int` or `dbl_str`
                # as well as array types, e.g., `str_` or `dbl_`.
                LowerCamelCase
                repeat(operator("_"), AtLeastTimes: 1)
                # present if we're a map type, absent if an array type:
                optional(TypeElement)
            ])
            # simple type, e.g., `int` or `dbl` or `myClassType`
            LowerCamelCase
        ])
        RhsStatement: oneOf([
            AtomicStatement,
            sequence([AtomicStatement, AnyOperator, RhsStatement]),
        ])
        AtomicStatement: oneOf([
            VariableName
            FunctionCall
            parentheses(RhsStatement)
            list(DefinedArgument)
        ])
        ClassName: sequence([
            LowerCamelCase
            optional(TemplateArguments)
        ])
        ExtendParentClasses: sequence([
            keyword("extend")
            list(ClassName)
        ])
        ClassDefinition: sequence([
            ClassName
            oneOf([
                operator(":=")
                doNotAllow(operator(";="), "Classes cannot be mutable.")
                doNotAllow(operator("?;="), "Classes cannot be nullable/mutable.")
                doNotAllow(operator("?:="), "Classes cannot be nullable.")
            ])
            optional(ExtendParentClasses)
            parentheses(Block)
        ])
        EndOfInput: tokenMatcher(
            match(Index;, Array: token_) := Index >= Array size()
        )
    ]

    ::match(Index;, Array: token_, GrammarMatcher): bool
        # ensure being able to restore the current token index if we don't match:
        Snapshot := Index
        Matched := consider GrammarMatcher Is
            case tokenMatcher
                GrammarMatcher match(Index;, Array)
            case grammarElement
                Elements_GrammarMatcher match(Index;, Array)
            case token
                Index < Array size() && Array _ Index++ == Grammar Matcher
        if not Matched
            Index = Snapshot
        return Matched
}

# TODO: actually compiling code will require going through the TokenMatchers
# in a specific order to avoid running through all options to see what fits.
# OR, maybe we can parse a statement into tokens, and then do a `consider Statement`
# with `case RhsStatement` etc., where the hash only takes into account the sequence
# of tokens but not the actual token content.  we'd also need to ignore repeated fields,
# i.e., only count them once.
# TODO: support for labeling token matchers, e.g. "parentClassNames" and "classBlock"

# a list encompasses things like (), (GrammarMatcher), (GrammarMatcher, GrammarMatcher), etc.,
# but also lists with newlines if properly tabbed.
list(GrammarMatcher) := parentheses(repeat([
    GrammarMatcher
    CommaOrBlockNewline
])

sequence := extend(tokenMatcher) {
    Uninterruptible: grammarMatcher_
    # TODO: some annotation to pass a variable up to the parent class,
    # e.g., `reset(@passTo(TokenMatcher) Name: str, OtherArgs...):`
    ;;reset(Name: str, Array; grammarMatcher_):
        tokenMatcher;;reset(Name)
        This Uninterrutible = Array!

    ::match(Index;, Array: token_): bool
        for (GrammarMatcher) in Uninterruptible
            if not Grammar match(Index;, Array, GrammarMatcher)
                return False
        return True
}

# TODO: make `block` a type of token as well.
parentheses := extend(tokenMatcher) {
    ;;reset(Name: str, This GrammarMatcher):
        tokenMatcher;;reset(Name)

    ::match(Index;, Array: token_): bool
        # TODO: make sure copies are elided for constant temporaries like this:
        CurrentToken := Array_Index
        if CurrentToken Is != parenthesesToken
            return False

        InternalIndex; index = 0
        PartialMatch := Grammar match(InternalIndex;, CurrentToken InternalTokens, GrammarMatcher)
        if not PartialMatch
            return False

        # need to ensure that the full content was matched inside the parentheses:
        if InternalIndex < CurrentToken InternalTokens size()
            return False
        
        ++Index
        return True
}

# TODO: make this a function which returns either `repeatInterruptible` and `repeatTimes`
# this is essentially the definition for repeatInterruptible:
repeat := extend(tokenMatcher) {
    Interruptible: GrammarMatcher_
    # until `Until` is found, checks matches through `Interruptible` repeatedly.
    # note that `Until` can be found at any point in the array;
    # i.e., breaking out of the array early (after finding `Until`) still counts as a match.
    # if you need to ensure a non-breakable sequence is found before `Until`,
    # use the `sequence` token matcher inside `Interruptible`.
    ;;reset(Name: str, This Until: GrammarMatcher = EndOfInput, Array: GrammarMatcher_):
        This Interruptible = Array!
        tokenMatcher;;reset(Name)

    ::match(Index;, Array: token_): bool
        if Index >= Array size()
            return False

        while True
            for (GrammarMatcher) in Interruptible
                # always check the escape sequence, Until:
                if Grammar match(Index;, Array, Until)
                    return True
                if not Grammar match(Index;, GrammarMatcher)
                    return False
}
# TODO: make sure the cyclic dependency is ok: i.e., Grammar match being called inside of
# these token matchers...

#(#
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
for Variable := StartingValue, Variable < UpperBoundExclusive
    ... use Variable from StartingValue ...
#)#
ForLoop := oneOf([
    sequence([
        keyword("for")
        VariableDeclaration
        oneOf([operator("<"), operator("<=")])
        Expression
        Block
    ])
    sequence([
        keyword("for")
        list(FunctionArgument)
        keyword("in")
        Expression
        Block
    ])
])

TODO: support internationalization.  do we really require Upper/lower+CamelCase for variables/functions?
or is the syntax unambiguous enough to not need them?

# tokenizer

TODO

# references via keys

TODO: maybe want to separate elements inside a container from container in a consistent way.
e.g., `Array[3]` -> `Key; key~array~int = 3, Array[Key]`.

# object format

Every variable instance has two different storage layouts, one for "only type" and one for
"dynamic type."  "Only-type" variables require no extra memory to determine what type they are.
For example, an array of `i32` has some storage layout for the `array` type, but
each `i32` element has "only type" storage, which is 4 consecutive bytes, ensuring that the
array is packed tightly.  "Dynamic-type" variables include things like objects and instances
that could be one of many class types (e.g., a parent or a child class).  Because of this,
dynamic-type variables include a 64 bit field for their type at the start of the instance,
acting much like a vtable.  However, the type table includes more than just method pointers.

```
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
i.e., all possible child classes.  if we know for certain how files are loaded and used, 
and ensure encapsulation in some way, we can probably update max storage required if a child
class is loaded, and undo the update once the child class is unloaded.

TODO: discuss having all instance methods in some special virtual table, e.g., possibly 
with additional reflection information (for things like `@for (MyClass;;method) in myClass`
macro code).

TODO: make it possible to have template methods with template classes
