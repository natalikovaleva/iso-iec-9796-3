BROOT := ../../../

.PHONY : clean

CXXFLAGS += -I$(BROOT)/include -I$(BROOT)/build-libmath/deps/include  -I. -flto -O2 -ftree-vectorize -march=native -ggdb

all: dss_ecnr_frnd

dss_ecnr_frnd: dss_ecnr_frnd.cpp $(BROOT)/lib/lib9796-3.a $(BROOT)/build-libmath/libmath/libmath.a
		@mkdir -p $(dir $@)
		$(CXX) -static -Wall $(CXXFLAGS) -o $@ $^ $(BROOT)/build-libmath/libmath/libmath.a
		find -name "*.gcda" -delete

clean: 
		rm -f dss_ecnr_frnd
