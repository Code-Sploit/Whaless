#!/usr/bin/python3

CFLAGS='-c -O3 -std=c18 -Wall -I. '

csources = []

import fnmatch
import os

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

os.system("gcc -o " + total + " -lSDL2 -lSDL2_image -lSDL2_ttf")

print("Done!\nRunning Whaless, the best chess engine lol")

os.system("./whaless")