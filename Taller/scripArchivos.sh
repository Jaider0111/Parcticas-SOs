#!/bin/bash
gcc -o consumidor consumidor.c -lm
gcc -o productor productor.c -lm
./consumidor &
./productor