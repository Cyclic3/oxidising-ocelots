#!/bin/bash
set -e -x

SOURCEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

BASEDIR=$(mktemp -d)

cd $BASEDIR
function nu() {
	git clone https://github.com/c3-projects/nu.git
	cd nu
	git checkout develop
	mkdir -p build
	cd build
	cmake ..
	cmake --build .
	cmake --build . --target test
#	cmake --build .
	if [ -f "/etc/debian_version" ]; then
		cmake --build . --target package
		sudo dpkg -i *.deb
	else
		cmake --build . --target install
	fi
	cd $BASEDIR
	rm -rf nu
}

{
	mkdir -p build
	cd build
	cmake "$SOURCEDIR" -DCMAKE_BUILD_TYPE=Debug
	cmake --build .
	cp *oxidising-ocelots-backend* ..
	cd $BASEDIR
	rm -rf build
}

echo Binary is in $BASEDIR
