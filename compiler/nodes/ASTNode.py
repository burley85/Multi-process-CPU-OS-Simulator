from abc import ABC, abstractmethod
import Compiler

class ASTNode(ABC):
    @abstractmethod
    def __str__(self):
        ...
        
    @abstractmethod
    def parse(self, compiler : Compiler):
        ...
    
    @abstractmethod
    def compile(self, compiler : Compiler, file):
        ...

    @classmethod
    @abstractmethod
    def createRandom(cls, context):
        return cls()
        ...