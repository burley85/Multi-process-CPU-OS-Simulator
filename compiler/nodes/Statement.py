from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode
from abc import abstractmethod

'''<statement> ::= <if_statement> | <while_statement> | <for_statement> | <return_statement> |
                   <block> | <arithmetic_statement> | ";"'''
class Statement(ASTNode):    
    @staticmethod
    def parse(compiler : Compiler, stackOffset):
        import nodes.IfStatement as IfStatement
        #import WhileStatement as WhileStatement
        #import ForStatement as ForStatement
        import nodes.ReturnStatement as ReturnStatement
        import nodes.ArithmeticStatement as ArithmeticStatement
        import nodes.Block as Block
        import nodes.EmptyStatement as EmptyStatement
        if compiler.currentToken().type != TokenType.SEMICOLON:
            map = {
                TokenType.IF : IfStatement.IfStatement,
                #TokenType.WHILE : WhileStatement.WhileStatement,
                #TokenType.FOR : ForStatement.ForStatement,
                TokenType.RETURN : ReturnStatement.ReturnStatement,
                TokenType.LBRACE : Block.Block,
            }
            statement = map.get(compiler.currentToken().type, ArithmeticStatement.ArithmeticStatement)()
            if(hasattr(statement, "stackSize")):
                statement.parse(compiler, stackOffset)
            else: statement.parse(compiler)

        else: statement = EmptyStatement.EmptyStatement()
        return statement

    @property
    @abstractmethod
    def stackSize(self) -> int:
        ...