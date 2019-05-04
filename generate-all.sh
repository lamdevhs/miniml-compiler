#!/bin/bash

folder=test-programs

Msg ()
{
  echo "generated-all.sh: $1"
}

Crash ()
{
  Msg "unexpected error: $1 -- aborting"
  exit 1
}

Done ()
{
  Msg "done"
  exit 0
}

if [ "$1" = "clean" ] ; then
  echo rm ./$folder/*.c ./$folder/*.out
  echo press enter to delete those files
  read PAUSE
  rm ./$folder/*.c ./$folder/*.out
  Done
fi

make all || Crash "could not build ./comp"
[ -d ./ccam ] || Crash "could not find folder ./ccam"
[ -d ./$folder ] || Crash "could not find folder ./$folder"
! [ -e ./tmp_ccam ] || Crash "could not create temporary folder ./tmp_ccam"
cp -r ./ccam ./tmp_ccam || Crash "could not create temporary folder ./tmp_ccam"
cd ./tmp_ccam
for path in ../$folder/*.ml ; do
  filename="$(basename "$path")"
  shortname="${filename%.ml}"
  ../comp "$path" "$shortname.c" && \
    make build file="$shortname.c" out="$shortname.out" && \
    mv "$shortname.out" "../$folder/$shortname.out" && \
    make build file="$shortname.c" out="$shortname.out" DBG=y && \
    mv "$shortname.out" "../$folder/$shortname.dbg.out" && \
    mv "$shortname.c" "../$folder/$shortname.c"
done
cd ../
rm -r ./tmp_ccam || Msg "warning: could not erase temporary folder ./tmp_ccam"
Done
