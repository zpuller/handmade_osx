CC=g++
DEBUG=-DDEBUG=1 -g
FLAGS=-Wno-c++11-extensions 
DEPS=-framework SFML -framework sfml-audio -framework sfml-graphics -framework sfml-window -framework sfml-system

debug: main.cc handmade.o
	$(CC) $(DEBUG) $(FLAGS) $(DEPS) -o main $^

release: main.cc handmade.o
	$(CC) $(FLAGS) $(DEPS) -o main $^

%.o: %.cc
	$(CC) -c $(FLAGS) $<

clean:
	rm -rf main main.dSYM/ *.o
