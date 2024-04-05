from Compiler import Compiler, TokenType
from nodes.Statement import Statement

class EmptyStatement(Statement):
    @property
    def stackSize(self):
        return 0
    
    def __init__(self):
        pass

    def __str__(self):
        return ";"

    def parse(self, compiler : Compiler, _):
        compiler.expect(TokenType.SEMICOLON)
        return self

    def compile(self, compiler: Compiler, file, withComments):
        if withComments: print(f";{self}", file = file)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        return obj