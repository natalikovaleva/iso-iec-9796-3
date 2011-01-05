.PHONY : all clean

INCLUDE += -Intl-5.5.2/include

all:  sign 

# CXXFLAGS := -O2 -ftree-vectorize -fprofile-arcs -fwhole-program -combine -flto -pg
CXXFLAGS := -O0 -ggdb -fprofile-arcs -pg

%.o : %.cpp
		g++ -Wall -pedantic $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

%.o : %.c
		gcc -Wall -pedantic --std=c99 $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

sign:	sign.o ec.o ec_defaults.o ec_compress.o sha512.o hash.o
		g++ -Wall $(CXXFLAGS) -o  $@ $^ ntl.a
		find -name "*.gcda" -delete

clean: 
		rm -rf *.o
		rm -rf sign