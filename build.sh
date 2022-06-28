#!/bin/bash

clang -Ofast -flto -march=native ./bwts.c ./main.c -o glitch
