#!/bin/bash
set -e -x

SOURCEDIR=$(cd "$(dirname "$0")"; pwd -P)

BASEDIR=$(mktemp -d)

cd $BASEDIR
mkdir -p build
cd build

{
	git clone https://github.com/c3-projects/nu.git
	cd nu
	git checkout develop
	cd ..
}

{
	cmake "$SOURCEDIR" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$BASEDIR/build/nu"
	cmake --build .
	cp *oxidising-ocelots-backend* $1
	cd $BASEDIR
	rm -rf build
}

rm -rf $BASEDIR
