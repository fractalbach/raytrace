# Requires mingw-w64 to compile to a Windows executable
# sudo apt-get install mingw-w64
x86_64-w64-mingw32-gcc \
    src/main.cc \
    -o bin/main32.exe \
    -Wall
