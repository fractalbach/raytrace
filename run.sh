#!/usr/bin/env bash

# Compile c++ to linux executable
g++ \
    src/main.cc \
    -Wall \
    -o bin/main

# Run the program
./bin/main > out/test.ppm

# Convert the image output
convert out/test.ppm out/test.png