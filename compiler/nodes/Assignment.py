from Compiler import Compiler, TokenType
from nodes.Expression import Expression
from nodes.ASTNode import ASTNode
import Randomizer
from nodes.Declaration import Declaration
from nodes.Type import *

'''<assignment> ::= <l_value> <assign_op> <expression> | <l_value> "++" | <l_value> "--"
   <l_value> ::= <identifier> | "*" <identifier>'''
class Assignment(ASTNode):
    def __init__(self):
        self.lValueID = None
        self.derefLValue = False
        self.operator = None
        self.expression = None
        
    def __str__(self):
        opMap = {
            TokenType.ASSIGN: "=",
            TokenType.PLUS_ASSIGN: "+=",
            TokenType.MINUS_ASSIGN: "-=",
            TokenType.MULTIPLY_ASSIGN: "*=",
            TokenType.DIVIDE_ASSIGN: "/=",
            TokenType.MODULO_ASSIGN: "%=",
            TokenType.INCREMENT: "++",
            TokenType.DECREMENT: "--"
        }
        if self.expression != None:
            return f'{"*" if self.derefLValue else ""}{self.lValueID} {opMap[self.operator]} {self.expression}'
        else:
            return f'{"*" if self.derefLValue else ""}{self.lValueID}{opMap[self.operator]}'
        
    def parse(self, compiler : Compiler):
        assignOp = [TokenType.ASSIGN, TokenType.PLUS_ASSIGN, TokenType.MINUS_ASSIGN,
            TokenType.MULTIPLY_ASSIGN, TokenType.DIVIDE_ASSIGN, TokenType.MODULO_ASSIGN]
        if compiler.currentToken().type == TokenType.DEREF:
            compiler.nextToken()
            self.derefLValue = True
        self.lValueID = compiler.expect(TokenType.IDENTIFIER).value
        self.operator = compiler.expect(assignOp + [TokenType.INCREMENT, TokenType.DECREMENT]).type
        if self.operator in assignOp:
            self.expression = Expression().parse(compiler)
        return self

    def compile(self, compiler : Compiler, file, withComments = False):
        opMap = {
            TokenType.ASSIGN: "=",
            TokenType.PLUS_ASSIGN: "+",
            TokenType.MINUS_ASSIGN: "-",
            TokenType.MULTIPLY_ASSIGN: "*",
            TokenType.DIVIDE_ASSIGN: "/",
            TokenType.INCREMENT: "+ 1",
            TokenType.DECREMENT: "- 1"
        }

        if(self.operator == TokenType.MODULO_ASSIGN): compiler.genericError(f"Modulo not yet implemented")

        if(self.expression != None):
            self.expression.compile(compiler, file, withComments) #Stores result of expression in rax
        

        decl = compiler.findDeclaration(self.lValueID)

        #Load lvalue
        print(Compiler.addrOfVarCode("rcx", decl), file = file)
        if self.derefLValue: print("rcx = (rcx)", file = file)
        print("rbx = (rcx)", file = file)
        if decl.size < 8: print(f"rbx / {2**((8 - decl.size) * 8)}", file = file)
        
        #Calculate
        if(self.expression != None): print(f"rbx {opMap.get(self.operator)} rax", file = file)
        else: print(f"rbx{opMap.get(self.operator)}", file = file)

        #Write to lvalue
        print(Compiler.storeVarCode("rbx", "rcx", "rdx", decl), file = file)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        #Pick number of derefs
        #while(random.randint(0, 1) == 1): obj.lValueRefDepth += 1 -- NOT YET IMPLEMENTED
        obj.lValueRefDepth = 0
        #Pick random l-value
        obj.lValueID = context.randomVar()
        if obj.lValueID == None: return None
        #Pick random operator
        obj.operator = Randomizer.weightedChoice({
            TokenType.ASSIGN : 5,
            TokenType.PLUS_ASSIGN : 1,
            TokenType.MINUS_ASSIGN : 1,
            TokenType.MULTIPLY_ASSIGN : 1,
            TokenType.DIVIDE_ASSIGN : 1,
            TokenType.MODULO_ASSIGN : 0, #NOT YET IMPLEMENTED
            TokenType.INCREMENT : 1,
            TokenType.DECREMENT : 1
        })

        #If operator is assignment, pick random expression
        if obj.operator not in {TokenType.INCREMENT, TokenType.DECREMENT}:
            obj.expression = Expression.createRandom(context)
        return obj