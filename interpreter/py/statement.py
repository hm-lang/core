from parenthetical import ParentheticalBuilder

class StatementError(Exception):
    pass

class StatementBuilder(object):
    def __init__(self):
        self.indent = 0
        self.parenthetical = None
        self.inMultilineComment = False
        # TODO: add backtick quote multiline
        self.lines = []

    def addLine(self, line):
        # TODO: possibly make an exception for backtick quotes:
        if len(line) > 137:
            raise StatementError('please refactor to avoid lines longer than 137 chars')

        # TODO: add Windows support \r\n.  probably use sys for that.
        if line[-1] != '\n':
            line = line + '\n'

        indent = getFirstNonSpaceCharacterIndex(line)
        if self.parenthetical is None:
            if indent % 4 != 0:
                raise StatementError('lines must indent four spaces')
            if indent > 24:
                raise StatementError('please refactor to avoid indents more than 24 spaces')
            self.indent = indent
            self.parenthetical = ParentheticalBuilder(indent)

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
        nextIndex = self.parenthetical.consume(line, indent, indent)
        if nextIndex >= 0:
            raise StatementError('line finished an un-opened parenthesis: "%s"'%line[nextIndex])
        self.lines.append(line)

    def isComplete(self):
        return not self.inMultilineComment and self.parenthetical.isComplete()

    def build(self):
        if not self.isComplete():
            raise StatementError('cannot build if not complete!')

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
