from Compiler import Compiler, TokenType
from nodes.Declaration import Declaration
from nodes.Statement import Statement

'''<block> ::= "{" <declaration_list> <statement_list> "}"
   <declaration_list> ::= "" | <declaration> ";" <declaration_list>
   <statement_list> ::= "" | <statement> <statement_list>'''
class Block(Statement):
    @property
    def stackSize(self):
        return self._stackSize

    def __init__(self):
        self.declarations = []
        self.statements = []
        self.stackOffset = 0
        self._stackSize = 0

    def parse(self, compiler : Compiler, stackOffset):
        self.stackOffset = stackOffset
        compiler.expect(TokenType.LBRACE)
        while compiler.currentToken().type in [TokenType.CHAR, TokenType.SHORT, TokenType.INT,
                                               TokenType.LONG, TokenType.UNSIGNED, TokenType.VOID]:
            declaration = Declaration().parse(compiler, self.stackOffset + self._stackSize)
            self.declarations.append(declaration)
            self._stackSize += declaration.size
            compiler.expect(TokenType.SEMICOLON)
        
        maxChildBlockSize = 0
        while compiler.currentToken().type != TokenType.RBRACE:
            statement = Statement.parse(compiler, self._stackSize + stackOffset)
            self.statements.append(statement)
            maxChildBlockSize = max(statement.stackSize, maxChildBlockSize)
        self._stackSize += maxChildBlockSize
        
        compiler.expect(TokenType.RBRACE)
        return self
    
    def print(self, file, indent = ""):
        print(indent + "{\n", file = file, end = "")
        for declaration in self.declarations:
            declaration.print(file, indent + "  ")
            print(";\n", end = "")
        for statement in self.statements:
            statement.print(file, indent + "  ")
            print("\n", end = "")
        print(indent + "}\n", file = file, end = "")

    def compile(self, compiler : Compiler, file):
        compiler.enterBlock()
        for declaration in self.declarations:
            declaration.compile(compiler, file)
        for statement in self.statements:
            statement.compile(compiler, file)
        compiler.leaveBlock()