# Template/generic classes

Defining a generic or templated class has the following syntax.
For a class that will depend on a single type, you can use 
`NEW_IDENTIFIER` where `IDENTIFIER` is an `UPPER_SNAKE_CASE`
name of your choosing.  This effectively declares the new
type, and it appears before the class name, e.g. `class NEW_T MY_CLASS;`.
You can then use `NEW_T` (or whatever identifier) inside the class
as a type declaration:

```
    class NEW_T ARRAY
        ...
        NEW_T MD this[INDEX]
            if index < fixed.size
                return fixed[index]
            throw ERROR("unimplemented!")

        NEW_T! FIXED_ARRAY fixed__

    INT ARRAY x
    print(x[3])
```

For classes with multiple types that should be defined, you can
use any number of `UPPER_SNAKE_CASE` identifiers after the class name
to specify which types are being used in the class.  In the class
body, those identifiers can be used to declare variables of those types.
For example,

```
    class MAP KEY VALUE
        [KEY, VALUE]? ARRAY hashTable__
        ...
        VALUE MD this(KEY)
            ...
```

Named types in a template must be specified, similarly to an
argument specification:

```
    MAP Key(STRING) Value(INT) counter
    counter["blah"] += 1
    counter["heyeye"] += 10
```

Note that any inheritance can be made explicit with the `extends` keyword:

```
    class NEW_T ARRAY extends [INDEX, NEW_T] CONTAINER;

    class MAP KEY VALUE extends [KEY, VALUE] CONTAINER;
```

Right now there are no plans for making a "default" type
in a template class.  Instead, all types must be specified when
instantiating a class which is a generic/template class.
