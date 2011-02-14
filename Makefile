.PHONY : all clean

INCLUDE += -Intl-5.5.2/include

INCLUDE += -Iinclude/

all: sign signgf compressgf2x signmeasure signgfbench signecnrgfbench

CXXFLAGS := -O2 -march=native -ftree-vectorize -fprofile-arcs -fwhole-program -combine -flto -pg
#CXXFLAGS := -O0 -ggdb -fprofile-arcs -pg  
WARNINGS := -Wall -Wextra -pedantic -Winit-self

AFFINE_ZZ_P := utils.o ec.o ec_defaults.o ec_compress.o
AFFINE_GF2X := utils.o ec.o ec_defaults.o ec_compress.o
#AFFINE_ZZ_PX:= utils.o ec.o ec_defaults.o ec_compress.o

HASHES := rmd160.o sha512.o sha1.o
GENERIC := octet.o hash.o mgf.o convhex.o

lib/lib9796-3.a : $(addprefix build/affine/ZZ_p/, $(AFFINE_ZZ_P)) \
									$(addprefix build/affine/GF2X/, $(AFFINE_GF2X)) \
									$(addprefix build/affine/ZZ_pX/,$(AFFINE_ZZ_PX))\
									$(addprefix build/hashes/,      $(HASHES))			\
									$(addprefix build/generic/,			$(GENERIC))	
		@mkdir -p $(dir $@)
		rm -f $@
		$(AR) r $@ $^

build/%.o : src/%.cpp
		@mkdir -p $(dir $@)
		g++ $(CXXFLAGS) $(WARNINGS) $(INCLUDE) --no-rtti -c -o $@ $<

build/%.o : src/%.c
		@mkdir -p $(dir $@)
		gcc --std=gnu99 $(CXXFLAGS) $(WARNINGS) $(INCLUDE) -c -o $@ $<

sign:	build/examples/sign.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

signgf:	build/examples/signgf.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

compressgf2x:	build/examples/compressgf2x.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

signmeasure:	build/examples/signmeasure.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

signecnrgfbench:	build/examples/signecnrgfbench.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -static -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete


signgfbench:	build/examples/signgfbench.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -static -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete


clean: 
		[ -d build ] && find build -name "*.o" -delete || true
		rm -rf build
		rm -rf lib
		rm -rf sign
