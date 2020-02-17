#!/usr/bin/env python3
import sys
from statement import *

def testStatementBuilder():
    def test(fn):
        try:
            fn()
            return 0
        except Exception as e:
            print('error in test %s: "%s"'%(fn.__name__, e), file=sys.stderr)
            return 1

    errors = 0

    def setsIndentCorrectly():
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = 3')
        assert statementBuilder.indent == 0

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    y = 3')
        assert statementBuilder.indent == 4

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('        z = 3')
        assert statementBuilder.indent == 8

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('            q = 3')
        assert statementBuilder.indent == 12

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('                class BLAH')
        assert statementBuilder.indent == 16

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('                    INT x = 1')
        assert statementBuilder.indent == 20

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('                        DBL x = 1.0')
        assert statementBuilder.indent == 24

    errors += test(setsIndentCorrectly)

    def expectError(error, fn):
        try:
            statementBuilder = StatementBuilder()
            fn(statementBuilder)
            raise Exception("expected error: %s"%error)
        except StatementError as e:
            se = str(e)
            assert se == error , 'expected error: "%s", got "%s"'%(error, se)

    def complainsAboutLongLines():
        def expectLongLineError(fn):
            expectError('please refactor to avoid lines longer than 137 chars', fn)

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x'*137) # should be ok

        expectLongLineError(lambda sb: sb.addLine('y'*138))
        expectLongLineError(lambda sb: sb.addLine('z'*139))
        expectLongLineError(lambda sb: sb.addLine('w'*300))

    errors += test(complainsAboutLongLines)

    def complainsAboutNonFourSpaceIndents():
        def expectIndentError(fn):
            expectError('lines must indent four spaces', fn)

        expectIndentError(lambda sb: sb.addLine(' x = 3'))
        expectIndentError(lambda sb: sb.addLine('  y = 3'))
        expectIndentError(lambda sb: sb.addLine('   z = 3'))
        expectIndentError(lambda sb: sb.addLine('     INT w = 3'))

    errors += test(complainsAboutNonFourSpaceIndents)

    def complainsAboutHugeIndents():
        def expectIndentError(fn):
            expectError('please refactor to avoid indents more than 24 spaces', fn)

        expectIndentError(lambda sb: sb.addLine('                            x = 3'))
        expectIndentError(lambda sb: sb.addLine('                                y = 3'))

    errors += test(complainsAboutHugeIndents)

    def addsTrailingNewlineIfNotPresent():
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    x = 5')
        assert statementBuilder.lines == ['    x = 5\n']

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('        y = 7\n')
        assert statementBuilder.lines == ['        y = 7\n']

    errors += test(addsTrailingNewlineIfNotPresent)

    def stripsEndOfLineCommentsAndAddsNewlineCharacter():
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = 43 # this is bad news')
        # it's important that a newline is added back:
        assert statementBuilder.lines == ['x = 43 \n']

        # test with indents:
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    y = 100#\n')
        # it's important that a newline is added back:
        assert statementBuilder.lines == ['    y = 100\n']

        # test multiple # inside the end-of-line comment:
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('        z = 100  # asdf # asdf2 # \n')
        # it's important that a newline is added back:
        assert statementBuilder.lines == ['        z = 100  \n']

    errors += test(stripsEndOfLineCommentsAndAddsNewlineCharacter)

    def stripsMidlineComments():
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    #/ ok at front of line /# y = 100')
        # TODO: should readjust indent here:
        assert statementBuilder.lines == ['     y = 100\n']
        assert statementBuilder.indent == 4

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = 4#/ comment /#3')
        # TODO: probably should add a space between 4 and 3 to discourage
        # people doing this sort of thing.
        assert statementBuilder.lines == ['x = 43\n']

        # multiple midline comments, plus final end-of-line comment:
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    x = 43 #/ xyz /# + 53 #/ asdf /# * 3 #/ /q#p/ /# / 5 # hide!')
        assert statementBuilder.lines == ['    x = 43  + 53  * 3  / 5 \n']

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('INT z = 1 #/ xyz /# + 53 # ignore here #/ asdf /# ')
        assert statementBuilder.lines == ['INT z = 1  + 53 \n']

    errors += test(stripsMidlineComments)

    def complainsAboutBadMidlineComments():
        def expectMidlineCommentError(fn):
            expectError('midline comments #/ ... /# must end on the same line', fn)

        expectMidlineCommentError(lambda sb: sb.addLine('x #/   asdf asdf #\n'))
        expectMidlineCommentError(lambda sb: sb.addLine('    y = 2 #/  /# + 5 #/ '))

    errors += test(complainsAboutBadMidlineComments)
   
    def complainsAboutTabIndents():
        def expectTabError(fn):
            expectError('no lines can begin with tabs: `\t`', fn)

        expectTabError(lambda sb: sb.addLine('\tx = 3'))
        expectTabError(lambda sb: sb.addLine('	\ty = 4'))
        expectTabError(lambda sb: sb.addLine('\t\t\tz = 5'))
        expectTabError(lambda sb: sb.addLine('    	\t\tw = 6'))
        expectTabError(lambda sb: sb.addLine('        \tINT p = 9'))

    errors += test(complainsAboutTabIndents)
   
    def understandsMultilineComments():
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('===')
        assert statementBuilder.inMultilineComment
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('===')
        assert not statementBuilder.inMultilineComment
        assert statementBuilder.isComplete()
        assert statementBuilder.lines == []

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    === ignores this')
        assert statementBuilder.inMultilineComment
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    ignores this, too')
        assert statementBuilder.inMultilineComment
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    === ignores this three')
        assert not statementBuilder.inMultilineComment
        assert statementBuilder.isComplete()
        assert statementBuilder.lines == []

        # nested multiline comment fragments ok!
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    ===')
        assert statementBuilder.inMultilineComment
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('     === bad indent ignored')
        assert statementBuilder.inMultilineComment
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('     ===')
        assert statementBuilder.inMultilineComment
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('        ===')
        assert statementBuilder.inMultilineComment
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    ===')
        assert not statementBuilder.inMultilineComment
        assert statementBuilder.isComplete()
        assert statementBuilder.lines == []

    errors += test(understandsMultilineComments)

    def complainsAboutUnindentedLinesInMultilineComments():
        def expectMultilineError(fn):
            expectError('multiline comment `===` should be indented ' + 
                'greater-equal to the starting line', fn)
       
        def unindentMultilineComment1(sb):
            sb.addLine('    ===')
            sb.addLine('    hello!') 
            sb.addLine('this is bad!') 
            sb.addLine('    not a problem') 
            sb.addLine('    ===')
        expectMultilineError(unindentMultilineComment1)

        def unindentMultilineComment2(sb):
            sb.addLine('        ===')
            sb.addLine('    ===')
        expectMultilineError(unindentMultilineComment2)

        def unindentMultilineComment3(sb):
            sb.addLine('        ===')
            sb.addLine('hey')
            sb.addLine('===')
        expectMultilineError(unindentMultilineComment3)
    
    errors += test(complainsAboutUnindentedLinesInMultilineComments)

    def isOkWithCorrectlyMatchedParenthesesOnSingleLine():
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = (5 + 5^(3 * 4 / (6 - 1)))')
        assert statementBuilder.isComplete()

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = [5 + 5^[3 * 4] / [6 - 1]]')
        assert statementBuilder.isComplete()

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('    x = {5 + {5^{3 * 4 / 6} - 1}}')
        assert statementBuilder.isComplete()

        statementBuilder = StatementBuilder()
        statementBuilder.addLine('        x = {5 + [5^(3 * {4} / [6 + 2]) - 1]}')
        assert statementBuilder.isComplete()

    errors += test(isOkWithCorrectlyMatchedParenthesesOnSingleLine)

    def isOkWithCorrectlyMatchedParenthesesOnMultipleLines():
        # test open parens are last on the line, closed parens are last on the line
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = 5 + (')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    3 * {')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('        100 / [')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('            10 + 2 / ')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('            3')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('        ]')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    }')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine(')')
        assert statementBuilder.isComplete()

        # test adding things after the parens
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = [5 + ')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    {1 - ')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('        [ 10 + 3')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('            - 1 + 4 # comment1')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('            - 5 + 1 # comment2')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('        ] ^ 399')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    }/100')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('] - 5')
        assert statementBuilder.isComplete()

        # test adding things before and after the parens
        statementBuilder = StatementBuilder()
        statementBuilder.addLine('x = {5 + ')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    3 * {1 - ')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('        100 / [ 10 + 3')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('            *100 # comment')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('# oops, bad indent comment, but ok')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('            +101 # comment2')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('        -1 ] ^ 399')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('    +5}/100')
        assert not statementBuilder.isComplete()
        statementBuilder.addLine('><2} - 5')
        assert statementBuilder.isComplete()

    errors += test(isOkWithCorrectlyMatchedParenthesesOnMultipleLines)

    def complainsAboutMismatchedParentheses():
        def expectMismatchError(fn, openParen, closeParen):
            expectError('mismatched parentheses: %s -> %s'%(openParen, closeParen), fn)

        expectMismatchError(lambda sb: sb.addLine('x = 3 + (4 / 5]'), '(', ']')
        expectMismatchError(lambda sb: sb.addLine('x = (4 / 5 - 1}'), '(', '}')
        expectMismatchError(lambda sb: sb.addLine('x = {1/[500)+1}'), '[', ')')
        expectMismatchError(lambda sb: sb.addLine('x = [1/500 - 1}'), '[', '}')
        expectMismatchError(lambda sb: sb.addLine('x = {1/5 + 100]'), '{', ']')
        expectMismatchError(lambda sb: sb.addLine('x = {1000/5)+12'), '{', ')')

        def mismatchParenthesesOnMultipleLines(sb):
            sb.addLine('x = {')
            sb.addLine('    5 + (')
            sb.addLine('        100')
            sb.addLine('    }')
            sb.addLine(')')

        expectMismatchError(mismatchParenthesesOnMultipleLines, '(', '}')

    errors += test(complainsAboutMismatchedParentheses)

    def complainsAboutMismatchedParentheticalIndents():
        def expectMismatchError(fn, openParen, closeParen):
            expectError('indent of opening %s must match indent of closing %s'%(
                    openParen, closeParen), fn)

        def mismatchIndentsIncreasing(sb):
            sb.addLine('x = {')
            sb.addLine('    5 + (')
            sb.addLine('        100')
            sb.addLine('        )')
            sb.addLine('}')

        expectMismatchError(mismatchIndentsIncreasing, '(', ')')

        def mismatchIndentsDecreasing(sb):
            sb.addLine('x = {')
            sb.addLine('    5 + (')
            sb.addLine('        100 * [100 / 3')
            sb.addLine('    ])')
            sb.addLine('}')

        expectMismatchError(mismatchIndentsDecreasing, '[', ']')

    errors += test(complainsAboutMismatchedParentheticalIndents)

    return errors

def runTests():
    return testStatementBuilder() 

if __name__ == '__main__':
    errors = runTests()
    if errors:
        print('ran tests, got %d error(s)'%errors, file=sys.stderr)
        sys.exit(1)
    else:
        print('ran tests, no errors!')

