CC=g++
DEBUG=-DDEBUG=1 -g
FLAGS=-Wno-c++11-extensions -Wno-c++11-compat-deprecated-writable-strings
DEPS=-framework SFML -framework sfml-audio -framework sfml-graphics -framework sfml-window -framework sfml-system

debug: main.cc handmade.dylib
	$(CC) $(DEBUG) $(FLAGS) $(DEPS) -o main $^

release: main.cc handmade.dylib
	$(CC) $(FLAGS) $(DEPS) -o main $^

%.o: %.cc
	$(CC) -c $(FLAGS) $<

%.dylib: %.cc
	$(CC) -dynamiclib $(FLAGS) $< -o $@

clean:
	rm -rf main main.dSYM/ *.o *.dylib
