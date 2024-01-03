CXX       := gcc
CXX_FLAGS := -ggdb -Werror=all

BIN     := bin
SRC     := src
MONITOR_SRC := monitor_src
INCLUDE := include
LIBRARIES   := -lwsock32
EXECUTABLE  := sim.exe
MONITOR_EXECUTABLE  := cpu_monitor.exe

all: $(BIN)/$(EXECUTABLE) $(BIN)/$(MONITOR_EXECUTABLE)

run: all
	py startup.py

$(BIN)/$(EXECUTABLE): $(SRC)/*.c $(INCLUDE)/*.h
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) -D COMPILE_MAIN_EXE $^ $(LIBRARIES) -o $@
	
$(BIN)/$(MONITOR_EXECUTABLE): $(SRC)/*.c $(INCLUDE)/*.h
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) -D COMPILE_MONITOR_EXE $^ $(LIBRARIES) -o $@

clean:
	-del /Q /F $(BIN)\*