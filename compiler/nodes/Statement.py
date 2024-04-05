from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode
from abc import abstractmethod
import Randomizer
'''<statement> ::= <if_statement> | <while_statement> | <for_statement> | <return_statement> |
                   <block> | <arithmetic_statement> | ";"'''
class Statement(ASTNode):    
    @staticmethod
    def parse(compiler : Compiler, stackOffset):
        import nodes.IfStatement as IfStatement
        import nodes.WhileStatement as WhileStatement
        #import ForStatement as ForStatement
        import nodes.ReturnStatement as ReturnStatement
        import nodes.ArithmeticStatement as ArithmeticStatement
        import nodes.Block as Block
        import nodes.EmptyStatement as EmptyStatement
        if compiler.currentToken().type != TokenType.SEMICOLON:
            map = {
                TokenType.IF : IfStatement.IfStatement,
                TokenType.WHILE : WhileStatement.WhileStatement,
                #TokenType.FOR : ForStatement.ForStatement,
                TokenType.RETURN : ReturnStatement.ReturnStatement,
                TokenType.LBRACE : Block.Block,
            }
            statement = map.get(compiler.currentToken().type, ArithmeticStatement.ArithmeticStatement)()
            statement.parse(compiler, stackOffset)

        else: statement = EmptyStatement.EmptyStatement().parse(compiler, stackOffset)
        return statement

    @property
    @abstractmethod
    def stackSize(self) -> int:
        ...

    @classmethod
    def createRandom(cls, context):
        from nodes.IfStatement import IfStatement
        from nodes.WhileStatement import WhileStatement
        #from nodes.ForStatement import ForStatement
        from nodes.ReturnStatement import ReturnStatement
        from nodes.ArithmeticStatement import ArithmeticStatement
        from nodes.Block import Block
        from nodes.EmptyStatement import EmptyStatement
        weights = {
            IfStatement : .1,
            WhileStatement : .1,
            ReturnStatement : 1,
            ArithmeticStatement : 1,
            Block : .25,
            EmptyStatement : 1,
        }
        return Randomizer.weightedChoice(weights).createRandom(context)