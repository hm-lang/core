class StatementError(Exception):
    pass

class StatementBuilder(object):
    def __init__(self):
        self.indent = 0
        self.lines = []
        self.inMultilineComment = False
        self.seenSomething = False

    def addLine(self, line):
        # TODO: add Windows support \r\n.  probably use sys for that.
        if line[-1] != '\n':
            line = line + '\n'

        start = getFirstNonSpaceCharacterIndex(line)
        if not self.seenSomething:
            if start % 4 != 0:
                raise StatementError('lines must indent four spaces')
            if start > 24:
                raise StatementError('please refactor to avoid indents more than 24 spaces')
            self.indent = start
            self.seenSomething = True

        if self.inMultilineComment:
            self.handleMultilineCommentLine(line, start)
        else:
            self.handleNormalLine(line, start)

    def handleMultilineCommentLine(self, line, start):
        if start < self.indent:
            raise StatementError('multiline comment `===` should be indented ' + 
                'greater-equal to the starting line')
        if line.startswith('===', start):
            if start > self.indent:
                # ignore nested multiline comments.
                pass
            else:
                self.inMultilineComment = False

    def handleNormalLine(self, line, start):
        if line.startswith('===', start):
            self.inMultilineComment = True
            return
        line = stripSingleLineComments(line, start)
        assert line[-1] == '\n', 'stripping comments invalidated the invariant'
        if line[start] == '\n':
            if start != len(line) - 1:
                raise StatementError('expected newline only at end of line')
            # this line is blank, do nothing with it
            return
        if line[start] == '\t':
            raise StatementError('no lines can begin with tabs: `\t`')
        self.lines.append(line)

    def isComplete(self):
        # TODO: also check unfinished parentheses.
        return not self.inMultilineComment

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
