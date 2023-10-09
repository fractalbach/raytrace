#!/usr/bin/env bash

set -e

echo "===[ BUILD ]==="
time -p ./scripts/build.sh
echo "done building"

echo "===[ ULTRA SPECIAL RUN BEGINS ]==="
for value in {1..61..1}
do
    echo "===[ FILE ${value} ]==="

    ./bin/main \
        --frame ${value} \
        > out/second/${value}.ppm

    convert \
        out/second/${value}.ppm \
        out/second/png/${value}.png

done
echo "done running"


# ffmpeg -r 24 -i 'out/second/png/%d.png' out/second/output.gif


# cores_to_use=$(expr $(nproc) - 1)
# echo "cores to use: ${cores_to_use}"