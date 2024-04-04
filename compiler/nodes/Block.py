from Compiler import Compiler, TokenType
from nodes.Declaration import Declaration
from nodes.Statement import Statement
import Randomizer

'''<block> ::= "{" <declaration_list> <statement_list> "}"
   <declaration_list> ::= "" | <declaration> ";" <declaration_list>
   <statement_list> ::= "" | <statement> <statement_list>'''
class Block(Statement):
    @property
    def stackSize(self):
        return self._stackSize

    def __init__(self):
        self.declarations = []
        self.statements = []
        self.stackOffset = 0
        self._stackSize = 0

    def __str__(self):
        s = "{\n"
        for declaration in self.declarations:
            s += str(declaration) + ";\n"
        for statement in self.statements:
            s += str(statement) + "\n"
        s += "}"
        s = s.replace("\n", "\n\t", s.count("\n") - 1)
        return s

    def parse(self, compiler : Compiler, stackOffset):
        self.stackOffset = stackOffset
        compiler.expect(TokenType.LBRACE)
        while compiler.currentToken().type in [TokenType.CHAR, TokenType.SHORT, TokenType.INT,
                                               TokenType.LONG, TokenType.UNSIGNED, TokenType.VOID]:
            declaration = Declaration().parse(compiler, self.stackOffset + self._stackSize)
            self.declarations.append(declaration)
            self._stackSize += declaration.size
            compiler.expect(TokenType.SEMICOLON)
        
        maxChildBlockSize = 0
        while compiler.currentToken().type != TokenType.RBRACE:
            statement = Statement.parse(compiler, self._stackSize + stackOffset)
            self.statements.append(statement)
            maxChildBlockSize = max(statement.stackSize, maxChildBlockSize)
        self._stackSize += maxChildBlockSize
        
        compiler.expect(TokenType.RBRACE)
        return self

    def compile(self, compiler : Compiler, file):
        compiler.enterBlock()
        for declaration in self.declarations:
            declaration.compile(compiler, file)
        for statement in self.statements:
            statement.compile(compiler, file)
        compiler.leaveBlock()

    @classmethod
    def createRandom(cls, context):
        obj = cls()
        context.enterScope()
        numberOfDeclarations = Randomizer.geometricDistribution(.1)
        numberOfStatements = Randomizer.geometricDistribution(.25)
        for i in range(numberOfDeclarations):
            obj.declarations.append(Declaration.createRandom(context))
        for i in range(numberOfStatements):
            stmt = Statement.createRandom(context)
            if stmt is not None:
                obj.statements.append(stmt)
        context.exitScope()
        return obj