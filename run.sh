#!/usr/bin/env bash

# Remove old output if it exists
[ ! -e out/test.ppm ] || rm out/test.ppm && echo "removed: out/test.ppm"
[ ! -e out/test.png ] || rm out/test.png && echo "removed: out/teest.png"

# Compile c++ to linux executable
./scripts/build.sh      &&

# Run the program
./bin/main > out/test.ppm &&

# Convert the image output
echo "converting" &&
convert out/test.ppm out/test.png