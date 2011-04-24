.PHONY : all clean

INCLUDE += -Ibuild-libmath/deps/include

INCLUDE += -Iinclude/

SIGNS := ecnr  \
				 ecmr	 \
				 ecao  \
				 ecpv  \
				 ecknr

POC := 	compressgf2x \
				basis				 \
				signmeasure	 \
				signecnrgfbench \
				signgfbench \
				signkfix

DSS := dss_ecknr \
			 dss_ecnr	 \
			 dss_ecpv	 \
			 dss_ecmr	 \
			 dss_ecao

all: build-libmath/libmath/libmath.a $(DSS)

CFLAGS := -O2 -ftree-vectorize -fwhole-program -combine -flto -march=native -fPIC -fvisibility=hidden
# CFLAGS := -O0 -fPIC -ggdb -flto -fvisibility=hidden
CXXFLAGS := $(CFLAGS)
WARNINGS := -Wall -Wextra -pedantic -Winit-self

AFFINE_ZZ_P := utils.o ec.o ec_defaults.o ec_compress.o
AFFINE_GF2X := utils.o ec.o ec_defaults.o ec_compress.o

PROJ_GF2X   := ec.o
PROJ_ZZ_P		:= ec.o

HASHES := rmd160.o sha512.o sha1.o
GENERIC := octet.o hash.o mgf.o convhex.o zz_utils.o gf2x_utils.o

lib/lib9796-3.a : $(addprefix build/ec/ZZ_p/affine/,    $(AFFINE_ZZ_P)) \
									$(addprefix build/ec/GF2X/affine/,    $(AFFINE_GF2X)) \
									$(addprefix build/ec/GF2X/projective/,$(PROJ_GF2X))	  \
									$(addprefix build/ec/ZZ_p/projective/,$(PROJ_ZZ_P))	  \
									$(addprefix build/hashes/,            $(HASHES))		  \
									$(addprefix build/generic/,			      $(GENERIC))
		@mkdir -p $(dir $@)
		rm -f $@
		$(AR) r $@ $^

build/%.o : src/%.cpp
		@mkdir -p $(dir $@)
		$(CXX) $(CXXFLAGS) $(WARNINGS) $(INCLUDE) -fvisibility-inlines-hidden --no-rtti -c -o $@ $<

build/%.o : src/%.c
		@mkdir -p $(dir $@)
		$(CC) --std=gnu99 $(CFLAGS) $(WARNINGS) $(INCLUDE) -c -o $@ $<

# Examples builds to cwd
%: build/examples/%.o  lib/lib9796-3.a build-libmath/libmath/libmath.a
		@mkdir -p $(dir $@)
		$(CXX) -static -Wall $(CXXFLAGS) -o $@ $^ build-libmath/libmath/libmath.a
		find -name "*.gcda" -delete

%: build/tests/%.o  lib/lib9796-3.a build-libmath/libmath/libmath.a
		@mkdir -p $(dir $@)
		$(CXX) -static  -Wall $(CXXFLAGS) -o $@ $^ build-libmath/libmath/libmath.a
		find -name "*.gcda" -delete

build-libmath/libmath/libmath.a:
	  cd build-libmath/ && \
	  CXXFLAGS="$(CXXFLAGS)" CFLAGS="$(CFLAGS)" GCC="$(CC)" GXX="$(CXX)" sh ./build.sh

clean:
		[ -d build ] && find build -name "*.o" -delete || true
		rm -rf build
		rm -rf lib
		rm -rf $(DSS) $(POC) $(SIGNS)
