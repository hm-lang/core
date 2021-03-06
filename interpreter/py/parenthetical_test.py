#!/usr/bin/env python3
from parenthetical import *
from test import test, testSuite

def testParentheticalBuilder():
    errors = 0

    def canBeCompleteAnytimeIfNoOpenParen():
        pb = ParentheticalBuilder()
        assert pb.isComplete()
        pb.consume('hello\n', 0)
        assert pb.isComplete()
        pb.consume('        + 5\n', 8)
        assert pb.isComplete()

    errors += test(canBeCompleteAnytimeIfNoOpenParen)

    def signalsIncompleteInsideParenthesesStartingWithNoOpenParen():
        pb = ParentheticalBuilder()
        assert pb.isComplete()
        pb.consume('5 + 3\n', 0)
        assert pb.isComplete()
        pb.consume('        *(\n', 8)
        assert not pb.isComplete()
        pb.consume('            13 + [4]\n', 12)
        assert not pb.isComplete()
        pb.consume('        )\n', 8)
        assert pb.isComplete()
        # still ok:
        pb.consume('        + 5\n', 8)
        assert pb.isComplete()

    errors += test(signalsIncompleteInsideParenthesesStartingWithNoOpenParen)

    def ignoresTextBeforeIndent():
        pb = ParentheticalBuilder()
        pb.consume('))))hello\n', 4)
        assert pb.isComplete()

    errors += test(ignoresTextBeforeIndent)

    def finishesAtClosedParenthesis():
        pb = ParentheticalBuilder(0, '(')
        assert not pb.isComplete()
        index = pb.consume('    hello\n', 4)
        assert not pb.isComplete() and index == -1, 'asdf'
        index = pb.consume(')\n', 0)
        assert pb.nextBuilder is None
        assert pb.isComplete() and index == 1

        pb = ParentheticalBuilder(8, '[')
        assert not pb.isComplete()
        index = pb.consume('        hello] - 5\n', 8)
        # TODO: make better asserts, these are annoying to debug
        assert index == 14
        assert pb.isComplete()

        pb = ParentheticalBuilder(4, '{')
        assert not pb.isComplete()
        index = pb.consume('        1 + (5)\n', 8)
        assert not pb.isComplete() and index == -1
        index = pb.consume('    } + 12345\n', 4)
        assert index == 5
        assert pb.isComplete()

    errors += test(finishesAtClosedParenthesis)

    def expectError(error, fn):
        try:
            parentheticalBuilder = ParentheticalBuilder()
            fn(parentheticalBuilder)
        except Exception as e:
            se = str(e)
            assert se == error , 'expected error: "%s", got "%s"'%(error, se)
            return
        raise Exception("expected error: %s"%error)

    def complainsAboutSeeingClosedParenthesisFirst():
        def expectAngry(fn, closeParen):
            expectError('cannot close parentheses with %s, no open parenthesis.'%closeParen,
                    fn)

        expectAngry(lambda pb: pb.consume('x = ] * 30000\n', 0), ']')
        expectAngry(lambda pb: pb.consume('    ) / 50000\n', 4), ')')
        expectAngry(lambda pb: pb.consume('        y +1}\n', 8), '}')

    errors += test(complainsAboutSeeingClosedParenthesisFirst)

    def complainsAboutMismatchedParentheses():
        def expectMismatchError(fn, openParen, closeParen):
            expectError('mismatched parentheses: %s -> %s'%(openParen, closeParen), fn)

        expectMismatchError(lambda pb: pb.consume('x = 3 + (4 / 5]\n', 0), '(', ']')
        expectMismatchError(lambda pb: pb.consume('x = (4 / 5 - 1}\n', 0), '(', '}')
        expectMismatchError(lambda pb: pb.consume('x = {1/[500)+1}\n', 0), '[', ')')
        expectMismatchError(lambda pb: pb.consume('x = [1/500 - 1}\n', 0), '[', '}')
        expectMismatchError(lambda pb: pb.consume('x = {1/5 + 100]\n', 0), '{', ']')
        expectMismatchError(lambda pb: pb.consume('x = {1000/5)+12\n', 0), '{', ')')

        def mismatchParenthesesOnMultipleLines(pb):
            pb.consume('x = {    \n', 0)
            pb.consume('    5 + (\n', 4)
            pb.consume('        1\n', 8)
            pb.consume('    }    \n', 4)
            pb.consume(')        \n', 0)

        expectMismatchError(mismatchParenthesesOnMultipleLines, '(', '}')

    errors += test(complainsAboutMismatchedParentheses)

    def complainsAboutMismatchedParentheticalIndents():
        def expectMismatchError(fn, openParen, closeParen):
            expectError('indent of opening %s must match indent of closing %s'%(
                    openParen, closeParen), fn)

        def mismatchIndentsIncreasing(pb):
            pb.consume('x = {      \n', 0)
            pb.consume('    5 + (  \n', 4)
            pb.consume('        100\n', 8)
            pb.consume('        )  \n', 8)
            pb.consume('}          \n', 0)

        expectMismatchError(mismatchIndentsIncreasing, '(', ')')

        def mismatchIndentsDecreasing(pb):
            pb.consume('x = {                 \n', 0)
            pb.consume('    5 + (             \n', 4)
            pb.consume('        100 * [100 / 3\n', 8)
            pb.consume('    ])                \n', 4)
            pb.consume('}                     \n', 0)

        expectMismatchError(mismatchIndentsDecreasing, '[', ']')

    errors += test(complainsAboutMismatchedParentheticalIndents)

    # TODO: test internalParts of parentheticals being created correctly

    return errors

def runTests():
    return testParentheticalBuilder() 

if __name__ == '__main__':
    testSuite(runTests)
