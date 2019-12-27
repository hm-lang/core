# Constant variables

Variables which cannot be modified have `CONST` (constant) types.
In a class, note that `CONST` variables are allowed to be assigned
or initialized once, i.e., in the constructor, but can never be
reassigned/modified (even in the constructor).  Constant types
are preceded with `CONST_` concatenated into the type, e.g.
`CONST_INT` to indicate a constant `INT` variable.  

In case of a more complicated type, we use the syntax `SOME_TYPE CONST`
to indicate that `SOME_TYPE` should be constant.  For example,
`INT? CONST` indicates a constant `INT MAYBE` type.  No one is allowed
to change the reference (the `MAYBE`), nor the `INT` value (if it exists).
