#Default Flags
FLAGS = -Wall -Wextra -pedantic -Wvla -std=c++11

# make release - will compile "all" with $(FLAGS) and the O3 flag
release: FLAGS += -O3
release: all

# make debug - will compile "all" with $(FLAGS) and the g flag
debug: FLAGS += -g
debug: all

# make gprof - will compile "all" with flags and -pg
gprof: FLAGS += -pg
gprof: all

all: expcnf.o tseitin.o sqlite3.o util.o
	g++ -lpthread -ldl $(FLAGS) expcnf.o tseitin.o sqlite3.o util.o -o expcnf
expcnf.o: expcnf.cpp
	g++ $(FLAGS) -c expcnf.cpp
tseitin.o: tseitin.cpp tseitin.h
	g++ $(FLAGS) -c tseitin.cpp
sqlite3.o: sqlite3.c sqlite3.h
	gcc -c sqlite3.c 
util.o: util.cpp util.h
	g++ $(FLAGS) -c util.cpp


# make clean - remove .o files.  You can modify it to remove whatever
clean:
	rm -f *.o
	rm -f expcnf
	
