.PHONY : all clean

INCLUDE += -Intl-5.5.2/include

INCLUDE += -Iinclude/

EXAMPLES := dss_ecknr \
						dss_ecnr	\
						dss_ecmr	\
						dss_ecao	\
						ecnr 			\
						ecmr			\
						ecao 			\
						ecpv 			\
						ecknr			\
					  compressgf2x basis signmeasure signecnrgfbench signgfbench signkfix

all: $(EXAMPLES)

CXXFLAGS := -O2 -ftree-vectorize -fprofile-arcs -fwhole-program -combine -flto -pg
# CXXFLAGS := -O0 -ggdb -fprofile-arcs -pg 
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
		g++ $(CXXFLAGS) $(WARNINGS) $(INCLUDE) --no-rtti -c -o $@ $<

build/%.o : src/%.c
		@mkdir -p $(dir $@)
		gcc --std=gnu99 $(CXXFLAGS) $(WARNINGS) $(INCLUDE) -c -o $@ $<

dss_ecknr:	build/examples/dss_ecknr.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

dss_ecnr:	build/examples/dss_ecnr.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

dss_ecmr:	build/examples/dss_ecmr.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

dss_ecao:	build/examples/dss_ecao.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

dss_ecpv:	build/examples/dss_ecpv.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

ecknr:	build/examples/ecknr.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

ecao:	build/examples/ecao.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

ecpv:	build/examples/ecpv.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

ecnr:	build/examples/ecnr.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

signkfix:	build/examples/signkfix.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

ecmr:	build/examples/ecmr.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

compressgf2x:	build/examples/compressgf2x.o  lib/lib9796-3.a
		@mkdir -p $(dir $@)
		g++ -Wall $(CXXFLAGS) -o $@ $^ libntl.a -lgmp
		find -name "*.gcda" -delete

basis:	build/examples/basis.o  lib/lib9796-3.a
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
		rm -rf $(EXAMPLES)
