from sys import *
from Compiler import Compiler, TokenType
from nodes.Method import Method
from nodes.ASTNode import ASTNode

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