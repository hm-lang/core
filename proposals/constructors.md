# Constructors

A constructor is used to initialize an instance of a class.  We use the `from`
keyword inside of a class body to define the constructor.  We require the use of 
`this` inside a constructor to access any member variables, so that passed-in 
parameters can have variable names that shadow member variables.  That should 
make the following code at least a bit more clear:

```
    class SOME_CLASS
        INT value
        from(INT value)
            this.value = value
```


It may be helpful to have constructors that can immediately set the value of some
member variable based on a passed-in parameter.  Note that the constructor line
ends with a semicolon because no further logic in an indent is required (for this
class; your classes may require other initialization):

```
    class SOME_CLASS
        # INT value -- automatically defined and assigned based on this:
        from(INT this.value);
```

Of course, it becomes a bit ambiguous if multiple constructor
overloads have other parameters that they set, but these can all be 
defined, but not necessarily assigned in other constructors.  (Of course,
they will be default-initialized in constructors which do not specify them.)

```
    class SOME_CLASS
        from(INT this.x);
        from(INT this.y);
        from(INT this.x, INT this.z)
            this.y = this.x - this.z

    # equivalent to this:

    class SOME_CLASS
        INT x
        INT y
        INT z
        from(INT x)
            this.x = x
        from(INT y)
            this.y = y
        from(INT x, INT z)
            this.x = x
            this.z = z
            this.y = this.x - this.z
```

## Constructor overloads

Multiple constructors follow the same overload patterns as other functions.
Use the first overload that has all of the variables that you are specifying.

## Default constructor

The first public constructor can act as a default constructor.  If there
is no public constructor, any attempt to construct the class will fail,
including default construction.  It is generally a good idea to have a public
constructor for your class, but there are certainly some good exceptions.

```
    class SOME_CLASS
        from(STRING this.name, INT this.age)
            print("Hi ${this.name}, it looks like you are ${this.age} years old!")

    # this will trigger the default constructor:
    SOME_CLASS someClass
    # which prints "Hi , it looks like you are 0 years old!"
    # since the default initialization for a string is the empty string,
    # and the default initialization for an integer is zero.
    print(someClass.name)   # prints empty string
    print(someClass.age)    # prints zero
```
