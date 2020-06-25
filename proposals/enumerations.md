# Enumerate

We can create a new type that exhaustively declares all possible values it can take.
The syntax is `enum UPPER_SNAKE_CASE_TYPE_NAME` followed by an indented block with
named values, with optional values they take.

## Simple example

```
enum BOOL
    false = 0
    true = 1
```

Enums provide a few extra additional properties for free as well, including the number of
values that are enumerated as well as some convenience methods.

```
BOOL test = false

# use `isUpperCamelCaseName()` to check for equality:
if test.isTrue(), print "test is true :("
if test.isFalse(), print "test is false!"

# get the size (number of enumerated values) of the enum:
print "BOOL has ${BOOL.size} possibilities."
```

Because of this, it is illegal to create an `enum` that has `size` as an enumerated
value name.  As a note, `ENUM.size` is not a legitimate value for the enum type;
e.g. this would result in a compile-time failure:

```
BOOL value = BOOL.size  # ERROR!
```

Also note that the static `size` variable will be set to the total number of
enumerations, not the number +1 after the last enum value.  This can be confusing
in case non-standard enumerations are chosen:

```
enum SIGN
    negative = -1
    zero = 0
    positive = 1

print SIGN.size  # prints 3

enum WEIRD
    x = 1
    y = 2
    z = 4
    t = 8

print WEIRD.size # prints 4
```

## Testing enums with lots of values

Note that if you are checking many values, a `consider-case` statement may be more useful:

```
enum OPTION
    unselected
    notAGoodOption
    contentWithLife
    betterOptionsOutThere
    bestOptionStillComing
    oopsYouMissedIt
    nowYouWillBeSadForever

print "number of options should be 7:  ${OPTION.size}"

OPTION option = contentWithLife 

# avoid doing this if you are checking many possibilities:
if option.isNotAGoodOption()
    print "oh no"
...

# instead, do this:
consider option
    case notAGoodOption
        print "oh no"
    case bestOptionStillComing
        print "was the best actually..."
    case unselected # falls through in case of no intermediate line:
    default
        print "that was boring"
```

See [consider case statements](./consider_case.md).  As seen in this example, you don't
need to explicitly set each enum value.


## Default value of an enumeration

Defaults to whatever value is 0 (the first, if no values are specified).

If no value is zero, then the first specified value is default.
