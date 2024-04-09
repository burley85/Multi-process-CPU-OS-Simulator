from enum import Enum
import io

TokenType = Enum("TokenType", "CHAR, SHORT, INT, LONG, UNSIGNED, VOID, IF, ELSE, FOR, WHILE,\
                 RETURN, SIZEOF, INCREMENT, DECREMENT, EQUALS, NOT_EQUALS, LESS, GREATER,\
                 LESS_EQUALS, GREATER_EQUALS, AND, OR, NOT, ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN,\
                 MULTIPLY_ASSIGN, DIVIDE_ASSIGN, MODULO_ASSIGN, DEREF, ADDR, LPAREN, RPAREN,\
                 LBRACE, RBRACE, LBRACKET, RBRACKET, SEMICOLON, COMMA, IDENTIFIER, NUMBER, STRING,\
                 CHARACTER, EOF, UNKNOWN")

class Token:
    def __init__(self, type : TokenType, value : str | int | None = None):
        self.type = type
        if type in [TokenType.IDENTIFIER, TokenType.STRING, TokenType.CHARACTER]:
            #Make sure the value is a string
            assert isinstance(value, str)
            if type == TokenType.CHARACTER: assert len(value) == 1
            self.value = value
        if type == TokenType.NUMBER:
            #Make sure the value is an int
            assert isinstance(value, int)
            self.value = value

    def __str__(self):
        if self.isKeyword(self):
            return self.type.name.lower()
        elif self.type == TokenType.IDENTIFIER:
            return self.value
        elif self.type == TokenType.NUMBER:
            return str(self.value)
        elif self.type == TokenType.STRING:
            return f'"{self.value}"'
        elif self.type == TokenType.CHARACTER:
            return f"'{self.value}'"
        elif self.type == TokenType.EOF:
            return ""     
        elif self.type == TokenType.UNKNOWN:
            return "?"
        else:
            map = {
                TokenType.INCREMENT: "++",
                TokenType.DECREMENT: "--",
                TokenType.EQUALS: "==",
                TokenType.NOT_EQUALS: "!=",
                TokenType.LESS: "<",
                TokenType.GREATER: ">",
                TokenType.LESS_EQUALS: "<=",
                TokenType.GREATER_EQUALS: ">=",
                TokenType.AND: "&&",
                TokenType.OR: "||",
                TokenType.NOT: "!",
                TokenType.ASSIGN: "=",
                TokenType.PLUS_ASSIGN: "+=",
                TokenType.MINUS_ASSIGN: "-=",
                TokenType.MULTIPLY_ASSIGN: "*=",
                TokenType.DIVIDE_ASSIGN: "/=",
                TokenType.MODULO_ASSIGN: "%=",
                TokenType.DEREF: "*",
                TokenType.ADDR: "&",
                TokenType.LPAREN: "(",
                TokenType.RPAREN: ")",
                TokenType.LBRACE: "{",
                TokenType.RBRACE: "}",
                TokenType.LBRACKET: "[",
                TokenType.RBRACKET: "]",
                TokenType.SEMICOLON: ";",
                TokenType.COMMA: ","
            }
            assert self.type in map
            return map[self.type]
        
    def isKeyword(self, token):
        return token.type in [TokenType.CHAR, TokenType.SHORT, TokenType.INT, TokenType.LONG,
                              TokenType.UNSIGNED, TokenType.VOID, TokenType.IF, TokenType.ELSE,
                              TokenType.FOR, TokenType.WHILE, TokenType.RETURN, TokenType.SIZEOF]

class Tokenizer:
    def __init__(self, source):
        if hasattr(source, "read"):
            self.source = source
        else:
            self.source = io.StringIO(source)
        self.position = 0
        self.token = Token(TokenType.UNKNOWN)
        self.nextToken()

    def nextToken(self):
        keywordMap = {
            "char": TokenType.CHAR, "short": TokenType.SHORT, "int": TokenType.INT,
            "long": TokenType.LONG, "unsigned": TokenType.UNSIGNED, "void": TokenType.VOID,
            "if": TokenType.IF, "else": TokenType.ELSE, "for": TokenType.FOR,
            "while": TokenType.WHILE, "return": TokenType.RETURN, "sizeof": TokenType.SIZEOF,
        }
        oneCharMap = {
            "<": TokenType.LESS, ">": TokenType.GREATER, "!": TokenType.NOT, "=": TokenType.ASSIGN,
            "*": TokenType.DEREF, "&": TokenType.ADDR, "(": TokenType.LPAREN, ")": TokenType.RPAREN,
            "{": TokenType.LBRACE, "}": TokenType.RBRACE, "[": TokenType.LBRACKET,
            "]": TokenType.RBRACKET, ";": TokenType.SEMICOLON, ",": TokenType.COMMA
        }
        twoCharMap = {
            "++": TokenType.INCREMENT, "--": TokenType.DECREMENT, "==": TokenType.EQUALS,
            "!=": TokenType.NOT_EQUALS, "<=": TokenType.LESS_EQUALS, ">=": TokenType.GREATER_EQUALS,
            "&&": TokenType.AND, "||": TokenType.OR, "+=": TokenType.PLUS_ASSIGN,
            "-=": TokenType.MINUS_ASSIGN, "*=": TokenType.MULTIPLY_ASSIGN,
            "/=": TokenType.DIVIDE_ASSIGN, "%=": TokenType.MODULO_ASSIGN,
        }

        s = self.source.read(1)
        while s.isspace(): s = self.source.read(1)

        if s == "":
            self.token = Token(TokenType.EOF)
            return self.token

        if not s.isalnum() and s != "_":
            #Check if it's a two character token
            originalPosition = self.source.tell()
            s += self.source.read(1)
            if s in twoCharMap:
                self.token = Token(twoCharMap[s])
            #Otherwise, it must be a one character token
            else:
                s = s[0]
                self.token = Token(oneCharMap.get(s, TokenType.UNKNOWN))
                if self.token.type == TokenType.UNKNOWN: print(f"WARNING: Unrecognized character '{s}'")
                self.source.seek(originalPosition)

        elif s.isalpha() or s == "_":
            #Check if it is a keyword, otherwise it must be an identifier
            originalPosition = self.source.tell()
            c = self.source.read(1)
            while c.isalnum() or c == "_":
                originalPosition = self.source.tell()
                s += c
                c = self.source.read(1)
            self.source.seek(originalPosition)
            self.token = \
                Token(keywordMap[s]) if s in keywordMap else Token(TokenType.IDENTIFIER, s)


        elif s.isdigit():
            #Read the whole number
            originalPosition = self.source.tell()
            c = self.source.read(1)
            while c.isdigit():
                originalPosition = self.source.tell()
                s += c
                c = self.source.read(1)
            self.source.seek(originalPosition)
            self.token = Token(TokenType.NUMBER, int(s))
        
        self.position = self.source.tell()
        return self.token

    def currentToken(self):
        return self.token