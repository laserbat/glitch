#!/bin/bash

cd libsais
clang -Ofast -flto -march=native -c ./src/libsais.c -o ./libsais.o
