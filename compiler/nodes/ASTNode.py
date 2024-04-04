from abc import ABC, abstractmethod
import Compiler

class ASTNode(ABC):
    @abstractmethod
    def parse(self, compiler : Compiler):
        ...

    @abstractmethod
    def print(self, file, indent = ""):
        ...
    
    @abstractmethod
    def compile(self, compiler : Compiler, file):
        ...

    @classmethod
    @abstractmethod
    def createRandom(cls, context):
        return cls()
        ...