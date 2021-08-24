#!/bin/bash
gcc socketp.c -o socketp -lpthread
sudo ./socketp &
gcc socketc.c -o socketc
sudo ./socketc