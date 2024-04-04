from Compiler import Compiler, TokenType
from nodes.Expression import Expression
from nodes.Statement import Statement

'''<while_statement> ::= "while" "(" <expression> ")" <statement>'''
class WhileStatement(Statement):
    @property
    def stackSize(self):
        return 0
    
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

    def compile(self, compiler : Compiler, file):
        conditionStartLabel = compiler.newLabel()
        statementStartLabel = compiler.newLabel()
        print(f"jmp {conditionStartLabel}", file = file)
        print(f"{statementStartLabel}:", file = file)
        self.statement.compile(compiler, file)
        print(f"{conditionStartLabel}:", file = file)
        self.expression.compile(compiler, file)
        print("rax = rax", file = file)
        print(f"jnz {statementStartLabel}", file = file)

        