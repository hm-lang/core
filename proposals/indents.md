# Indenting

hm-lang takes a side on the space vs. tab debate, and even a stance on
how many spaces are required for an indent.  There is also a hard limit
on the maximum allowed number of indents.  (Refactor your code if it
has 5 or more levels of indentation.)  An indent consists of four spaces:

```
# indent 0
INT x = 0

FN doSomething()
    # indent 1
    print("something was done")

for INT i < 10
    for INT j < 100
        # indent 2
        print("${i}, ${j}")

# indent 0
class EXAMPLE
    # indent 1
    DBL MD doSomething(INT)
        # indent 2
        if int % 3 == 0
            # indent 3
            if int % 5 == 0
                # indent 4.  NO MORE ALLOWED AFTER THIS!
                return 1.125
            return 1.0625
        return 1.0        
```
