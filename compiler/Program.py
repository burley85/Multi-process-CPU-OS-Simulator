from Compiler import Compiler, TokenType
from nodes.Method import Method
from sys import *

'''<program> ::= <main_method> | <method> <program>'''
class Program:
    def __init__(self, source):
        self.methods = []
        self.compiler = Compiler(source)

    def parse(self):
        while self.compiler.currentToken().type != TokenType.EOF:
            self.methods.append(Method().parse(self.compiler))

        #TODO: Check if there is a main method
        return self
    
    def print(self, file = stdout, indent = ""):
        for method in self.methods:
            method.print(file, indent)

    def compile(self, file = stdout):
        for method in self.methods:
            self.compiler.currentMethodIdentifier = method.identifier
            method.compile(self.compiler, file)