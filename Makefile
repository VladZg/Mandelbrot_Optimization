TARGET = 1_NoAVX
FLAGS = #-mavx2 -O3 #-mavx512vl
SRC_DIR = ./Source
OBJ_DIR = ./Object

all: compile link run

compile:
	@g++ -c $(SRC_DIR)/$(TARGET).cpp $(FLAGS) -I/usr/include/ -o $(OBJ_DIR)/$(TARGET).o
	@g++ -c $(SRC_DIR)/AppUtils.cpp  $(FLAGS) -I/usr/include/ -o $(OBJ_DIR)/AppUtils.o

link:
	@g++ $(OBJ_DIR)/$(TARGET).o $(OBJ_DIR)/AppUtils.o -o $(TARGET) -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

run:
	@./$(TARGET)

clean:
	@rm -f $(OBJ_DIR)/*.o
