CXX       := gcc
CXX_FLAGS := -ggdb

BIN     := bin
SRC     := src
INCLUDE := include
LIBRARIES   := -lwsock32
EXECUTABLE  := sim

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.c
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE)  $^ $(LIBRARIES) -o $@
	
clean:
	-rm $(BIN)/*