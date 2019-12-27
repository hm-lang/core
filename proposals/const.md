# Constant variables

Variables which cannot be modified have `CONST` (constant) types.
In a class, note that `CONST` variables are allowed to be assigned
or initialized once, i.e., in the constructor, but can never be
reassigned/modified (even in the constructor).  Constant types
are preceded with `CONST_` concatenated into the type, e.g.
`CONST_INT` to indicate a constant `INT` variable.  

In case of a more complicated type, we can also use parentheses to
group what is not modifiable together, e.g. `CONST(INT?)` to
indicate a constant `INT MAYBE` type.  We may want to cascade the
`CONST` to all types covered by the parentheses, e.g.
`CONST(INT?)` --> `CONST_INT CONST_MAYBE`, so that the user is not
allowed to change the reference (`MAYBE`) nor the underlying type (`INT`).
