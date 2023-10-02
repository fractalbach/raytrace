# Requires mingw-w64 to compile to a Windows executable
# sudo apt-get install mingw-w64
x86_64-w64-mingw32-g++ \
    src/main.cc \
    -o bin/a.exe \
    -g \
    -Wall
