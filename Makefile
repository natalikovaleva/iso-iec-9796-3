.PHONY : all clean

INCLUDE += -Intl-5.5.2/include

all:  sign 

CXXFLAGS := -O3 -march=native -ftree-vectorize -fwhole-program -combine -flto -flto-report -fprofile-correction -fprofile-use
# CXXFLAGS := -O2 -fprofile-correction -fprofile-generate -march=native -ftree-vectorize -fprofile-arcs -fwhole-program -combine -flto -pg
#CXXFLAGS := -O0 -ggdb -fprofile-arcs -pg 
WARNINGS := -Wall -Wextra -pedantic -Winit-self

%.o : %.cpp
		g++ $(CXXFLAGS) $(WARNINGS) $(INCLUDE) --no-rtti -c -o $@ $<

%.o : %.c
		gcc --std=gnu99 $(CXXFLAGS) $(WARNINGS) $(INCLUDE) -c -o $@ $<

sign:	sign.o \
			ec.o ec_defaults.o ec_compress.o \
			sha512.o rmd160.o hash.o \
			octet.o \
			utils.o
		g++ -Wall $(CXXFLAGS) -o  $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

clean: 
		rm -rf *.o
		rm -rf sign