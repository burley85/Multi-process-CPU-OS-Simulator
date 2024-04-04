from Compiler import Compiler, TokenType
from nodes.Type import Type
from nodes.ASTNode import ASTNode
import Randomizer

'''<return_type> ::= "void" | <type>'''
class ReturnType(ASTNode):
    def __init__(self):
        self.type = None
    
    def __str__(self):
        if self.type is None: return "void"
        else: return str(self.type)

    def parse(self, compiler : Compiler):
        current = compiler.currentToken()
        peek = compiler.peekToken()
        if current.type == TokenType.VOID and peek.type != TokenType.DEREF:
            compiler.nextToken() #Keep self.type as None
        else:
            self.type = Type().parse(compiler)
        return self

    def compile(self, compiler: Compiler, file):
        pass

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        weights = {
            Type.createRandom(context) : 1,
            None : 1
        }
        obj.type = Randomizer.weightedChoice(weights)
        return obj
        