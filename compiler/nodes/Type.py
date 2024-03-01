from enum import Enum
from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode

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
    
    def print(self, file, indent = ""):
        print(indent, end = "")
        if(self.unsigned): print("unsigned ", file = file, end = "")
        if(self.type == None): print("void ", file = file, end = "")
        elif(self.type == PrimitiveType.LONG_LONG): print("long long ", end = "")
        else: print(self.type.name.lower() + " ", end = "")
        print("*" * self.pointerDepth, end = "")

    def compile(self, compiler: Compiler, file):
        pass