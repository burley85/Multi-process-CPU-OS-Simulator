from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode

'''<comparison> ::= <id_or_literal> <comparison_op> <id_or_literal>'''
class Comparison(ASTNode):
    def __init__(self):
        self.op1 = None
        self.operator = None
        self.op2 = None

    def parse(self, compiler : Compiler):
        idOrLiteral = [TokenType.IDENTIFIER, TokenType.NUMBER, TokenType.CHAR, TokenType.STRING]
        comparisonOperator = [TokenType.EQUALS, TokenType.NOT_EQUALS, TokenType.LESS,
                              TokenType.GREATER, TokenType.LESS_EQUALS, TokenType.GREATER_EQUALS,
                              TokenType.AND, TokenType.OR]
        self.op1 = compiler.expect(idOrLiteral)
        self.operator = compiler.expect(comparisonOperator).type
        self.op2 = compiler.expect(idOrLiteral)
        return self
    
    def print(self, file, indent = ""):
        print(f"{self.op1} {self.operator} {self.op2}", end = "")