import random
from typing import Any

def weightedChoice(weights : dict[Any, int | float]):
    """Returns a random element from a dictionary of weights."""
    return random.choices(list(weights.keys()), weights = list(weights.values()))[0]

def bernoulliDistribution(p: float):
    """Returns True with probability p and False with probability 1 - p."""
    return random.random() < p

def geometricDistribution(p: float):
    """Returns a random number generated from a geometric distribution with success 
    probability p."""
    result = 0
    while not bernoulliDistribution(p):
        result += 1
    return result

class RandomizerContext:
    def __init__(self):
        self.ID = [[]] #ID[0] is a list of functions, each list after that is a layer of nested scope
    def newDecl(self):
        id = ""
        redeclarationErrorProbability = .01
        validDuplicateProbability = .05
        if bernoulliDistribution(redeclarationErrorProbability) and len(self.ID[-1]) > 0:
            #Pick random ID that has already been declared in the current scope
            id = random.choice(self.ID[-1])
        elif bernoulliDistribution(validDuplicateProbability) and len(self.ID) > 1:
            #Pick random ID that has already been declared in an outer scope
            innermostScope = self.ID.pop()
            randomScope = random.choice(self.ID)
            if len(randomScope) > 0:
                id = random.choice(randomScope)
            else:
                id = self.uniqueID()
            self.ID.append(id)
        else:
            #Pick a unique ID
            id = self.uniqueID()
        self.ID[-1].append(id)
        return id
            
    def uniqueID(self):
        maxLen = 32
        IDLen = random.randint(1, maxLen)
        s = random.choice("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")
        while len(s) < IDLen:
            s += random.choice("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789")
        for scope in self.ID:
            if s in scope:
                return self.uniqueID()
        return s


    def enterScope(self):
        self.ID.append([])
    def exitScope(self):
        self.ID.pop()
    def randomVar(self):
        random.choice(random.choice(self.ID))
    def randomFunc(self):
        random.choice(self.ID[0])


