from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode

'''<call> ::= <identifier> "(" <argument_list> ")" | <identifier> "(" ")"
   <argument_list> ::= <id_or_literal> | <id_or_literal> "," <argument_list>'''
class Call(ASTNode):
    def __init__(self):
        self.identifier = ""
        self.argument_list = []

    def parse(self, compiler : Compiler):
        idOrLiteral = [TokenType.IDENTIFIER, TokenType.NUMBER, TokenType.CHAR, TokenType.STRING]
        self.identifier = compiler.expect(TokenType.IDENTIFIER).value
        compiler.expect(TokenType.LPAREN)
        while compiler.currentToken().type != TokenType.RPAREN:
            self.argument_list.append(compiler.expect(idOrLiteral))
            if compiler.currentToken().type != TokenType.RPAREN: compiler.expect(TokenType.COMMA)
        return self
    
    def print(self, file, indent = ""): pass
