from Compiler import Compiler, TokenType
from nodes.Expression import Expression
from nodes.Statement import Statement

'''<return_statement> ::= "return" <expression> ";" | "return" ";"'''
class ReturnStatement(Statement):
    @property
    def stackSize(self):
        return 0
    
    def __init__(self):
        self.expression = None
        
    def __str__(self):
        if self.expression is None: return "return;"
        else: return f"return {self.expression};"

    def parse(self, compiler : Compiler, _):
        compiler.expect(TokenType.RETURN)
        if compiler.currentToken().type!= TokenType.SEMICOLON:
            self.expression = Expression().parse(compiler)
        compiler.expect(TokenType.SEMICOLON)
        return self #TODO: make sure expression matches method's return type

    def compile(self, compiler : Compiler, file, withComments = False):
        if withComments: print(f";{self}", file = file)
        if self.expression != None: 
            self.expression.compile(compiler, file, withComments)
        
        #Tear down the stack
        print("rsp = rbp", file = file)
        print("pop rbp", file = file)
        
        if compiler.currentMethodIdentifier == "main": print("halt", file = file)
        else: print("ret", file = file)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        obj.expression = Expression.createRandom(context)
        if obj.expression is None: return None
        return obj