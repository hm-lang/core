# Function keywords

We can create a function using the `FN` keyword (FunctioN), whose
definition is based on the following indent+1 block:

```
    FN hello(STRING noun)
        print("hello, ${noun}!")

    hello("world")
```

We also make it easy to create (an unnamed) lambda function, e.g. for testing:

```
    TEST someBehavior(FN()
        INT x = 3
        INT y = 12
        assert(x * y == 36, OrThrow "That was unexpected")
    )
```

We also have `MD` for a class method (MethoD, just like FunctioN).  Method
functions can only be invoked on an instance of the class, and they have
access to a `this`.

```
    class SOME_CLASS(INT this.value)
        # method: can access `this`, e.g. `this.value`:
        INT MD doSomething(STRING)
            return string.size + 5 * value

    SOME_CLASS anInstance(Value(3))
    anInstance.doSomething("blah")  # ok, using class method on an instance.
```

To define a static function, we use the `CLASS_FN` keyword.  These can be
invoked without an instance of the class, but they are not allowed to access
a `this` (since they are not a class instance method).  

```
    class SOME_CLASS(INT this.value)
        # static function; does not have access to `this`:
        INT CLASS_FN getSize(STRING)
            return string.size

    # can be used without an instance:
    SOME_CLASS.getSize("chamber")   # ok, using static function of class.
```

Trying to access `this` in a (static) class function will throw a compile error.

```
    class ERROR_CLASS(INT this.value)
        INT FN getGreatValue()
            return value * 1234 # ERROR!  this.value is a member variable.
```

Use the `MD` keyword instead for class methods, which operate on the instance.
Methods do not have to reference `this`.  The special keyword `MD` also helps
us remember that child classes can modify the method.

## CONST-ness and more examples

Functions are by default `CONST`, i.e. they cannot be changed.

```
    FN hello()
        print("hello, world!")

    # this errors:
    hello = FN()
        print("hey, you!")
    # ERROR!  functions are constant/non-mutable by default.
```

If you want to define a function which can change, use the 
[`VAR`](./var_vs_const.md) keyword:

```
    VAR_FN hello()
        print("hello, world!")

    hello = FN()
        print("hey, you!")
    # ok!  hello is still mutable, based on its original definition.

    hello = VAR_FN()
        print("hi, friend.")
    # also ok.
```

We can also pass in functions to be member variables of classes.

```
    #
    # define a class with a lambda function which cannot be modified:
    #
    class FN_CLASS(INT FN this.hash(STRING));

    # instantiate the class
    FN_CLASS f(INT FN hash(STRING)
        ...
        return 7
    )

    # ERROR!  this fails, since hash is a constant function, `FN`.
    f.hash = INT FN(STRING)
        ...
        return 3

    #
    # define a class with a modifiable lambda function:
    #
    class VAR_FN_CLASS(INT VAR_FN this.hash(STRING));

    # instantiate the class
    VAR_FN_CLASS u(INT VAR_FN hash(STRING)
        ...
        return 5
    )

    # this is ok:
    u.hash = INT FN(STRING)
        ...
        return 3

    #
    # define a class with a hash method:
    #
    class METHOD_CLASS(INT this.value)
        INT MD hash(STRING)
            return value * string.size

    # instantiate:
    METHOD_CLASS m(3)

    # ERROR!  this fails.  since hash is a member function, 
    # i.e. class method, it cannot be changed.
    m.hash = INT FN(STRING)
        return 1234
```

Also, for completeness, there is no such thing as a mutable method,
nor a mutable static method:

```
    class WEIRD
        # errors:
        VAR_MD hey(STRING)
            print("hi, ${string}")
        # ERROR!  Methods are already mutable by derived classes,
        # but cannot otherwise by reassigned.

        VAR_CLASS_FN bad()
            print("NOT ALLOWED")
        # ERROR!  class functions are not allowed to be changed.
```

We can also define lambda functions inside a class.

```
    class EXAMPLE
        # define a function, but initialize in the constructor:
        INT FN get(DBL);

        from(INT this.value)
            if this.value == 0
                # OK since `this.get` hasn't been initialized yet:
                this.get = INT FN(DBL)
                    return dbl.sign
            else
                # OK since `this.get` hasn't been initialized yet:
                this.get = INT FN(DBL)
                    return this.value.abs * dbl

    EXAMPLE example(Value(0))
    print(example.get(1234))    # prints 1
    print(example.get(-1337))   # prints -1
    print(example.get(0))       # prints 0
```

If no function is specified in the constructor, a default value
will be chosen; usually it is a zero function.  I.e., all inputs
will be ignored, and all outputs will be set to their default
initialization.
