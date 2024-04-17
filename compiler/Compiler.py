from Tokenizer import Tokenizer, Token, TokenType

class Compiler:
    def __init__(self, source):
        self.tokenizer = Tokenizer(source)
        import nodes.Program
        self.program = nodes.Program.Program()
        self.symbolTableList = [{}] #List of Dictionarys containing id : Declaration pairs
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
        self.tokenizer.position, self.tokenizer.token = originalPos, originalCurrentToken
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
        if declaration.identifier in self.symbolTableList[-1]:
            self.genericError(f"Redeclaration of '{declaration.identifier}'")
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
    
    def compile(self, file):
        self.program.parse(self)
        return self.program.compile(self, file, True)

    @classmethod
    def addrOfVarCode(cls, register, decl):
        s = f"{register} = rbp\n"
        s += f"{register} {'-' if decl.stackOffset > 0 else '+'} {abs(decl.stackOffset)}\n"
        return s

    @classmethod
    def loadVarCode(cls, register, decl):
        s = f"{register} = rbp\n"
        s += f"{register} {'-' if decl.stackOffset > 0 else '+'} {abs(decl.stackOffset)}\n"
        s += f"{register} = ({register})\n"
        s += f"{register} / {2 ** (64 - (decl.size * 8))}"
        return s
    
    @classmethod
    def derefVarCode(cls, register, decl):
        s = f"{register} = rbp\n"
        s += f"{register} {'-' if decl.stackOffset > 0 else '+'} {abs(decl.stackOffset)}\n"
        s += f"{register} = ({register})\n"
        s += f"{register} = ({register})\n"
        return s
    
    @classmethod
    def storeVarCode(cls, valueRegister, tempRegister1, tempRegister2, decl):
        varBits = decl.size * 8
        ptrRegister = tempRegister1
        maskedValue = tempRegister2
        s = f"{ptrRegister} = rbp\n"
        s += f"{ptrRegister} {'-' if decl.stackOffset > 0 else '+'} {abs(decl.stackOffset)}\n"
        if(decl.size < 8):
            #Isolate last 64 - varBits bits of original value
            s += f"{maskedValue} = ({ptrRegister})\n"
            s += f"{maskedValue} * {2**varBits}\n"
            s += f"{maskedValue} / {2**varBits}\n"
            #Isolate last varBits bits of valueRegister
            s += f"{valueRegister} * {2 ** (64 - varBits)}\n"
            #Combine last 64 - varBits bits of original value with first varBits bits of valueRegister
            s += f"{valueRegister} + {maskedValue}\n"

        s += f"({ptrRegister}) = {valueRegister}\n"
        return s

    @classmethod
    def stackSetupCode(cls, stackSize):
        s = "push rbp\n"
        s += "rbp = rsp\n"
        s += f"rsp - {stackSize}\n"

    @classmethod
    def stackTeardownCode(cls):
        s = "rsp = rbp\n"
        s += "pop rbp\n"

    @classmethod
    def comparisonCode(cls, operator : TokenType):
        return {
            TokenType.EQUALS:
                f"rax - rbx\n\
                jnz l1\n\
                rax = 1\n\
                jmp l2\n\
                l1:\n\
                rax = 0\n\
                l2:\n",
            TokenType.NOT_EQUALS:
                f"rax - rbx\n\
                jz l1\n\
                rax = 1\n\
                l1:\n",
            TokenType.LESS:
                f"rax - rbx\n\
                jns l1\n\
                rax = 1\n\
                jmp l2\n\
                l1:\n\
                rax = 0\n\
                l2:\n",
            TokenType.GREATER:
                f"rax - rbx\n\
                js l1\n\
                jz l1\n\
                rax = 1\n\
                jmp l2\n\
                l1:\n\
                rax = 0\n\
                l2:\n",
            TokenType.LESS_EQUALS:
                f"rax - rbx\n\
                js l1\n\
                jz l1\n\
                rax = 0\n\
                jmp l2\n\
                l1:\n\
                rax = 1\n\
                l2:\n",
            TokenType.GREATER_EQUALS:
                f"rax - rbx\n\
                jns l1\n\
                rax = 0\n\
                jmp l2\n\
                l1:\n\
                rax = 1\n\
                l2:\n",
            TokenType.AND:
                f"rax * rbx\n\
                jz l1\n\
                rax = 1\n\
                l1:\n",
            TokenType.OR:
                f"rax + rbx\n\
                jz l1\n\
                rax = 1\n\
                l1:\n"
        }.get(operator, "")