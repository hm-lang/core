# Numbers

Numbers, like `int` (an arbitrary precision, AKA big, integer) and `dbl` (a 64-bit floating point number)
have a few convenience methods with them.  All of them have the following methods:

```
Number.string()         # convert to string
Number.format(String)   # format based on passed-in string
```

## Formatting

Integers can be formatted in various ways; by default, all digits are printed.

```
int X = 1234
print X             # prints 1234
print X.string()    # also prints 1234
print X.format("%05d")  # prints 01234
print X.format("%.2e")  # prints 1.23E3
```

Floats by default print 4 significant figures (for 32-bit precision, i.e. `f32` or `flt`)
or 7 significant figures (for 64-bit `f64` or `dbl` types).  For large-in-magnitude numbers
(more than number of significant figures), we will switch to using exponential form.  We
likely will also use an integer representation if the float (`f32` or `f64`) is equal to
an integer.

```
dbl X = 1234.56789
print X             # prints 1234.567
print X.string()    # also prints 1234.567
print X.format("%.2e")  # prints 1.23E3

X = 123456789
print X     # prints 1.234567e8

X = 1234567.8
print X     # prints 1234567.

X = 1234567.0
print X     # prints 1234567    # no period, this looks like an integer
```

Some examples for floats (`flt32`):

```
flt X = 123456789
print X     # prints 1.234e8

X = 1234.56789
print X     # prints 1234.      # period to show continuation

X = 1234.0
print X     # prints 1234       # no period, this looks like an integer
```
