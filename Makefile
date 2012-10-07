.PHONY : all clean

TOP := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

INCLUDE += -I$(TOP)/build-libmath/deps/include

INCLUDE += -I$(TOP)/include/

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

LTO   := -fwhole-program -flto
LOOPS := -ftree-vectorize
FEATURES ?= lto loops nortti

ABI  ?= $(shell [ "$$(uname -m)" = "x86_64" ] && echo 64 || echo 32)

CFLAGS ?= -O2 -ggdb2 -march=native -fPIC -fvisibility=hidden -m$(ABI)
CXXFLAGS ?= $(CFLAGS)
WARNINGS := -Wall -Wextra -pedantic -Winit-self

ifeq ($(findstring lto,$(FEATURES)),lto)
 CFLAGS   += $(LTO)
 CXXFLAGS += $(LTO)
endif

ifeq ($(findstring loops,$(FEATURES)),loops)
 CFLAGS   += $(LOOPS)
 CXXFLAGS += $(LOOPS)
endif

ifeq ($(findstring nortti,$(FEATURES)),nortti)
 CXXFLAGS += --no-rtti
endif


AFFINE_ZZ_P := utils.o ec.o ec_defaults.o ec_compress.o
AFFINE_GF2X := utils.o ec.o ec_defaults.o ec_compress.o

PROJ_GF2X   := ec.o
PROJ_ZZ_P		:= ec.o

HASHES := rmd160.o sha512.o sha1.o
GENERIC := octet.o blob.o hash.o mgf.o convhex.o zz_utils.o gf2x_utils.o

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
		$(CXX) $(CXXFLAGS) $(WARNINGS) $(INCLUDE) -fvisibility-inlines-hidden -c -o $@ $(TOP)/$<

build/%.o : src/%.c
		@mkdir -p $(dir $@)
		$(CC) --std=gnu99 $(CFLAGS) $(WARNINGS) $(INCLUDE) -c -o $@ $(TOP)/$<

# Examples builds to cwd
%: build/examples/%.o  lib/lib9796-3.a build-libmath/libmath/libmath.a
		@mkdir -p $(dir $@)
		$(CXX) -static -Wall $(CXXFLAGS) -o $@ $^ build-libmath/libmath/libmath.a $(LDFLAGS)
		find -name "*.gcda" -delete

%: build/tests/%.o  lib/lib9796-3.a build-libmath/libmath/libmath.a
		@mkdir -p $(dir $@)
		$(CXX) -static  -Wall $(CXXFLAGS) -o $@ $^ build-libmath/libmath/libmath.a
		find -name "*.gcda" -delete

build-libmath/libmath/libmath.a:
	  cd build-libmath/ && 			\
		  CXXFLAGS="$(CXXFLAGS)" 	\
			CFLAGS="$(CFLAGS)" 			\
			GCC="$(CC)" 						\
			GXX="$(CXX)" 						\
			ABI=$(ABI)							\
				sh ./build.sh

clean:
		[ -d build ] && find build -name "*.o" -delete || true
		rm -rf build
		rm -rf lib
		rm -rf $(DSS) $(POC) $(SIGNS)
