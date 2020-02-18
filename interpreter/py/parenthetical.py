class ParentheticalError(Exception):
    pass

'''
Part of a Statement.  A bit of a misnomer when there are no parentheses,
but Statements must parse for parentheses (to know if they are finished),
so we break a Statement down into a sequence of parentheticals.
'''
class ParentheticalBuilder(object):
    def __init__(self, indent, openParen = None):
        self.indent = indent
        self.openParen = openParen

        # internalParts is a list of strings and/or sub-parentheticals.
        self.internalParts = []
        self.nextParenBuilder = None
        # if there is no openParen, then the Parenthetical can be considered complete at any time.
        self.completed = True if openParen is None else False

    '''
    eats part of the line to add to the parenthetical.  returns -1 if not yet finished. 
    otherwise returns the index after the close parenthesis that completes this parenthetical.
    TODO: should make line+indent its own class.
    '''
    def consume(self, line, indent, start):
        if self.nextParenBuilder:
            start = self.nextParenBuilder.consume(line, indent, start)
            if start < 0:
                return -1
            # we finished a pair of internal parens:
            self.internalParts.append(self.nextParenBuilder)
            self.nextParenBuilder = None
            # make sure to continue parsing on from here.

        parenIndex = getNextParenIndex(line, start)
        if parenIndex < 0:
            self.internalParts.append(line[start:])
            return -1

        paren = line[parenIndex]
        parenType = allParens.index(paren)

        if parenType < 3: # open paren:
            self.internalParts.append(line[start:parenIndex])
            self.nextParenBuilder = ParentheticalBuilder(indent, paren)
            return self.consume(line, indent, parenIndex+1)

        # close paren
        if self.openParen is None:
            raise ParentheticalError('cannot close parentheses with %s, no open parenthesis.'%paren)
        if allParens.index(self.openParen) != parenType - 3:
            raise ParentheticalError('mismatched parentheses: %s -> %s'%(self.openParen, paren))
        if indent != self.indent:
            raise ParentheticalError('indent of opening %s must match indent of closing %s'%(
                    self.openParen, paren))

        self.internalParts.append(line[start:parenIndex])
        self.completed = True
        return parenIndex + 1

    def isComplete(self):
        return self.nextParenBuilder is None and self.completed == True

import re
allParens = ['(', '[', '{', ')', ']', '}']
parenRe = re.compile('[\\(\\)\\[\\]{}]')
def getNextParenIndex(line, start):
    match = parenRe.search(line, start)
    if match is None:
        return -1
    return match.start()
