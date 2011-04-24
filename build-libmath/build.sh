#!/bin/sh

export CC=${CC:-gcc}
export CXX=${CXX:-g++}
export CFLAGS="${CFLAGS:-\"-O2 -march=native -pipe -fomit-frame-pointer\"}"
export CXXFLAGS="${CXXFLAGS:-\"-O2 -march=native -pipe -fomit-frame-pointer\"}"

export CFLAGS="${CFLAGS} -fPIC"
export CXXFLAGS="${CXXFLAGS} -fPIC --no-rtti"

ABI_NATIVE=$( [ "$(uname -m)" = "x86_64" ] && echo 64 || echo 32)

export ABI=${ABI:-${ABI_NATIVE}}

VNTL=5.5.2
VGF2X=1.0
VGMP=5.0.1

[ ! -d ntl-${VNTL} ] && ( ( wget -c http://www.shoup.net/ntl/ntl-${VNTL}.tar.gz && tar zxf ntl-${VNTL}.tar.gz ) || exit 1 )
[ ! -d gf2x-${VGF2X} ] && ( ( wget -c http://gforge.inria.fr/frs/download.php/27999/gf2x-${VGF2X}.tar.gz && tar zxf gf2x-${VGF2X}.tar.gz ) || exit 1 )
[ ! -d gmp-${VGMP} ] && ( ( wget -c ftp://ftp.gmplib.org/pub/gmp-${VGMP}/gmp-${VGMP}.tar.bz2 && tar jxf gmp-${VGMP}.tar.bz2 ) || exit 1 )

BUILDPWD=$(pwd)

cd "${BUILDPWD}/gmp-${VGMP}"
./configure \
    CC=$CC \
    CXX=$CXX \
    CFLAGS="${CFLAGS}" \
    CXXFLAGS="${CXXFLAGS}" \
    ABI="${ABI}" \
    --prefix="${BUILDPWD}/deps"

make install || exit 1

cd "${BUILDPWD}/gf2x-${VGF2X}"
./configure \
    CC=$CC \
    CXX=$CXX \
    CFLAGS="${CFLAGS}" \
    CXXFLAGS="${CXXFLAGS}" \
    --prefix="${BUILDPWD}/deps"
make install || exit 1

cd "$BUILDPWD/ntl-${VNTL}/src"

make clean

./configure \
    CC=$CC \
    CXX=$CXX \
    CFLAGS="${CFLAGS}" \
    CXXFLAGS="${CXXFLAGS}" \
    GF2X_PREFIX="${BUILDPWD}/deps/" \
    GMP_PREFIX="${BUILDPWD}/deps/" \
    LDFLAGS="-Wl,-rpath,\"${BUILDPWD}/deps/lib\""\
    LDLIBS="${BUILDPWD}/deps/lib/libgmp.a ${BUILDPWD}/deps/lib/libgf2x.a"\
    NTL_GMP_LIP=on \
    NTL_GF2X_LIB=on \
    NTL_AVOID_BRANCHING=on \
    PREFIX="${BUILDPWD}/deps" \
    WIZARD=off

rm -f ntl.a
make
make ntl.a  || exit 1
make install || exit 1


mkdir -p "$BUILDPWD/libmath"
cd "$BUILDPWD/libmath"

echo "Building fat archive"


for archive in ${BUILDPWD}/deps/lib/*.a ; do
 ar x ${archive}
done

ar ruv libmath.a *.o
ranlib libmath.a
rm -f *.o

cd "${BUILDPWD}"
echo DONE

patch -p0 << __EOF__
--- deps/include/NTL/tools.h.orig	2011-04-23 22:10:01.124612099 +0300
+++ deps/include/NTL/tools.h	2011-04-23 22:09:43.102649844 +0300
@@ -267,7 +267,7 @@
 
 #else
 
-inline void ForceToMem(double *p) { }
+inline void ForceToMem(double *p __attribute__((unused))) { }
 
 #endif
 
--- deps/include/NTL/ZZ.h.orig	2011-04-23 22:09:15.100708556 +0300
+++ deps/include/NTL/ZZ.h	2011-04-23 22:10:58.907491310 +0300
@@ -1994,7 +1994,7 @@
 
 #define NTL_SPMM_VEC_T vec_double
 
-static inline double PrepMulModPrecon(long b, long n, double ninv)
+static inline double PrepMulModPrecon(long b, long n __attribute__((unused)), double ninv)
 {
    return ((double) b) * ninv;
 }
__EOF__

[ -f libmath/libmath.a  ] || exit 1

