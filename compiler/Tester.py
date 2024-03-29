import os, sys
from Compiler import Compiler
from nodes.Program import Program

#Create random program
output = open("out.c", "w")
p = Program.createRandom(None)
p.print(output)
output.close()

#Compile and run using gcc
os.system("gcc ./out.c -o out.exe") #Creates out.exe
expectedResult = os.system("out.exe") #Executes out.exe
print(f"Expected result: {expectedResult}")

#Compile and run the program using my compiler
source = open("out.c", "r")
output = open("out.asm", "w")
compiler = Compiler(source)
compiler.compile(output)
source.close()
output.close()

myResult = os.system(".\\bin\sim.exe")
print(f"My result: {myResult}")