#!/bin/bash
set -x

OPT=""
if [ "$1" == "hosttest" ]
then
	OPT="${OPT} -DHOSTTEST=ON"
fi

cd `dirname $0`
rm -rf build
mkdir build
cd build

cmake .. ${OPT} && make VERBOSE=1 && ./tpk

cd ..

