import sys
from Compiler import Compiler, Token, TokenType
from nodes.Call import Call
from nodes.Comparison import Comparison
from nodes.Cast import Cast
from nodes.ASTNode import ASTNode
import Randomizer

'''<expression> ::= <id_or_literal> | "*" <identifier> | "&" <identifier> | "sizeof" <identifier> |
                    "!" <id_or_literal> | <cast> | <comparison> | <call>'''
class Expression(ASTNode):
    def __init__(self):
        self.child : Token | Comparison | Call | Cast = None
        self.unaryOperator : TokenType = None

    def __str__(self):
        if isinstance(self.child, Call) or isinstance(self.child, Comparison):
            return str(self.child)
        else:
            opMap = {TokenType.DEREF: "*", TokenType.ADDR: "&", TokenType.NOT: "!"}
            return opMap.get(self.unaryOperator, "") + str(self.child)

    def parse(self, compiler : Compiler):
        idOrLiteral = [TokenType.IDENTIFIER, TokenType.NUMBER, TokenType.CHAR, TokenType.STRING]
        comparisonOperator = [TokenType.EQUALS, TokenType.NOT_EQUALS, TokenType.LESS,
                              TokenType.GREATER, TokenType.LESS_EQUALS, TokenType.GREATER_EQUALS,
                              TokenType.AND, TokenType.OR]
        current = compiler.currentToken()
        peek = compiler.peekToken()

        if current.type in [TokenType.DEREF, TokenType.ADDR, TokenType.NOT, TokenType.SIZEOF]:
            self.unaryOperator = compiler.expect([TokenType.DEREF, TokenType.ADDR,
                                                  TokenType.NOT, TokenType.SIZEOF]).type
            if current.type == TokenType.DEREF:
                self.child = compiler.expect(TokenType.IDENTIFIER,
                                             "operand of '*' must be an identifier")
            elif current.type == TokenType.ADDR:
                self.child = compiler.expect(TokenType.IDENTIFIER,
                                             "operand of '&' must be an identifier")
            elif current.type == TokenType.SIZEOF:
                self.child = compiler.expect(TokenType.IDENTIFIER,
                                             "operand of 'sizeof' must be an identifier")
            elif current.type == TokenType.NOT:
                self.child = compiler.expect(idOrLiteral)

        elif(current.type == TokenType.LPAREN):
            self.child = Cast().parse(compiler)
        elif(current.type == TokenType.IDENTIFIER and peek.type == TokenType.LPAREN):
            self.child = Call().parse(compiler)
        elif(current.type in idOrLiteral and peek.type in comparisonOperator):
            self.child = Comparison().parse(compiler)
        else: self.child = compiler.expect(idOrLiteral)
        return self
            
    def compile(self, compiler : Compiler, file, withComments = False):
        if isinstance(self.child, Call) or isinstance(self.child, Comparison):
           self.child.compile(compiler, file, withComments)
        elif self.unaryOperator == TokenType.ADDR:
            decl = compiler.findDeclaration(self.child.value)
            print(Compiler.addrOfVarCode("rax", decl), file = file)
        elif self.unaryOperator == TokenType.DEREF:
            decl = compiler.findDeclaration(self.child.value)
            print(Compiler.loadDerefVarCode("rax", decl), file = file)
        elif self.unaryOperator == TokenType.SIZEOF:
            decl = compiler.findDeclaration(self.child.value)
            print(f"rax = {decl.size}", file = file)
        elif self.unaryOperator != None:
            compiler.genericError(f'Expression type not yet implemented: "{self.unaryOperator}" <{self.child.type}>')
        elif self.child.type == TokenType.NUMBER: 
            print(f"rax = {self.child.value}", file = file)
        elif self.child.type in [TokenType.CHAR, TokenType.STRING]:
            compiler.genericError(f"Expression type not yet implemented: {self.child.type})")
        elif self.child.type == TokenType.IDENTIFIER:
            #Load value into rax
            decl = compiler.findDeclaration(self.child.value)
            print(Compiler.loadVarCode("rax", decl), file = file)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        #Pick call, comparison, *, &, !, or id/literal
        weights = {
            Call: 0,            #NOT YET IMPLEMENTED
            Comparison: 0,      #NOT YET IMPLEMENTED
            TokenType.DEREF: 0, #NOT YET IMPLEMENTED
            TokenType.ADDR: 0,  #NOT YET IMPLEMENTED
            TokenType.NOT: 0,   #NOT YET IMPLEMENTED
            TokenType.IDENTIFIER: 1,
            TokenType.NUMBER: 1,
            TokenType.CHAR: 0,  #NOT YET IMPLEMENTED
            TokenType.STRING: 0 #NOT YET IMPLEMENTED
        }
        result = Randomizer.weightedChoice(weights)
        #Set unaryOperator
        if result in {TokenType.DEREF, TokenType.ADDR, TokenType.NOT}:
            obj.unaryOperator = result

        #Set child
        if result in {Call, Comparison}:
            obj.child = result().createRandom(context)

        if result == TokenType.NOT:
            weights.update({
                Call : 0,
                Comparison : 0,
                TokenType.DEREF : 0,
                TokenType.ADDR : 0,
                TokenType.NOT : 0
            })
            result = Randomizer.weightedChoice(weights)

        if result in {TokenType.IDENTIFIER, TokenType.ADDR, TokenType.DEREF}:
            var = context.randomVar()
            if var is None: return None
            obj.child = Token(TokenType.IDENTIFIER, var)
        elif result == TokenType.NUMBER:
            obj.child = Token(TokenType.NUMBER, Randomizer.randomInteger())
        elif result == TokenType.CHAR:
            obj.child = Token(TokenType.CHAR, Randomizer.randomChar())
        elif result == TokenType.STRING:
            obj.child = Token(TokenType.STRING, Randomizer.randomString())

        return obj