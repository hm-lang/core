#!/usr/bin/env python3
# TODO: autogenerate this file
import sys

def test(fn):
    try:
        fn()
        return 0
    except Exception as e:
        print('error in test %s: "%s"'%(fn.__name__, e), file=sys.stderr)
        return 1

def runTests():
    errors = 0

    import parenthetical_test
    print('running parenthetical_test.py...')
    errors += parenthetical_test.runTests()

    import statement_test
    print('running statement_test.py...')
    errors += statement_test.runTests()

    return errors

def testSuite(testFn):
    errors = testFn()
    if errors:
        print('ran tests, got %d error(s)'%errors, file=sys.stderr)
        sys.exit(1)
    else:
        print('ran tests, no errors!')

if __name__ == '__main__':
    testSuite(runTests)
