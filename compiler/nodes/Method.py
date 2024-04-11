from Compiler import Compiler, TokenType
from nodes.MethodHeader import MethodHeader
from nodes.Block import Block
from nodes.ASTNode import ASTNode

'''<method> ::= <method_header> <block>'''
class Method(ASTNode):
    def __init__(self):
        self.method_header = None
        self.block = None
        self.stackSize = 0
        self.identifier = ""
        
    def __str__(self):
        s = str(self.method_header) + str(self.block)
        s = s.replace("\n", "\t\n")
        return s
    
    def parse(self, compiler : Compiler):
        self.method_header = MethodHeader().parse(compiler)
        self.identifier = self.method_header.identifier
        self.block = Block().parse(compiler, 0)
        self.stackSize = self.block.stackSize
        return self
    
    def name(self):
        return self.method_header.identifier

    def compile(self, compiler : Compiler, file, withComments = False):
        compiler.addDeclaration(self.method_header)
        compiler.enterBlock()
        self.method_header.compile(compiler, file, withComments)
        if withComments: print(";{", file = file)
        print(Compiler.stackSetupCode(self.stackSize), file = file)

        self.block.compile(compiler, file, withComments)

        #Cleanup the stack
        print(Compiler.stackTeardownCode(self.stackSize), file = file)

        if withComments: print(";}", file = file)
        compiler.leaveBlock()

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        obj.method_header = MethodHeader.createRandom(context)
        obj.block = Block.createRandom(context)
        return obj
