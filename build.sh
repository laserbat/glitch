#!/bin/bash

clang -Ofast -flto -march=native ./bwts.c ./mtf.c ./main.c -o glitch
