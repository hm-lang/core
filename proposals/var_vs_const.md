# Constant vs. mutable types

Variables which cannot be modified have `const` (constant) types.
In a class, note that `const` variables are allowed to be assigned
or initialized once, i.e., in the constructor, but can never be
reassigned/modified thereafter.  Constant types are preceded with
`const...` concatenated into the type, e.g. `constInt` to indicate
a constant `int` variable.  Variables which can be modified are
technically `var` types (*variable* in the adjective sense), but
this is the default for non-functions:

```
int X = 3

X = 4   # OK

constString Y = "hello!"

Y = "oops!" # ERROR! `Y` cannot be reassigned.
```

Functions are `const` by default, so they require a `var` out yonder
if they are allowed to be modified:

```
Int doSomething(Int)
    return Int + 5

doSomething = $Int + 3  # ERROR! `doSomething` cannot be reassigned.

Int doAnotherThing(String) var
    return String.size()

doAnotherThing = $String.size() + 1   # OK
```

See [functions](./functions.md) for more info in this regard.


## Auto types

We may want to allow setting variables where we know the return type
automatically:

```
const X = 5     # X's type is inferred to be constInt

var Y = 1.2     # Y's type is inferred to be varDbl
```

This can backfire if the default return-type of an overloaded function
(i.e., the first overload that matches your function signature) is
not what you expect.


## Advanced type: Deeply constant

In case of a more complicated type, we use the syntax `someType const`
to indicate that `someType` should be deeply constant.  For example,
`int? ref const` indicates a constant `int maybe ref` type.  No one is allowed
to change the reference (the `ref`), nor the `int maybe` value (if it exists).


## Container const-ness

Suppose we have an array of some other type.  We have a few
different ways we can use `const` (or not), and they each have
a different nuance:

Here we create an array that can be modified in any way, including
being reassigned:

```
Int array Int_Array = [10, 9, 8]
Int_Array.append(7)  # OK
Int_Array[2] = -1    # OK
Int_Array.shift()    # OK
Int_Array = [1, 2]   # OK
print(Int_Array[99]) # OK, resizes to 100 and lazily initializes the missing elements
```

Here we create an array whose internal values can be modified,
but whose size is fixed, and which cannot be reassigned:

```
Int constArray Int_ConstArray = [1, 2, 3, 4]
Int_ConstArray.append(7)    # ERROR, cannot change size!
Int_ConstArray[2] = -1      # OK, individual elements aren't const.
Int_ConstArray.shift()      # ERROR, cannot change size!
Int_ConstArray = [1, 2]     # ERROR, cannot reassign!
print(Int_ConstArray[99])   # ERROR, cannot resize!
```

Here we create an array where the values can't be changed after
they are set.  However, because array modifications can be
performed, we can end up removing old values and inserting new
elements in the same spot.  We don't want to disallow moving
`const` types, since we would still have similar issues if people
popped from the end of the array and pushed new values.

```
constInt array ConstInt_Array = [1, 3, 5, 7]
ConstInt_Array.push(7)      # OK
ConstInt_Array[2] = -1      # ERROR, individual elements are const
ConstInt_Array.shift()      # OK
ConstInt_Array = [1, 2]     # OK
print(ConstInt_Array[99])   # OK, resizes to 100 and gives a default int
ConstInt_Array[98] = 5      # OK, this is the first time it is assigned.
```

Here we create a deeply constant array; i.e. the values cannot be
modified and the array cannot be modified.

```
constInt constArray ConstInt_ConstArray = [3, 2, 1]
ConstInt_ConstArray.push(7)     # ERROR
ConstInt_ConstArray[2] = -1     # ERROR
ConstInt_ConstArray.shift()     # ERROR
ConstInt_ConstArray = [1, 2]    # ERROR
print(ConstInt_constArray[99])  # ERROR
ConstInt_ConstArray[98] = 5     # ERROR
print(ConstInt_ConstArray[1])   # ok
```

Note that this shorthand may be desirable for the deeply constant
version:

```
Int Array const Int_Array_Const = [-1, 5, 11]
```

The arguments above are similar in case the contained type
is some non-primitive type, e.g. `class vector2(dbl X, dbl Y);`.
In the case that the contained type is `const`, e.g.
`vector2 const`, then you cannot reassign or change the instance,
i.e. `Vector2.X = 3` is disallowed.
