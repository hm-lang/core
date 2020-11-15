# Enumerate

We can create a new type that exhaustively declares all possible values it can take.
The syntax is `enum UpperCamelCase` followed by an indented block with named values
(each an `UpperCamelCase` identifier), with optional values they take.  Enumerations
are mutually exclusive -- no two values may be held simultaneously.  See
[masks](./masks.md) for a similar class type that allows multiple options at once.

## Simple example

```
enum Bool
    False = 0
    True = 1
```

Enums provide a few extra additional properties for free as well, including the number of
values that are enumerated via the static method `count()`, the min and max values `min()`, `max()`,
and some convenience methods on any instance of the enumeration.

```
bool Test = False

# use `isUpperCamelCaseName()` to check for equality:
if Test.isTrue()
    print "test is true :("
if Test.isFalse()
    print "test is false!"

# get the size (number of enumerated values) of the enum:
print "Bool has ${bool.count()} possibilities:"
# get the lowest and highest values of the enum:
print "starting at ${bool.min()} and going to ${bool.max()}"
```

Because of this, it is a bit confusing to create an `enum EnumName` that has `Count` as an
enumerated value name, but it is not illegal, since we can still distinguish between the
enumerated value (`enumName.Count`) and total number of enumerated values (`enumName.count()`).

Also note that the static `count()` method will be set to the total number of
enumerations, not the number +1 after the last enum value.  This can be confusing
in case non-standard enumerations are chosen:

```
enum Sign
    Negative = -1
    Zero = 0
    Positive = 1

print "sign has ${sign.count()} values" # prints 3 for count()
print "starting at ${sign.min()} and going to ${sign.max()}" # -1 and 1

enum Weird
    X = 1
    Y = 2
    Z = 3
    Q = 9

print weird.count() # prints 4
print weird.min() # prints 1
print weird.max() # prints 9
```

## Testing enums with lots of values

Note that if you are checking many values, a `consider-case` statement may be more useful
than testing each value against the various possibilities.  Also note that you don't need
to explicitly set each enum value.

```
enum Option
    Unselected
    NotAGoodOption
    ContentWithLife
    BetterOptionsOutThere
    BestOptionStillComing
    OopsYouMissedIt
    NowYouWillBeSadForever

print "number of options should be 7:  ${option.count()}"

option Option1 = ContentWithLife

# avoid doing this if you are checking many possibilities:
if Option1.isNotAGoodOption()
    print "oh no"
elif Option1.isOopsYouMissedIt()
    print "whoops"
...

# instead, consider doing this:
consider Option1
    case NotAGoodOption
        print "oh no"
    case BestOptionStillComing
        print "was the best actually..."
    case Unselected # falls through in case of no intermediate line:
    default
        print "that was boring"
```

See [consider case statements](./consider_case.md).


## Default value of an enumeration

Defaults to whatever value is 0 (the first, if no values are specified).

If no value is zero, then the first specified value is default.

TODO: Consider allowing something like `default.Option = NotAGoodOption`
