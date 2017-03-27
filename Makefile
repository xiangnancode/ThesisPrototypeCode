NAME = code
CC = g++ 
CFLAGS = -Wall -g -ggdb `pkg-config --cflags opencv` -std=c++0x
LIBS = `pkg-config --libs opencv` -lwiringPi
OBJ =`basename $(NAME).cpp .cpp` 

all: 
	$(CC) $(CFLAGS) $(NAME).cpp $(LIBS) -o $(OBJ)

clean:
	rm -f $(OBJ) $(NAME)