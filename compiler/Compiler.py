from Tokenizer import Tokenizer, Token, TokenType

class Compiler:
    def __init__(self, source):
        self.tokenizer = Tokenizer(source)
        self.symbolTableList = [] #List of Dictionarys containing id : Declaration pairs
        self.currentMethodIdentifier = ""
        self.labelCount = 0
    
    def nextToken(self):
        return self.tokenizer.nextToken()

    def currentToken(self):
        return self.tokenizer.currentToken()
    
    '''Return the next token without advancing the tokenizer.'''
    def peekToken(self):
        originalPos, originalCurrentToken = self.tokenizer.position, self.tokenizer.currentToken()
        token = self.nextToken()
        self.tokenizer.position, self.tokenizer.token = originalPos, originalCurrentToken
        self.tokenizer.source.seek(originalPos)
        return token

    '''Return a list of the next n tokens without advancing the tokenizer.'''
    def lookAhead(self, n) -> list[Token]:
        originalPos, originalCurrentToken = self.tokenizer.position, self.tokenizer.currentToken()
        tokenList = []
        for i in range(n):
            tokenList.append(self.nextToken())
        self.tokenizer.position, self.tokenizer.currentToken = originalPos, originalCurrentToken
        self.tokenizer.source.seek(originalPos)
        return tokenList
    
    '''Checks if the current token is of the given type. If it is, consume it and return the token. Otherwise, raise an exception.'''
    def expect(self, type : TokenType | list[TokenType], message = None):
        if not isinstance(type, list): type = [type]
        
        token = self.tokenizer.currentToken()
        if token.type in type:
            self.tokenizer.nextToken()
            return token
        else:
            if message is None: message = f"Expected one of {type} but got {token}"
            self.genericError(message)
    
    def genericError(self, message):
        raise Exception(message)
    
    def addDeclaration(self, declaration):
        self.symbolTableList[-1][declaration.identifier] = declaration

    def findDeclaration(self, identifier):
        #Search list in reverse order to find most recent declaration
        for symbolTable in reversed(self.symbolTableList):
            assert isinstance(symbolTable, dict)
            if identifier in symbolTable: return symbolTable[identifier]
        self.genericError(f"Identifier {identifier} is undefined")


    def enterBlock(self):
        self.symbolTableList.append({})

    def leaveBlock(self):
        self.symbolTableList.pop()

    def newLabel(self):
        self.labelCount += 1
        return f"l{self.labelCount}"