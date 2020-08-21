# Indenting

hm-lang takes a side on the space vs. tab debate, and even a stance on
how many spaces are required for an indent.  There is also a hard limit
on the maximum allowed number of indents.  (Refactor your code if it
has 5 or more levels of indentation.)  An indent consists of four spaces:

```
# indent 0
int X = 0

doSomething()
    # indent 1
    print("something was done")

for int I < 10
    for int J < 100
        # indent 2
        print("${I}, ${J}")

# indent 0
class example
    # indent 1
    Dbl doSomething(Int)
        # indent 2
        if Int % 3 == 0
            # indent 3
            if Int % 5 == 0
                # indent 4.  NO MORE ALLOWED AFTER THIS!
                return 1.125
            return 1.0625
        return 1.0        
```
