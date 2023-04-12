TARGET = SSE

NoAVX_FLAGS  =
SSE_FLAGS    = -mavx
AVX2_FLAGS   = -mavx2
AVX512_FLAGS = -mavx512vl -mavx512bw
OPTM_FLAGS   = -Ofast

FLAGS = $($(TARGET)_FLAGS) $(OPTM_FLAGS)
SRC_DIR = ./Source
OBJ_DIR = ./Object

LIBS = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

all: compile link run

compile:
	@g++ -c $(SRC_DIR)/$(TARGET).cpp $(FLAGS) -I/usr/include/ -o $(OBJ_DIR)/$(TARGET).o
	@g++ -c $(SRC_DIR)/AppUtils.cpp  $(FLAGS) -I/usr/include/ -o $(OBJ_DIR)/AppUtils.o
	@g++ -c $(SRC_DIR)/IntrinsicsPrintf.cpp  $(FLAGS) -o $(OBJ_DIR)/IntrinsicsPrintf.o

link:
	@g++ $(OBJ_DIR)/$(TARGET).o $(OBJ_DIR)/AppUtils.o $(OBJ_DIR)/IntrinsicsPrintf.o -o $(TARGET) $(LIBS)

run:
	@./$(TARGET)

clean:
	@rm -f $(OBJ_DIR)/*.o
