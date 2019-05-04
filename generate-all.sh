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
  filename=$(basename "$path")
  ../comp "$path" "$filename" && \
    make build name="$filename" && \
    mv "$filename".out "$path".out && \
    make build name="$filename" DBG=y && \
    mv "$filename".out "$path".dbg.out && \
    mv "$filename".c "$path".c
done
Done
