#!/usr/bin/env bash

lastdir="$PWD"
thisdir="$(dirname $0)"
cd "$thisdir"
srcdir="$thisdir/../../src"
outdir="$thisdir/../out"
srcfiles=(`ls "$srcdir"`)
exitval=0

echo -n '('
for i in ${srcfiles[@]}; do
  if ! diff "$srcdir/$i" "$outdir/$i" &>/dev/null; then
    test "$i" = "${srcfiles[0]}" && echo
    echo -n " '$i'"
    if ! cp "$srcdir/$i" "$outdir/$i"; then
      echo ' (failed), '
      exitval=$((exitval + 1))
    else
      echo ', '
    fi
  fi
done
printf '%s%s\n' ')' " -> $outdir"

echo done.
exit $exitval
