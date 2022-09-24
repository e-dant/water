#!/usr/bin/env bash

lastdir="$PWD"
cd "$(dirname $0)" \
  || exit 1
cd ../out \
  || exit 1
installdir="$PWD"
cd ../../src \
  || exit 1
./autogen.sh \
  || exit 1
./configure --prefix="$installdir" \
  || exit 1
make -j 7 \
  || exit 1
cd "$lastdir" \
  || exit 1
