from Compiler import Compiler, TokenType
from nodes.Statement import Statement

class EmptyStatement(Statement):
    @property
    def stackSize(self):
        return 0
    
    def __init__(self):
        pass
    
    def parse(self, compiler : Compiler, _):
        compiler.expect(TokenType.SEMICOLON)
        return self

    def print(self, file, indent = ""):
        print(indent + ";\n", file = file, end = "")

    def compile(self, compiler: Compiler, file):
        return

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        return obj