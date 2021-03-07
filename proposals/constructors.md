# Constructors

A constructor is used to initialize an instance of a class.  We use the `from`
keyword inside of a class body to define the constructor.  We require the use of 
`This` inside a constructor to access any member variables, so that passed-in 
parameters can have variable names that shadow member variables.  That should 
make the following code at least a bit more clear:

```
class someClass()
    int Value
    from(int Value)
        This.Value = Value
```

It is helpful to have constructors that can immediately set the value of some
member variable based on a passed-in parameter.  Note that the constructor line
ends with a semicolon because no further logic in an indent is required (for this
class; your classes may require other initialization):

```
class someClass()
    # int Value -- automatically defined and assigned based on this:
    from(int This.Value);
```

Of course, it becomes a bit ambiguous if multiple constructor
overloads have other parameters that they set, but these can all be 
declared, but not necessarily assigned in other constructors.  (Of course,
they will be default-initialized in constructors which do not specify them.)

```
class someClass()
    from(int This.X);
    from(int This.Y);
    from(int This.X, int This.Z)
        This.Y = This.X - This.Z

# equivalent to this:

class someClass()
    int X
    int Y
    int Z
    from(int X)
        This.X = X
    from(int Y)
        This.Y = Y
    from(int X, int Z)
        This.X = X
        This.Z = Z
        This.Y = This.X - this.Z
```

## Constructor overloads

Multiple constructors follow the same overload patterns as other functions.
Use the first overload that has all of the variables that you are specifying.

## Default constructor

The first public constructor will also act as a default constructor.  If there
is no public constructor, any attempt to construct the class will fail,
including default construction.  It is generally a good idea to have a public
constructor for your class, but there are some exceptions, e.g. the `Ref` type.

```
class someClass()
    from(string This.Name, int This.Age)
        print("Hi ${This.Name}, it looks like you are ${This.Age} years old!")

# this will trigger the default constructor:
someClass SomeClass
# which prints "Hi , it looks like you are 0 years old!"
# since the default initialization for a string is the empty string,
# and the default initialization for an integer is zero.
print(SomeClass.Name)   # prints empty string
print(SomeClass.Age)    # prints zero
```
