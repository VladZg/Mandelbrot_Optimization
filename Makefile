TARGET = AVX2
FLAGS = -mavx2 -Ofast #-mavx512vl

all: compile link run

compile:
	@g++ -c $(TARGET).cpp $(FLAGS) -I/usr/include/ -o $(TARGET).o
	@g++ -c AppUtils.cpp  $(FLAGS) -I/usr/include/ -o AppUtils.o

link:
	@g++ $(TARGET).o AppUtils.o -o $(TARGET) -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

run:
	@./$(TARGET)

clean:
	@rm -f *.o
