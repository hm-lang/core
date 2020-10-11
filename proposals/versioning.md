# Versioning

hm-lang will try to make updating your code very simple.
In most languages, you need to update your code to match the latest language spec,
but in hm-lang, the language will update your code automatically.

Proposal:  tag each file with a version number after a successful compile, with
the smallest language version that should compile that code correctly, e.g.
`#!/usr/bin/env -S hm -v 0.12` as a header.  When converting a file and a deprecated
method is found, the hm compiler will automatically update the method, and update
the header.
