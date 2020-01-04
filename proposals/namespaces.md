# Namespaces

Namespaces are automatic in hm-lang, based on the directory structure
of files.  If you want to import a class or function from another file
e.g. in a file `file.hm` in the subdirectories `some/subdir`, we use
this syntax `:some.subdir.file` (without the `.hm` extension).  In case
of needing to go up to the parent directory, a double `::` can be used,
e.g. `::some.other.file` (corresponds to file `../some/other/file.hm`).
A third colon `:::far.back` corresponds to `../../far/back.hm`, and so on.
It is recommended to avoid using multiple colons in your code structure.

There are a few options when importing external classes/functions.

1. Import the class/function using this inline notation:
   ```
   # inline where the external function/class is needed:
   :some.subdir.file.neededFunction()
   :some.subdir.file.NEEDED_CLASS neededClassInstance
   ```
   The benefit of this notation is indicating the external usage of a
   file in the same spot that it is used, though the notation is a bit
   verbose.

1. Import the needed classes/functions using this syntax:
   ```
   # near the top of the file:
   import [neededFunction, NEEDED_CLASS] from :some.subdir.file

   # somewhere later:
   neededFunction()
   NEEDED_CLASS neededClassInstance
   ```
   The benefit of this notation is that we indicate what external
   features we need, even though the code localization is bad.

1. Include the file in its entirety using this syntax:
   ```
   # near the top of the file:
   HM someFile = :some.subdir.file

   # somewhere later:
   someFile.neededFunction()
   someFile.NEEDED_CLASS neededClassInstance
   ```
   This syntax is great for indicating what external features are
   needed from a given file, and lowering the inline verbosity at
   the expense of some nonlocality (i.e., the `HM = :...` import).

See also public/protected/private for some discussion about accessing
protected/private functions or class methods based on namespaces.
