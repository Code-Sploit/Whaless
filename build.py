#!/usr/bin/python3

CFLAGS='-c -O3 -Wall -std=c11 -I include '

csources = []

import fnmatch
import os

os.system("rm -rf build")

for root, dirnames, filenames in os.walk('.'):
    for filename in fnmatch.filter(filenames, '*.c'):
            csources.append(os.path.join(root, filename))

for i in range(len(csources)):
    c = csources[i]
    os.system("mkdir -p build/" + c)
    print("[" + str(i + 1) + "/" + str(len(csources)) + "] " + "Compiling: [" + c + "]")
    os.system("gcc " + CFLAGS + c + " -o build/" + c + ".o")

total = ""

os.system("mkdir -p build")

for c in csources:
    total += "build/"
    total += c
    total += ".o "

print("\nLinking Whaless...\n")

os.system("gcc -o whaless " + total + " -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread")

print("Done!\nRunning Whaless, the best chess engine lol")

os.system("./whaless")
