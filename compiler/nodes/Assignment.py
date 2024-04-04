from Compiler import Compiler, TokenType
from nodes.Expression import Expression
from nodes.ASTNode import ASTNode
import Randomizer

'''<assignment> ::= <l_value> <assign_op> <expression> | <l_value> "++" | <l_value> "--"
   <l_value> ::= <identifier> | "*" <identifier>'''
class Assignment(ASTNode):
    def __init__(self):
        self.lValueID = None
        self.lValueRefDepth = 0
        self.operator = None
        self.expression = None
        
    def parse(self, compiler : Compiler):
        assignOp = [TokenType.ASSIGN, TokenType.PLUS_ASSIGN, TokenType.MINUS_ASSIGN,
            TokenType.MULTIPLY_ASSIGN, TokenType.DIVIDE_ASSIGN, TokenType.MODULO_ASSIGN]
        while(compiler.currentToken().type == TokenType.DEREF):
            compiler.nextToken()
            self.lValueRefDepth += 1
        self.lValueID = compiler.expect(TokenType.IDENTIFIER).value
        self.operator = compiler.expect(assignOp + [TokenType.INCREMENT, TokenType.DECREMENT]).type
        if self.operator in assignOp:
            self.expression = Expression().parse(compiler)
        return self
    
    def print(self, file, indent = ""):
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
        print(f'{indent}{"*" * self.lValueRefDepth}{self.lValueID} {opMap[self.operator]}',
              file = file, end = "")
        if(self.expression != None): 
            print(" ", file = file, end = "")
            self.expression.print(file, "")

    def compile(self, compiler, file):
        opMap = {
            TokenType.ASSIGN: "=",
            TokenType.PLUS_ASSIGN: "+",
            TokenType.MINUS_ASSIGN: "-",
            TokenType.MULTIPLY_ASSIGN: "*",
            TokenType.DIVIDE_ASSIGN: "/",
            TokenType.INCREMENT: "+ 1",
            TokenType.DECREMENT: "- 1"
        }

        if(self.lValueRefDepth > 0): compiler.genericError(f"Dereference of l-values not yet implemented")
        if(self.operator == TokenType.MODULO_ASSIGN): compiler.genericError(f"Modulo not yet implemented")

        if(self.expression != None):
            self.expression.compile(compiler, file) #Stores result of expression in rax
        
        #Load lvalue
        decl = compiler.findDeclaration(self.lValueID)
        print(f"rbp - {decl.stackOffset}", file = file)
        print("rbx = (rbp)", file = file)
        
        #Calculate
        if(self.expression != None): print(f"rbx {opMap.get(self.operator)} rax", file = file)
        else: print(f"rbx{opMap.get(self.operator)}", file = file)

        #Write to lvalue
        print("(rbp) = rbx", file = file)
        print(f"rbp + {decl.stackOffset}", file = file)

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
            TokenType.ASSIGN : 1,
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