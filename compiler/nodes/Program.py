from sys import *
from Compiler import Compiler, TokenType
from nodes.Method import Method
from nodes.ASTNode import ASTNode
import Randomizer
import random

'''<program> ::= <main_method> | <method> <program>'''
class Program(ASTNode):
    def __init__(self):
        self.methods = []

    def parse(self, compiler : Compiler):
        while compiler.currentToken().type != TokenType.EOF:
            self.methods.append(Method().parse(compiler))

        for method in self.methods:
            if method.name() == "main": return self
        compiler.genericError('"main" is not defined')
    
    def print(self, file = stdout, indent = ""):
        for method in self.methods:
            method.print(file, indent)

    def compile(self, compiler, file):
        print("jmp _main\n", file=file, end = "")
        for method in self.methods:
            compiler.currentMethodIdentifier = method.identifier
            method.compile(compiler, file)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        probabilityOfMissingMain = .05
        probabilityOfDuplicateMethodName = .05
        probabilityThatMainIsLast = .9
        methodNumberPValue = .1

        methodNumber = Randomizer.geometricDistribution(methodNumberPValue) + 1
        for i in range(methodNumber):
            obj.methods.append(Method.createRandom(context))
        if not Randomizer.bernoulliDistribution(probabilityOfMissingMain):
            #There is a "main" method
            if Randomizer.bernoulliDistribution(probabilityThatMainIsLast):
                #"main" is the last method
                main = obj.methods[-1]
                main.method_header.identifier = "main"
            else:
                #"main" is a random method
                main = random.choice(obj.methods)
                main.method_header.identifier = "main"
        return obj