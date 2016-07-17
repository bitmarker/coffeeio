ROOTDIR=`pwd`

mkdir -p build/src_generated
cd build

cmake ${ROOTDIR}
make
