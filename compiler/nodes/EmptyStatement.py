from Compiler import Compiler, TokenType
from nodes.Statement import Statement

class EmptyStatement(Statement):
    @property
    def stackSize(self):
        return 0
    
    def __init__(self):
        pass
    def parse(self, compiler : Compiler, _):
        pass