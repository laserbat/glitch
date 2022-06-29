#!/bin/bash

clang -Ofast -flto -march=native ./libsais/libsais.o ./main.c -o glitch
