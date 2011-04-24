.PHONY: clean

LIB=dstu4145
SONAME=lib$(LIB).so

CXXFLAGS ?= -O2 -ggdb -march=native -pipe
INCLUDE  ?= -I . -I ../../../build-libmath/deps/include/ -I ../../../include
LIBS ?=  ../../../build-libmath/libmath/libmath.a ../../../lib/lib9796-3.a

all: $(SONAME) example

$(SONAME): dstu4145_lib.cpp
		$(CXX) $(CXXFLAGS) $(INCLUDE) -fPIC -shared -o $@ $^ $(LIBS) \
				-Wl,--no-undefined,-soname,$(SONAME),--export-dynamic,--dynamic-list=dstu4145_syms.txt

signd: signd.c $(SONAME)
	 $(CC) -O -o $@  $^ -lconfig -Wl,-rpath,$$ORIGIN

example : example.c $(SONAME) 
		$(CC) -o $@ $^ -Wl,-rpath,$$ORIGIN

clean:
		rm -f *.o
		rm -f $(SONAME) example


