from enum import Enum
from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode
import Randomizer

PrimitiveType = Enum("PrimitiveType", "CHAR, SHORT, INT, LONG, LONG_LONG")

'''<type> ::= "unsigned" <primitive_type> | <primitive_type> |
              "unsigned" <primitive_type> <pointer> | <primitive_type> <pointer> | "void" <pointer>
   <pointer> ::= "*" | "*" <pointer>'''
class Type(ASTNode):
    def __init__(self):
        self.unsigned = False
        self.type = None
        self.pointerDepth = 0
        self.size = 0
        
    def __str__(self):
        s = ""
        if self.type is None: s += "void"
        else:
            if self.unsigned: s += "unsigned "
            if self.type == PrimitiveType.LONG_LONG: s += "long long"
            else: s += self.type.name.lower()
        s += "*" * self.pointerDepth
        return s

    def parse(self, compiler : Compiler):
        if(compiler.currentToken().type == TokenType.UNSIGNED):
            compiler.nextToken()
            self.unsigned = True
        token = compiler.expect([TokenType.CHAR, TokenType.SHORT, TokenType.INT, TokenType.LONG,
                                 TokenType.VOID])
        if(token.type == TokenType.LONG and compiler.currentToken().type == TokenType.LONG):
            self.type = PrimitiveType.LONG_LONG
            compiler.nextToken()
        else: self.type = {
            TokenType.CHAR : PrimitiveType.CHAR,
            TokenType.SHORT : PrimitiveType.SHORT,
            TokenType.INT : PrimitiveType.INT,
            TokenType.LONG : PrimitiveType.LONG,
            TokenType.VOID : None
        }.get(token.type)

        while(compiler.currentToken().type == TokenType.DEREF):
            compiler.nextToken()
            self.pointerDepth += 1
        if(self.pointerDepth == 0 and self.type == None):
            compiler.genericError("Invalid type")

        if(self.pointerDepth > 0): self.size = 8
        else: self.size = {
            PrimitiveType.CHAR : 1,
            PrimitiveType.SHORT : 2,
            PrimitiveType.INT : 4,
            PrimitiveType.LONG : 4,
            PrimitiveType.LONG_LONG : 8,
        }.get(self.type)

        return self

    def compile(self, compiler: Compiler, file, withComments):
        pass

    @classmethod
    def createRandom(cls, context):
        obj  = cls()
        #Chose unsigned
        chanceOfUnsigned = .1
        obj.unsigned = Randomizer.bernoulliDistribution(chanceOfUnsigned)
        #Choose type
        weights = {
            PrimitiveType.CHAR: 0,  #NOT YET IMPLEMENTED
            PrimitiveType.SHORT: 0, #NOT YET IMPLEMENTED
            PrimitiveType.INT: 0,   #NOT YET IMPLEMENTED
            PrimitiveType.LONG: 0,  #NOT YET IMPLEMENTED
            PrimitiveType.LONG_LONG: 1,
            None: 0                 #NOT YET IMPLEMENTED
        }
        obj.type = Randomizer.weightedChoice(weights)
        
        #Choose pointerDepth
        chanceOfPointer = 0 #NOT YET IMPLEMENTED
        if(obj.type is None): obj.pointerDepth = 1
        if Randomizer.bernoulliDistribution(chanceOfPointer):
            obj.pointerDepth += 1
            obj.pointerDepth += Randomizer.geometricDistribution(.75)
        return obj