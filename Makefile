CC=g++
FLAGS=-w -g -framework SFML -framework sfml-audio -framework sfml-graphics -framework sfml-window -framework sfml-system

main: main.cc
	$(CC) $(FLAGS) -o $@ $<
