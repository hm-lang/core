# Classes

## Three types of class functions

### Methods

We have functions, called methods, which are defined on classes and
have access to the `This` instance of the class; i.e., they can
access all the member variables and call other methods of the instance.

TODO: remove the `class X(typeY Z)` format, since that's a C++ holdover
TODO: unless we decide to go with lowercase `class x(typeY Z)` since that's sort of accurate.
```
class SomeClass(int This.Value)
    # method: can access `This`, e.g. `This.Value`:
    Int doSomething(String)
        return String.size() + 5 * Value

someClass AnInstance(Value: 3)
AnInstance.doSomething("blah")  # ok, using class method on an instance.

someClass.doSomething("hey")    # ERROR, method cannot be called on class
```

### Static/class functions

To define a static function, prefix `class.` on the function name.  These can be
invoked without an instance of the class, but they are not allowed to access
a `This` (since they are not a class instance method).  

TODO: make functions that don't refer to a `This` automatically static, as making
`class.new` functions all the time would be annoying.
BUT this would have problems for abstract methods -- what should they be?
obviously we want them to be methods and not static class functions.

```
class SomeClass(int This.Value)
    # static function; does not have access to `This`:
    Int class.getSize(String)
        return String.size()

# can be used without an instance:
someClass.getSize("chamber")   # ok, using static function of class.

# and they can also be used on an instance:
someClass Instance(Value: 5)
Instance.getSize("chalice")     # also ok.
```

Trying to access `This` in a (static) class function will throw a compile error.

```
class ClassWillThrowCompilerError(int This.Value)
    Int class.getGreatValue()
        return Value * 1234 # ERROR!  This.Value is a member/instance variable.
```

Don't use the `class.` prefix for class methods, which operate on the instance.
Methods have `This`, but do not need to explicitly reference it all the time
e.g. using `This.Value`.  Child classes are allowed to modify/override class methods.

### Lambda function variables

We can also define functions to be member variables of classes, using
the `const` and `var` keywords to ensure these are treated as variables
that are constant (immutable) or mutable (i.e., they can be reassigned to
a different function).

TODO: can we make `const` lambda functions and methods indistinguishable from a user's perspective??
      almost:  make sure that any abstract classes get automatic `new` definitions which allow you
      to specify the methods.

## Class method/function/lambda examples

```
#
# define a class with a lambda function which cannot be modified:
#
# TODO: maybe make `const` here optional, since it is clear we are passing
# in a lambda function:
#
class FnClass(Int hash(String) const);    # also ok, `This.hash` for `hash`

# an alternative definition of the same thing:
class FnClass2
    # Note that the `const` keyword is necessary to declare this a lambda function;
    # without `const` this otherwise would be an (abstract) class method:
    Int hash(String) const;

    from(Int hash(String))
        This.hash = hash

# another valid representation of the same class:
class FnClass3
    from(Int This.hash(String) const);

# instantiate the class with an excellent "hash" function, very fast:
fnClass F(Int hash(String) = 7)

# ERROR!  this fails, since hash is a constant function.
F.hash = Int fn(String)
    return 3

#
# define a class with a modifiable lambda function:
#
class VarFnClass(Int hash(String) var);

# instantiate the class
varFnClass U(Int hash(String)
    return 5
)

# this is ok:
U.hash = Int fn(String)
    return 3

#
# define a class with a hash method:
#
class MethodClass(int Value)
    Int hash(String)
        return Value * String.size()

# instantiate:
methodClass M(Value: 3)

# ERROR!  this fails.  since hash is a member function, 
# i.e. class method, it cannot be changed (except by child classes).
M.hash = Int fn(String)
    return 1234
```

Also, for completeness, there is no such thing as a mutable class function:

```
class Weird(int Times)
    # ERROR!  class functions are not allowed to be changed.
    class.bad() var
        print("NOT ALLOWED")

    class.redundant() const
        print("this is redundant, since class functions are constant.")
```

We can also define lambda functions inside a class.  See also
[lambda functions](./lambda_functions.md) for more on the fancy `$` notation.

```
class Example
    # define a mutable lambda function, but initialize in the constructor:
    Int getMutable(Dbl) var;

    # define a constant (once-settable) lambda function, initialized in constructor:
    Int getConstant(Dbl) const;

    from(int This.Value)
        if This.Value == 0
            # OK since `this.getConstant` hasn't been initialized yet:
            This.getConstant = $Dbl.sign()
        else
            # OK since `this.getConstant` hasn't been initialized yet:
            This.getConstant = $Dbl * This.Value.abs()

        This.getMutable = Int fn(Dbl)
            return This.Value + Dbl

example Example(Value: 0)
print(Example.getConstant(1234))    # prints 1
print(Example.getConstant(-1337))   # prints -1
print(Example.getMutable(1))       # prints 1
# ok to change this function:
Example.getMutable = Int fn(Dbl)
    return 5
# compile-time ERROR! getConstant is immutable:
Example.getConstant = $Dbl + 5
```

If no function is specified in the constructor, a default value
will be chosen; usually it is a zero function.  I.e., all inputs
will be ignored, and all outputs will be set to their default
initialization.

