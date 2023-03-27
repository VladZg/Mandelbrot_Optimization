TARGET = 1_NoSSE

all: compile link run

compile:
	@g++ -c $(TARGET).cpp -I/usr/include/ -o $(TARGET).o
	@g++ -c AppUtils.cpp -I/usr/include/ -o AppUtils.o

link:
	@g++ $(TARGET).o AppUtils.o -o $(TARGET) -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

run:
	@./$(TARGET)

clean:
	@rm -f *.o
