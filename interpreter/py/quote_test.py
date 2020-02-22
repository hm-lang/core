#!/usr/bin/env python3
from quote import *
from test import test, testSuite

def testQuoteBuilder():
    errors = 0

    def canFinish():
        qb = QuoteBuilder()
        assert not qb.isComplete()
        index = qb.consume('"hello there!" asdf\n', 0, 1)
        assert index == 14, 'consume did not get to the right index here'
        assert qb.isComplete()
        assert qb.internalParts == ['hello there!']

        qb = QuoteBuilder()
        assert not qb.isComplete()
        index = qb.consume('    "hi! "\n', 4, 5)
        assert index == 10, 'consume did not get to the right index there'
        assert qb.isComplete()
        assert qb.internalParts == ['hi! ']

    errors += test(canFinish)

    def handlesEscapeSequencesCorrectly():
        qb = QuoteBuilder()
        index = qb.consume('"he\\\\lo \\"there!\\"\\n" asdf\n', 0, 1)
        assert index == 21, 'consume did not get to the right index here'
        assert qb.isComplete()
        assert qb.internalParts == ['he\\\\lo \\"there!\\"\\n']

    errors += test(handlesEscapeSequencesCorrectly)

    def expectError(error, fn):
        try:
            qb = QuoteBuilder()
            fn(qb)
        except Exception as e:
            se = str(e)
            assert se == error , 'expected error: "%s", got "%s"'%(error, se)
            return
        raise Exception("expected error: %s"%error)

    def complainsAboutMissingFinalQuote():
        missing = 'a quote "..." must finish on a single line; missing final double-quote symbol.'
        expectError(missing, lambda qb: qb.consume('x = "asdf\n', 0, 5))
        # quote was escaped, so we should still be missing a final quote:
        expectError(missing, lambda qb: qb.consume('    "asd\\"f\n', 4, 5))

    errors += test(complainsAboutMissingFinalQuote)

    return errors

def runTests():
    return testQuoteBuilder() 

if __name__ == '__main__':
    testSuite(runTests)

