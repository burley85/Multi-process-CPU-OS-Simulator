from Compiler import Compiler, TokenType
from nodes.ReturnType import ReturnType
from nodes.Declaration import Declaration
from nodes.ASTNode import ASTNode
import Randomizer

'''<method_header> ::= <return_type> <identifier> "(" <parameter_list> ")"
   <parameter_list> ::= "" | <declaration> | <declaration> "," <parameter_list>'''
class MethodHeader(ASTNode):
    def __init__(self):
        self.returnType = None
        self.identifier = ""
        self.parameterList = []

    def __str__(self):
        s = f"{self.returnType} {self.identifier}("
        
        for parameter in self.parameterList:
            s += str(parameter)
            if parameter != self.parameterList[-1]: 
                s += ", "
        s += ")"
        return s

    def parse(self, compiler : Compiler):
        self.returnType = ReturnType().parse(compiler)
        self.identifier = compiler.expect(TokenType.IDENTIFIER).value
        compiler.expect(TokenType.LPAREN)

        argStackPosition = -16
        while compiler.currentToken().type != TokenType.RPAREN:
            self.parameterList.append(Declaration().parse(compiler, argStackPosition))
            argStackPosition -= 8
            if compiler.currentToken().type != TokenType.RPAREN: compiler.expect(TokenType.COMMA)

        compiler.expect(TokenType.RPAREN)
        return self

    def compile(self, compiler : Compiler, file):
        for parameter in self.parameterList:
            compiler.addDeclaration(parameter)
        print(compiler.symbolTableList.__repr__())
        print(f"_{self.identifier}:\n", file=file, end = "")
        
    @classmethod
    def createRandom(cls, context):
        obj = cls()
        obj.returnType = ReturnType.createRandom(context)
        obj.identifier = context.newDecl()
        parameterCount = Randomizer.geometricDistribution(.5)
        for i in range(parameterCount):
            obj.parameterList.append(Declaration.createRandom(context))
        return obj
