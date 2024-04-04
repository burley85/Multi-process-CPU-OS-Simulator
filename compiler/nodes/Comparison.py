from Compiler import Compiler, TokenType
from nodes.ASTNode import ASTNode

'''<comparison> ::= <id_or_literal> <comparison_op> <id_or_literal>'''
class Comparison(ASTNode):
    def __init__(self):
        self.op1 = None
        self.operator = None
        self.op2 = None

    def parse(self, compiler : Compiler):
        idOrLiteral = [TokenType.IDENTIFIER, TokenType.NUMBER, TokenType.CHAR, TokenType.STRING]
        comparisonOperator = [TokenType.EQUALS, TokenType.NOT_EQUALS, TokenType.LESS,
                              TokenType.GREATER, TokenType.LESS_EQUALS, TokenType.GREATER_EQUALS,
                              TokenType.AND, TokenType.OR]
        self.op1 = compiler.expect(idOrLiteral)
        self.operator = compiler.expect(comparisonOperator).type
        self.op2 = compiler.expect(idOrLiteral)
        return self
    
    def print(self, file, indent = ""):
        print(f"{self.op1} {self.operator} {self.op2}", end = "")

    def compile(self, compiler: Compiler, file):
        #Load op1 into rax
        if(self.op1.type == TokenType.IDENTIFIER):
            decl = compiler.findDeclaration(self.op1.value)
            print(f"rbp {'-' if decl.stackOffset > 0 else '+'} {abs(decl.stackOffset)}", file = file)
            print("rax = (rbp)", file = file)
            print(f"rbp {'+' if decl.stackOffset > 0 else '-'} {abs(decl.stackOffset)}", file = file)
        else:
            print(f"rax = {self.op1.value}", file = file)

        #Load op2 into rbx
        if(self.op2.type == TokenType.IDENTIFIER):
            decl = compiler.findDeclaration(self.op2.value)
            print(f"rbp {'-' if decl.stackOffset > 0 else '+'} {abs(decl.stackOffset)}", file = file)
            print("rbx = (rbp)", file = file)
            print(f"rbp {'+' if decl.stackOffset > 0 else '-'} {abs(decl.stackOffset)}", file = file)
        else:
            print(f"rbx = {self.op2.value}", file = file)
        
        codeMap = {
            TokenType.EQUALS:
                f"rax - rbx\n\
                jnz l1\n\
                rax = 1\n\
                jmp l2\n\
                l1:\n\
                rax = 0\n\
                l2:\n",
            TokenType.NOT_EQUALS:
                f"rax - rbx\n\
                jz l1\n\
                rax = 1\n\
                l1:\n",
            TokenType.LESS:
                f"rax - rbx\n\
                jns l1\n\
                rax = 1\n\
                jmp l2\n\
                l1:\n\
                rax = 0\n\
                l2:\n",
            TokenType.GREATER:
                f"rax - rbx\n\
                js l1\n\
                jz l1\n\
                rax = 1\n\
                jmp l2\n\
                l1:\n\
                rax = 0\n\
                l2:\n",
            TokenType.LESS_EQUALS:
                f"rax - rbx\n\
                js l1\n\
                jz l1\n\
                rax = 0\n\
                jmp l2\n\
                l1:\n\
                rax = 1\n\
                l2:\n",
            TokenType.GREATER_EQUALS:
                f"rax - rbx\n\
                jns l1\n\
                rax = 0\n\
                jmp l2\n\
                l1:\n\
                rax = 1\n\
                l2:\n",
            TokenType.AND:
                f"rax * rbx\n\
                jz l1\n\
                rax = 1\n\
                l1:\n",
            TokenType.OR:
                f"rax + rbx\n\
                jz l1\n\
                rax = 1\n\
                l1:\n"
        }
        code = codeMap[self.operator]
        if "l1" in code: code = code.replace("l1", compiler.newLabel())
        if "l2" in code: code = code.replace("l2", compiler.newLabel())
        print(code, file = file)