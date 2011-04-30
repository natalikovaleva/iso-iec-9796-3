.PHONY: clean

LIB=dstu4145
SONAME=lib$(LIB).so

MABI  ?= $(shell [ "$$(uname -m)" = "x86_64" ] && echo -m64 || echo -m32)

CXXFLAGS ?= -O0 -ggdb -march=native -pipe $(MABI)
INCLUDE  ?= -I . -I ../../../build-libmath/deps/include/ -I ../../../include
LIBS ?=  ../../../build-libmath/libmath/libmath.a ../../../lib/lib9796-3.a

all: $(SONAME) example

$(SONAME): dstu4145_lib.cpp
		$(CXX) $(CXXFLAGS) $(INCLUDE) -fPIC -shared -o $@ $^ $(LIBS) \
				-Wl,--no-undefined,-soname,$(SONAME),--export-dynamic,--dynamic-list=dstu4145_syms.txt

libsigndipc.so : signd-crypto.c signd-packets.c $(SONAME)
			$(CC) -o $@ $^ -shared -lconfig -fPIC -Wl,--no-undefined,-soname,$@,-rpath,$$ORIGIN,-rpath-link,.

signd: signd.c  signd-server.c libsigndipc.so
	 		$(CC) -o $@ $^ -lconfig -Wl,-rpath,$$ORIGIN,-rpath-link,. $(CXXFLAGS)

signd-client: signd-client.c libsigndipc.so
			$(CC) -o $@ $^  -Wl,-rpath,$$ORIGIN,-rpath-link,. $(CXXFLAGS)

example : example.c $(SONAME) 
		$(CC) -o $@ $^ -Wl,-rpath,$$ORIGIN $(CXXFLAGS)

clean:
		rm -f *.
		rm -f $(SONAME) example signd signd-client libsigndipc.so 


