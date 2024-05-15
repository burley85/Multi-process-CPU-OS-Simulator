from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode
from nodes.MethodHeader import MethodHeader

'''<call> ::= <identifier> "(" <argument_list> ")" | <identifier> "(" ")"
   <argument_list> ::= <id_or_literal> | <id_or_literal> "," <argument_list>'''
class Call(ASTNode):
    def __init__(self):
        self.identifier = ""
        self.argument_list = []

    def __str__(self):
        s = f"{self.identifier}("
        for argument in self.argument_list:
            s += str(argument)
            if argument != self.argument_list[-1]:
                s += ", "
        s += ")"
        return s

    def parse(self, compiler : Compiler):
        from nodes.Expression import Expression
        idOrLiteral = [TokenType.IDENTIFIER, TokenType.NUMBER, TokenType.CHAR, TokenType.STRING]
        self.identifier = compiler.expect(TokenType.IDENTIFIER).value
        compiler.expect(TokenType.LPAREN)
        while compiler.currentToken().type != TokenType.RPAREN:
            self.argument_list.append(Expression().parse(compiler))
            if compiler.currentToken().type != TokenType.RPAREN: compiler.expect(TokenType.COMMA)
        compiler.expect(TokenType.RPAREN)
        return self

    def compile(self, compiler: Compiler, file, withComments):
        from nodes.Expression import Expression
        #Check number of args
        methodHeader = compiler.findDeclaration(self.identifier)
        if not hasattr(methodHeader, "parameterList"):
            compiler.genericError(f"'{self.identifier}' is not a method")
        if len(methodHeader.parameterList) != len(self.argument_list):
            compiler.genericError(f"Incorrect number of arguments for '{self.identifier}'")
        #Push args onto stack
        for (parameter, argument) in zip(methodHeader.parameterList, self.argument_list):
            #Load arg
            argument.compile(compiler, file, withComments)
            #Push arg onto stack
            if parameter.size < 8:
                print(f"rax * {2**((8 - parameter.size) * 8)}", file = file)
                print("push rax", file = file)
                print(f"rsp + {8 - parameter.size}", file = file)
            else: print("push rax", file = file)
        #jump to method
        print(f"call _{self.identifier}", file = file)
        #Clean up stack
        print(f"rsp + {methodHeader.parameterSize}", file = file)

    @classmethod
    def createRandom(cls, context):
        return None