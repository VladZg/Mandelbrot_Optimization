TARGET = AVX512

NoAVX_FLAGS  =
AVX2_FLAGS   = -mavx2
AVX512_FLAGS = -mavx512vl -mavx512bw
OPTM_FLAGS   =

FLAGS = $($(TARGET)_FLAGS) $(OPTIM_FLAGS)
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
