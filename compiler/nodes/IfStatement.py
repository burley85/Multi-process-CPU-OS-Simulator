from Compiler import Compiler, TokenType
from nodes.Expression import Expression
from nodes.Statement import Statement
import Randomizer

'''<if_statement> ::= "if" "(" <expression> ")" <statement> <else_statement>
   <else_statement> ::= "" | "else" <statement>'''
class IfStatement(Statement):
    @property
    def stackSize(self):
        if self.elseStatement is None: return self.statement.stackSize
        else: return max(self.elseStatement.stackSize, self.statement.stackSize)
    
    def __str__(self):
        if self.elseStatement is None: return f"if({str(self.expression)}){self.statement}"
        else: return f"if({str(self.expression)}){self.statement}\nelse {self.elseStatement}"
        
    def __init__(self):
        self.expression = None
        self.statement = None
        self.elseStatement = None

    def parse(self, compiler : Compiler, _):
        compiler.expect(TokenType.IF)
        compiler.expect(TokenType.LPAREN)
        self.expression = Expression().parse(compiler)
        compiler.expect(TokenType.RPAREN)
        self.statement = Statement.parse(compiler, 0)
        if compiler.currentToken().type == TokenType.ELSE:
            compiler.expect(TokenType.ELSE)
            self.elseStatement = Statement.parse(compiler, 0)
        return self

    def compile(self, compiler : Compiler, file, withComments = False):
        if withComments: print(f";if({self.expression}){'{'}", file = file)
        self.expression.compile(compiler, file, withComments)
        statementEndLabel = compiler.newLabel()
        print("rax = rax", file = file)
        print(f"jz {statementEndLabel}", file = file)
        self.statement.compile(compiler, file, withComments)
        if self.elseStatement is not None:
            if withComments: print(";} else {", file = file)
            elseEndLabel = compiler.newLabel()
            print(f"jmp {elseEndLabel}", file = file)
            print(f"{statementEndLabel}:", file = file)
            self.elseStatement.compile(compiler, file, withComments)
            print(f"{elseEndLabel}:", file = file)
        else:
                print(f"{statementEndLabel}:", file = file)
        if withComments: print(";}", file = file)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        probabilityOfElseStatement = 0.3
        obj.expression = Expression.createRandom(context)
        if obj.expression is None: return None
        obj.statement = Statement.createRandom(context)
        if Randomizer.bernoulliDistribution(probabilityOfElseStatement):
            obj.elseStatement = Statement.createRandom(context)
        return obj