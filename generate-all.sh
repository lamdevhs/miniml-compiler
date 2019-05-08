#!/bin/bash

demo=demo
ocaml=ocaml
ccam=ccam
tmp=__tmp_folder_generate_all__

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
  echo rm ./$demo/*.c ./$demo/*.out
  echo press enter to delete those files
  read PAUSE
  rm ./$demo/*.c ./$demo/*.out
  Done
fi

[ -d ./$ocaml ] || Crash "could not find folder ./$ocaml"
[ -d ./$ccam ] || Crash "could not find folder ./$ccam"
[ -d ./$demo ] || Crash "could not find folder ./$demo"
! [ -e ./$tmp ] || Crash "could not create temporary folder ./$tmp"

cd $ocaml
make comp || Crash "could not build executable comp"
echo
cd ../
cp -r ./$ccam ./$tmp || Crash "could not create temporary folder ./$tmp"
cp ./$ocaml/comp ./$tmp
cd ./$tmp
for path in ../$demo/*.ml ; do
  filename="$(basename "$path")"
  shortname="${filename%.ml}"
  echo "### compiling $filename"
  ./comp "$path" "$shortname.c" && \
    make build in="$shortname.c" out="$shortname.out" && \
    mv "$shortname.out" "../$demo/$shortname.out" && \
    make build in="$shortname.c" out="$shortname.out" DBG=y && \
    mv "$shortname.out" "../$demo/$shortname.dbg.out" && \
    mv "$shortname.c" "../$demo/$shortname.c"
  echo
done

cd ../
rm -r ./$tmp || Msg "warning: could not erase temporary folder ./$tmp"
Done
