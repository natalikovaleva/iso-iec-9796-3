.PHONY: clean

LIB=dstu4145
SONAME=lib$(LIB).so

CXXFLAGS ?= -O2 -ggdb -march=native -pipe -m32
INCLUDE  ?= -I . -I ../../../build-libmath/deps/include/ -I ../../../include
LIBS ?=  ../../../build-libmath/libmath/libmath.a ../../../lib/lib9796-3.a

all: $(SONAME) example

$(SONAME): dstu4145_lib.cpp
		$(CXX) $(CXXFLAGS) $(INCLUDE) -fPIC -shared -o $@ $^ $(LIBS) \
				-Wl,--no-undefined,-soname,$(SONAME),--export-dynamic,--dynamic-list=dstu4145_syms.txt

signd: signd.c $(SONAME)
	 $(CC) -o  $@  $^ -lconfig -Wl,-rpath,$$ORIGIN $(CXXFLAGS)

example : example.c $(SONAME) 
		$(CC) -o $@ $^ -Wl,-rpath,$$ORIGIN $(CXXFLAGS)

clean:
		rm -f *.o
		rm -f $(SONAME) example


