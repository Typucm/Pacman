all: typacman

clean: 
	rm build/main.o

typacman: main.o
	g++ -std=c++11 build/main.o -lsfml-graphics -lsfml-window -lsfml-system -o build/typacman	
	cp src/img/* build/
	
main.o: 
	mkdir -p build
	g++ -std=c++11 -c src/main.cpp -o build/main.o

