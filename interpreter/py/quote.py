class QuoteError(Exception):
    pass

class QuoteBuilder(object):
    def __init__(self):
        # TODO support backtick quotes '`', double quotes '"', and single quotes "'"
        self.internalParts = []
        self.completed_ = False

    def consume(self, line, indent, start):
        assert line[start - 1] == '"', 'invalid state in QuoteBuilder!'
        assert line[-1] == '\n', 'Line should end with newline character'
        assert not self.isComplete(), 'QuoteBuilder is already complete'

        end = start
        while end < len(line):
            specialQuoteIndex = getNextSpecialQuoteIndex(line, end)

            if specialQuoteIndex < 0:
                raise QuoteError('a quote "..." must finish on a single line; missing final double-quote symbol.')

            if line[specialQuoteIndex] == '\\':
                end = specialQuoteIndex + 2
            elif line[specialQuoteIndex] == '"':
                end = specialQuoteIndex
                self.internalParts.append(line[start:end])
                self.completed_ = True
                return specialQuoteIndex + 1

    def build(self):
        # TODO
        return self

    def isComplete(self):
        return self.completed_

# TODO: add support for $:
import re
specialQuoteTypes = ['\\', '"']
specialQuoteTypesRe = re.compile('[\\\\"]')
def getNextSpecialQuoteIndex(line, start):
    match = specialQuoteTypesRe.search(line, start)
    if match is None:
        return -1
    return match.start()
