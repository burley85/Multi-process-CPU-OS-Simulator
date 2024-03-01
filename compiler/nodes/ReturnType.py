from Compiler import Compiler, TokenType
from nodes.Type import Type
from nodes.ASTNode import ASTNode

'''<return_type> ::= "void" | <type>'''
class ReturnType(ASTNode):
    def __init__(self):
        self.type = None
    
    def parse(self, compiler : Compiler):
        current = compiler.currentToken()
        peek = compiler.peekToken()
        if current.type == TokenType.VOID and peek.type != TokenType.DEREF:
            compiler.nextToken() #Keep self.type as None
        else:
            self.type = Type().parse(compiler)
        return self

    def print(self, file, indent = ""):
        if(self.type == None): print(indent + "void ", file = file, end = "")
        else: self.type.print(file, indent)

    def compile(self, compiler: Compiler, file):
        pass