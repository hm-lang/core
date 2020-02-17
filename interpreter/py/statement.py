class StatementError(Exception):
    pass

class StatementBuilder(object):
    def __init__(self):
        self.indent = 0
        self.lines = []
        self.inMultilineComment = False
        # TODO: add backtick quote multiline
        self.seenSomething = False
        self.parenWithIndentStack = []

    def addLine(self, line):
        # TODO: possibly make an exception for backtick quotes:
        if len(line) > 137:
            raise StatementError('please refactor to avoid lines longer than 137 chars')

        # TODO: add Windows support \r\n.  probably use sys for that.
        if line[-1] != '\n':
            line = line + '\n'

        indent = getFirstNonSpaceCharacterIndex(line)
        if not self.seenSomething:
            if indent % 4 != 0:
                raise StatementError('lines must indent four spaces')
            if indent > 24:
                raise StatementError('please refactor to avoid indents more than 24 spaces')
            self.indent = indent
            self.seenSomething = True

        if self.inMultilineComment:
            self.handleMultilineCommentLine(line, indent)
        else:
            self.handleNormalLine(line, indent)

    def handleMultilineCommentLine(self, line, indent):
        if indent < self.indent:
            raise StatementError('multiline comment `===` should be indented ' + 
                'greater-equal to the starting line')
        if line.startswith('===', indent):
            if indent > self.indent:
                # ignore nested multiline comments.
                pass
            else:
                self.inMultilineComment = False

    def handleNormalLine(self, line, indent):
        if line.startswith('===', indent):
            self.inMultilineComment = True
            return
        line = stripSingleLineComments(line, indent)
        assert line[-1] == '\n', 'stripping comments invalidated the invariant'
        if line[indent] == '\n':
            if indent != len(line) - 1:
                raise StatementError('expected newline only at end of line')
            # this line is blank, do nothing with it
            return
        if line[indent] == '\t':
            raise StatementError('no lines can begin with tabs: `\t`')
        self.addNormalLine(line, indent)

    def addNormalLine(self, line, indent):
        start = indent
        while start < len(line):
            parenIndex = getNextParenIndex(line, start)
            if parenIndex < 0:
                break
            self.handleParen(line[parenIndex], indent)
            start = parenIndex + 1
        self.lines.append(line)

    def handleParen(self, paren, indent):
        parens = ['(', '[', '{', ')', ']', '}']
        parenType = parens.index(paren)
        if parenType < 3: # open paren
            self.parenWithIndentStack.append((parenType, indent))
        else: # closed paren
            lastOpenParenType, lastOpenParenIndent = self.parenWithIndentStack.pop()
            if lastOpenParenType + 3 != parenType:
                raise StatementError('mismatched parentheses: %s -> %s'%(
                    parens[lastOpenParenType], parens[parenType]))
            if indent != lastOpenParenIndent:
                raise StatementError('indent of opening %s must match indent of closing %s'%(
                    parens[lastOpenParenType], parens[parenType]))

    def isComplete(self):
        return not self.inMultilineComment and len(self.parenWithIndentStack) == 0

""" Returns the first non-space character index for the line """
def getFirstNonSpaceCharacterIndex(line):
    i = 0
    while i < len(line):
        if line[i] != ' ':
            break
        i += 1
    return i

def stripSingleLineComments(line, start):
    commentIndex = line.find('#', start)
    if commentIndex < 0:
        return line
    if line[commentIndex + 1] != '/':
        return line[:commentIndex] + '\n'
    endMidlineCommentIndex = line.find('/#', commentIndex + 2)
    if endMidlineCommentIndex < 0:
        raise StatementError('midline comments #/ ... /# must end on the same line')
    return line[:commentIndex] + stripSingleLineComments(line[endMidlineCommentIndex+2:], 0)

import re
parenRe = re.compile('[\\(\\)\\[\\]{}]')
def getNextParenIndex(line, start):
    match = parenRe.search(line, start)
    if match is None:
        return -1
    return match.start()
