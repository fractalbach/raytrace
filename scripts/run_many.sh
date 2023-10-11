#!/usr/bin/env bash

set -e

echo "===[ BUILD ]==="
time -p ./scripts/build.sh
echo "done building"


echo "===[       IT BEGINS...        ]==="
echo "===[ THE WEAVING OF THE FRAMES ]==="

corez=$(expr $(nproc) - 1)

letsgo() {
    echo "===[ FILE ${1} ]==="

    ./bin/main \
        --frame ${1} \
        > "out/third/${1}.ppm"

    convert \
        "out/third/${1}.ppm" \
        "out/third/${1}.png"
}

export -f letsgo

parallel -j${corez} letsgo ::: $(seq 60)

ffmpeg -r 24 -i 'out/third/%d.png' "out/third/output.gif"

