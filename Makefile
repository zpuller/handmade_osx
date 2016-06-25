CC=g++
DEBUG=-DDEBUG=1 -g
FLAGS=-Wno-c++11-extensions -framework SFML -framework sfml-audio -framework sfml-graphics -framework sfml-window -framework sfml-system

debug: main.cc
	$(CC) $(DEBUG) $(FLAGS) -o main $<

release: main.cc
	$(CC) $(FLAGS) -o main $<

clean:
	rm -rf main main.dSYM/
