from Compiler import Compiler, TokenType
from nodes.Type import Type
from nodes.ASTNode import ASTNode
import Randomizer

'''<declaration> ::= <type> <identifier> | <type> <identifier> "[" <number> "]"'''
class Declaration(ASTNode):
    def __init__(self):
        self.type = None
        self.identifier = ""
        self.arraySize = 0
        self.size = 0
        self.stackOffset = 0

    def __str__(self):
        if self.arraySize == 0:
            return f"{self.type} {self.identifier}"
        else:
            return f"{self.type} {self.identifier}[{self.arraySize}]"

    def parse(self, compiler : Compiler, stackOffset):
        self.type = Type().parse(compiler)
        self.stackOffset = stackOffset
        self.identifier = compiler.expect(TokenType.IDENTIFIER).value
        if compiler.currentToken().type == TokenType.LBRACKET:
            compiler.nextToken()
            self.arraySize = compiler.expect(TokenType.NUMBER).value
            if self.arraySize <= 0: compiler.genericError("Array size must be greater than 0")
            compiler.expect(TokenType.RBRACKET)
        self.size = self.type.size if self.arraySize == 0 else self.type.size * self.arraySize
        return self

    def compile(self, compiler : Compiler, file, withComments = False):
        if withComments: print(f";{self}", file = file)
        compiler.addDeclaration(self)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        obj.type = Type.createRandom(context)
        obj.identifier = context.newDecl()
        
        #Decide if declaration is an array
        probabilityOfArray = 0 #NOT YET IMPLEMENTED
        if Randomizer.bernoulliDistribution(probabilityOfArray):
            arraySizePValue = .1
            obj.arraySize = Randomizer.geometricDistribution(arraySizePValue) + 1

        return obj