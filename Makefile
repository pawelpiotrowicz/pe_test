CC=g++ -Wall -std=c++0x   


main: main.cpp
	$(CC) -o prog main.cpp -lpthread

san: main.cpp
	$(CC) -fsanitize=thread -o prog main.cpp -lpthread
