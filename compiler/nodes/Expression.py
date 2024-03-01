import sys
from Compiler import Compiler, Token, TokenType
from nodes.Call import Call
from nodes.Comparison import Comparison

'''<expression> ::= <id_or_literal> | "*" <identifier> | "&" <identifier> | "!" <id_or_literal> |
                    <comparison> | <call>'''
class Expression:
    def __init__(self):
        self.child : Token | Comparison | Call = None
        self.unaryOperator : TokenType = None

    def parse(self, compiler : Compiler):
        idOrLiteral = [TokenType.IDENTIFIER, TokenType.NUMBER, TokenType.CHAR, TokenType.STRING]
        comparisonOperator = [TokenType.EQUALS, TokenType.NOT_EQUALS, TokenType.LESS,
                              TokenType.GREATER, TokenType.LESS_EQUALS, TokenType.GREATER_EQUALS,
                              TokenType.AND, TokenType.OR]
        current = compiler.currentToken()
        peek = compiler.peekToken()

        if current.type in [TokenType.DEREF, TokenType.ADDR, TokenType.NOT]:
            self.unaryOperator = compiler.expect([TokenType.DEREF, TokenType.ADDR,
                                                  TokenType.NOT]).type
            if current.type == TokenType.DEREF:
                self.child = compiler.expect(TokenType.IDENTIFIER,
                                             "operand of '*' must be a pointer")
            elif current.type == TokenType.ADDR:
                self.child = compiler.expect(TokenType.IDENTIFIER,
                                             "operand of '&' must be an lvalue")
            elif current.type == TokenType.NOT:
                self.child = compiler.expect(idOrLiteral)

        elif(current.type == TokenType.IDENTIFIER and peek.type == TokenType.LPAREN):
            self.child = Call().parse(compiler)
        elif(current.type in idOrLiteral and peek.type in comparisonOperator):
            self.child = Comparison().parse(compiler)
        else: self.child = compiler.expect(idOrLiteral)
        return self
    
    def print(self, file, indent = ""):
        if isinstance(self.child, Call) or isinstance(self.child, Comparison):
            self.child.print(file, "")
        else:
            print({TokenType.DEREF: "*", TokenType.ADDR: "&", TokenType.NOT: "!"}
                  .get(self.unaryOperator, "") + str(self.child), file = file, end = "")
            
    def compile(self, compiler : Compiler, file):
        if isinstance(self.child, Call) or isinstance(self.child, Comparison):
           self.child.compile(compiler, file)
        elif self.unaryOperator != None:
            compiler.genericError(f'Expression type not yet implemented: "{self.unaryOperator}" <{self.child.type}>')
        elif self.child.type == TokenType.NUMBER: 
            print(f"rax = {self.child.value}", file = file)
        elif self.child.type in [TokenType.CHAR, TokenType.STRING]:
            compiler.genericError(f"Expression type not yet implemented: {self.child.type})")
        elif self.child.type == TokenType.IDENTIFIER:
            #Load value into rax
            decl = compiler.findDeclaration(self.child.value)
            print(f"rbp - {decl.stackOffset}", file = file)
            print("rax = (rbp)", file = file)
            print(f"rbp + {decl.stackOffset}", file = file)

