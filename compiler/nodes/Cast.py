from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode
from nodes.Type import Type

'''<cast> ::= "(" <type> ")" <identifier>'''

class Cast(ASTNode):
    def __init__(self):
        self.identifier = ""
        self.type = None

    def __str__(self):
        s = f"({self.type}) {self.identifier}"
        return s

    def parse(self, compiler : Compiler):
        compiler.expect(TokenType.LPAREN)
        self.type = Type().parse(compiler)
        compiler.expect(TokenType.RPAREN)
        self.identifier = compiler.expect(TokenType.IDENTIFIER).value
        return self

    def compile(self, compiler: Compiler, file, withComments):
        decl = Compiler.findDeclaration(self.identifier)
        print(Compiler.loadVarCode("rax", decl), file = file)
        oldSize = decl.size
        newSize = self.type.size
        if newSize < oldSize:
            print(f"rax * {(8 - newSize) * 8}", file = file)
            print(f"rax / {(8 - newSize) * 8}", file = file)

    @classmethod
    def createRandom(cls, context):
        return None