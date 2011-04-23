.PHONY: clean

SONAME=libdstu4145.so

CXXFLAGS ?= -O2 -march=native -flto -combine -ftree-vectorize -fomit-frame-pointer -pipe
INCLUDE  ?= -I . -I ../../../include
LIBS ?=  ../../../build-libmath/libmath/libmath.a ../../../lib/lib9796-3.a

all: $(SONAME)

$(SONAME): dstu4145_lib.cpp
		$(CXX) $(CXXFLAGS) $(INCLUDE) -fPIC -shared -o $@ $^ $(LIBS) \
				-Wl,--no-undefined,-soname,$(SONAME),--export-dynamic,--dynamic-list=dstu4145_syms.txt

clean:
		rm -f *.o
		rm -f $(SONAME)


