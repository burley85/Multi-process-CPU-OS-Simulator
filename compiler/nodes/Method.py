from Compiler import Compiler, TokenType
from nodes.MethodHeader import MethodHeader
from nodes.Block import Block


'''<method> ::= <method_header> <block>'''
class Method:
    def __init__(self):
        self.method_header = None
        self.block = None
        self.stackSize = 0
        self.identifier = ""
        
    def parse(self, compiler : Compiler):
        self.method_header = MethodHeader().parse(compiler)
        self.identifier = self.method_header.identifier
        self.block = Block().parse(compiler, 0)
        self.stackSize = self.block.stackSize
        return self
    
    def name(self):
        return self.method_header.identifier

    def print(self, file, indent = ""):
        self.method_header.print(file, indent)
        self.block.print(file, indent)

    def compile(self, compiler : Compiler, file):
        self.method_header.compile(compiler, file)
        #Setup the stack
        print("push rbp", file = file)
        print("rbp = rsp", file = file)
        print("rsp - " + str(self.stackSize), file = file)

        self.block.compile(compiler, file)
