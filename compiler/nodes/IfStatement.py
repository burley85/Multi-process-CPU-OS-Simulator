from Compiler import Compiler, TokenType
from nodes.Expression import Expression
from nodes.Statement import Statement

'''<if_statement> ::= "if" "(" <expression> ")" <statement> <else_statement>
   <else_statement> ::= "" | "else" <statement>'''
class IfStatement(Statement):
    @property
    def stackSize(self):
        return 0
    
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
    
    def print(self, file, indent = ""):
        print(indent + "if (", file = file, end = "")
        self.expression.print(file, indent)
        print(") ", file = file, end = "")
        self.statement.print(file, indent)
        if self.elseStatement is not None:
            print("else ", file = file, end = "")
            self.elseStatement.print(file, indent)

    def compile(self, compiler : Compiler, file):
        self.expression.compile(compiler, file)
        statementEndLabel = compiler.newLabel()
        print("rax = rax", file = file)
        print(f"jz {statementEndLabel}", file = file)
        self.statement.compile(compiler, file)
        if self.elseStatement is not None:
            elseEndLabel = compiler.newLabel()
            print(f"jmp {elseEndLabel}", file = file)
            print(f"{statementEndLabel}:", file = file)
            self.elseStatement.compile(compiler, file)
            print(f"{elseEndLabel}:", file = file)
        else:
                print(f"{statementEndLabel}:", file = file)
        