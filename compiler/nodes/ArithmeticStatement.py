from Compiler import Compiler, TokenType
from nodes.Assignment import Assignment
from nodes.Expression import Expression
from nodes.Statement import Statement

'''<arithmetic_statement> ::= <assignment> ";" | <expression> ";"'''
class ArithmeticStatement(Statement):
    @property
    def stackSize(self):
        return 0
    
    def __init__(self):
        self.child = None

    def parse(self, compiler : Compiler, _):
        assignOp = [TokenType.ASSIGN, TokenType.PLUS_ASSIGN, TokenType.MINUS_ASSIGN,
                    TokenType.MULTIPLY_ASSIGN, TokenType.DIVIDE_ASSIGN, TokenType.MODULO_ASSIGN,
                    TokenType.INCREMENT, TokenType.DECREMENT]
        first = compiler.currentToken()
        if first.type in {TokenType.ADDR, TokenType.NOT, TokenType.NUMBER, TokenType.STRING,
                          TokenType.CHAR}:
            self.child = Expression().parse(compiler) #Starts with a non-lvalue
        elif first.type == TokenType.IDENTIFIER:
            peek = compiler.peekToken()
            if peek.type in assignOp: 
                self.child = Assignment().parse(compiler) #Something like <identifier> "="
            else:
                self.child = Expression().parse(compiler) #Something like <identifier> ";"
        elif first.type == TokenType.DEREF:
            peek = compiler.lookAhead(2)
            if peek[0].type == TokenType.IDENTIFIER and peek[1].type not in assignOp:
                self.child = Expression().parse(compiler) #Something like "*" <identifier> ";"
            else:
                self.child = Assignment().parse(compiler) #Something like "*" <identifier> "=" or "*" "*" <identifier>
        else: compiler.expect({TokenType.ADDR, TokenType.NOT, TokenType.NUMBER, TokenType.STRING,
                          TokenType.CHAR, TokenType.IDENTIFIER, TokenType.DEREF})
        compiler.expect(TokenType.SEMICOLON)
        return self
    
    def print(self, file, indent = ""):
        self.child.print(file, indent)
        print(";", file = file, end = "")

    def compile(self, compiler, file):
        self.child.compile(compiler, file)
