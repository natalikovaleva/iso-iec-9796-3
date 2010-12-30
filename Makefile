.PHONY : all clean

INCLUDE += -Intl-5.5.2/include

all:  sign 

# CXXFLAGS := -O2 -ftree-vectorize -fprofile-arcs -fwhole-program -combine -flto -pg
CXXFLAGS := -O0 -ggdb -fprofile-arcs -pg

%.o : %.cpp
		g++ -Wall $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

sign:	sign.o ec.o ec_defaults.o
		g++ -Wall $(CXXFLAGS) -o  $@ $^ ntl.a
		find -name "*.gcda" -delete

clean: 
		rm -rf *.o
		rm -rf sign