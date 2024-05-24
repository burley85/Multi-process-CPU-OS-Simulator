import os, sys
from Compiler import Compiler
from nodes.Program import Program
from Randomizer import RandomizerContext

def run_test_cases(num_tests : int):
    testCasePrefix = "compiler/test/test"
    testCaseSuffix = ".c"
    outputPrefix = "compiler/test/output"
    outputSuffix = ".asm"

    failuresToOpen = 0 # Abort when 3 consecutive files fail to open
    testCasesPassed = 0 

    reasonForFailure = { }
    for k in range(num_tests): reasonForFailure[k] = None

    i = 0
    for i in range(1, num_tests + 1):
        if failuresToOpen == 3:
            print("ERROR: Repeated failure to open test files. Aborting test cases.")
            return

        # Open test case
        fInName = testCasePrefix + str(i) + testCaseSuffix
        try: testCase = open(fInName, "r")
        except:
            print(f"ERROR: Failed to open {fInName}")
            failuresToOpen += 1
            reasonForFailure[i] = "File open error"
            print(f"Test case {i}: FAILED")
            continue

        # Open output file
        if saveAllTestCases or saveFailedCases: fOutName = outputPrefix + str(i) + outputSuffix
        else: fOutName = testCasePrefix + outputSuffix
        try: output = open(fOutName, "w")
        except:
            print(f"ERROR: Failed to open {fOutName}")
            failuresToOpen += 1
            reasonForFailure[i] = "File open error"
            print(f"Test case {i}: FAILED")
            testCase.close()
            continue
        failuresToOpen = 0

        # Compile with GCC
        gccCommand = f'gcc {fInName} -o out.exe'
        rval = os.system(gccCommand)
        if rval != 0:
            print(f"ERROR: Failed to compile {fInName} with GCC")
            reasonForFailure[i] = "GCC error"
            print(f"Test case {i}: FAILED")
            continue

        # Compile using my compiler
        if verbose: print(f"Compiling '{fInName}' to '{fOutName}'")
        compiler = Compiler(testCase)
        try: compiler.compile(output)
        except: 
            print(f"ERROR: Failed to compile {fInName} with Compiler")
            reasonForFailure[i] = "Compiler error"
            print(f"Test case {i}: FAILED")
        testCase.close()
        output.close()

        # Run
        myResult = os.system(f".\\bin\sim.exe {fOutName} > nul 2>&1")
        if verbose: print(f"My result: {myResult}")
        expectedResult = os.system("out.exe")
        if verbose: print(f"Expected result: {expectedResult}")

        # Compare
        success = expectedResult == myResult
        if success:
            if verbose: print(f"Test case {i}: SUCCESS")
            testCasesPassed += 1
            if not saveAllTestCases: os.remove(fOutName)
        if not success: 
            print(f"Test case {i}: FAILED")
            reasonForFailure[i] = "Unexpected result"
            if not saveFailedCases: os.remove(fOutName)

    print(f"{testCasesPassed} / {i} test cases passed")
    for j in range(1, i + 1):
        print("Failed test cases:")
        if reasonForFailure[j]: print(f"Test case {j}: ERROR: {reasonForFailure[j]}")

def test_random():
    i = 0
    while True:
        i += 1
        if i % 1000 == 0:
            print(f"Generated {i} programs")
        output = open("out.c", "w")
        context = RandomizerContext()
        p = Program.createRandom(context)
        print(p, file = output)
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
        exit()

verbose = True

saveAllTestCases = False
saveFailedCases = True
saveFailedCases = saveFailedCases or saveAllTestCases

run_test_cases(10)