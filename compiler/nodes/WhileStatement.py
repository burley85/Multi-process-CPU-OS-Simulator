from Compiler import Compiler, TokenType
from nodes.Expression import Expression
from nodes.Statement import Statement
import Randomizer

'''<while_statement> ::= "while" "(" <expression> ")" <statement>'''
class WhileStatement(Statement):
    @property
    def stackSize(self):
        return self.statement.stackSize
    
    def __init__(self):
        self.expression = None
        self.statement = None

    def __str__(self):
        return f"while({str(self.expression)}){str(self.statement)}"
    
    def parse(self, compiler : Compiler, _):
        compiler.expect(TokenType.WHILE)
        compiler.expect(TokenType.LPAREN)
        self.expression = Expression().parse(compiler)
        compiler.expect(TokenType.RPAREN)
        self.statement = Statement.parse(compiler, 0)
        return self

    def compile(self, compiler : Compiler, file, withComments = False):
        if withComments: print(f";while({self.expression}){'{'}", file = file)
        conditionStartLabel = compiler.newLabel()
        statementStartLabel = compiler.newLabel()
        print(f"jmp {conditionStartLabel}", file = file)
        print(f"{statementStartLabel}:", file = file)
        self.statement.compile(compiler, file, withComments)
        print(f"{conditionStartLabel}:", file = file)
        self.expression.compile(compiler, file, False)
        print("rax = rax", file = file)
        print(f"jnz {statementStartLabel}", file = file)
        if withComments: print("}", file = file)

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        obj.expression = Expression.createRandom(context)
        if obj.expression is None: return None
        obj.statement = Statement.createRandom(context)
        return obj