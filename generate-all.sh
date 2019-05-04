#!/bin/bash

folder=./test-programs

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
  echo rm $folder/*.c $folder/*.out
  echo press enter to delete those files
  read PAUSE
  rm $folder/*.c $folder/*.out
  Done
fi

make all || Crash "could not build ./comp"
cd ./ccam || Crash "could not find ./ccam/ folder"
for path in .$folder/*.ml ; do
  filename="$(basename "$path")"
  shortname="${filename%.ml}"
  ../comp "$path" "$shortname.c" && \
    make build file="$shortname.c" out="$shortname.out" && \
    mv "$shortname.out" ".$folder/$shortname.out" && \
    make build file="$shortname.c" out="$shortname.out" DBG=y && \
    mv "$shortname.out" ".$folder/$shortname.dbg.out" && \
    mv "$shortname.c" ".$folder/$shortname.c"
done

Done
