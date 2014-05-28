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

all: expcnf.o tseitin.o
	g++ $(FLAGS) expcnf.o tseitin.o -o expcnf
expcnf.o: expcnf.cpp
	g++ $(FLAGS) -c expcnf.cpp
tseitin.o: tseitin.cpp tseitin.h
	g++ $(FLAGS) -c tseitin.cpp


# make clean - remove .o files.  You can modify it to remove whatever
clean:
	rm -f *.o
	rm -f expcnf
	
