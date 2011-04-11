#!/bin/sh

export CC=${CC:-gcc}
export CXX=${CXX:-g++}
export CFLAGS=${CFLAGS:-"-O2 -march=native -pipe -fomit-frame-pointer"}
export CXXFLAGS=${CXXFLAGS:-"-O2 -march=native -pipe -fomit-frame-pointer"}

export CFLAGS="${CFLAGS} -flto -fwhole-program -ftree-vectorize  -floop-interchange -floop-strip-mine -floop-block"
export CXXFLAGS="${CXXFLAGS} -flto -fwhole-program -ftree-vectorize  -floop-interchange -floop-strip-mine -floop-block --no-rtti"

ABI_NATIVE=$( [ "$(uname -m)" = "x86_64" ] && echo 64 || echo 32)

export ABI=${ABI:-${ABI_NATIVE}}

VNTL=5.5.2
VGF2X=1.0
VGMP=5.0.1

[ ! -d ntl-${VNTL} ] && ( ( wget -c http://www.shoup.net/ntl/ntl-${VNTL}.tar.gz && tar zxf ntl-${VNTL}.tar.gz ) || exit 1 )
[ ! -d gf2x-${VGF2X} ] && ( ( wget -c https://gforge.inria.fr/frs/download.php/27999/gf2x-${VGF2X}.tar.gz && tar zxf gf2x-${VGF2X}.tar.gz ) || exit 1 )
[ ! -d gmp-${VGMP} ] && ( ( wget -c ftp://ftp.gmplib.org/pub/gmp-${VGMP}/gmp-${VGMP}.tar.bz2 && tar jxf gmp-${VGMP}.tar.bz2 ) || exit 1 )

BUILDPWD=$(pwd)

cd "${BUILDPWD}/gmp-${VGMP}"
./configure \
    CC=$CC \
    CXX=$CXX \
    CFLAGS="${CFLAGS} -m32" \
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

[ -f libmath/libmath.a  ] || exit 1

