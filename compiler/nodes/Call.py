from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode
from nodes.MethodHeader import MethodHeader

'''<call> ::= <identifier> "(" <argument_list> ")" | <identifier> "(" ")"
   <argument_list> ::= <id_or_literal> | <id_or_literal> "," <argument_list>'''
class Call(ASTNode):
    def __init__(self):
        self.identifier = ""
        self.argument_list = []

    def parse(self, compiler : Compiler):
        idOrLiteral = [TokenType.IDENTIFIER, TokenType.NUMBER, TokenType.CHAR, TokenType.STRING]
        self.identifier = compiler.expect(TokenType.IDENTIFIER).value
        compiler.expect(TokenType.LPAREN)
        while compiler.currentToken().type != TokenType.RPAREN:
            self.argument_list.append(compiler.expect(idOrLiteral))
            if compiler.currentToken().type != TokenType.RPAREN: compiler.expect(TokenType.COMMA)
        compiler.expect(TokenType.RPAREN)
        return self
    
    def print(self, file, indent = ""): 
        print(f'{indent}{self.identifier}(', file = file, end = "")
        for argument in self.argument_list:
            print(argument.value, file = file, end = "")
            if argument != self.argument_list[-1]:
                print(", ", file = file, end = "")
        print(")", file = file, end = "")

    def compile(self, compiler: Compiler, file):
        #Check number of args
        methodHeader = compiler.findDeclaration(self.identifier)
        if not hasattr(methodHeader, "parameterList"):
            compiler.genericError(f"'{self.identifier}' is not a method")
        if len(methodHeader.parameterList) != len(self.argument_list):
            compiler.genericError(f"Incorrect number of arguments for '{self.identifier}'")
        #Push args onto stack
        for (parameter, argument) in zip(methodHeader.parameterList, self.argument_list):
            #Load arg
            argDecl = compiler.findDeclaration(argument.value)
            print(f"rbp {'-' if argDecl.stackOffset > 0 else '+'} {abs(argDecl.stackOffset)}", file = file)
            print("rax = (rbp)", file = file)
            print(f"rbp {'+' if argDecl.stackOffset > 0 else '-'} {abs(argDecl.stackOffset)}", file = file)
            #Push arg onto stack
            print("push rax", file = file)
        #jump to method
        print(f"call _{self.identifier}", file = file)