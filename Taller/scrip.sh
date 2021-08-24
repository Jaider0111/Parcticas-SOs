#!/bin/bash
echo -e "\n\nTransferencia usando tuberias sin nombre"
echo -e "\nTransferencia usando tuberias nombradas"
gcc mfifop.c -o mfifop -lpthread -lm
gcc mfifoc.c -o mfifoc -lm
sudo ./mfifoc &
sudo ./mfifop 
echo -e "Transferencia usando Archivos"
gcc -o consumidor consumidor.c -lm
gcc -o productor productor.c -lm
./consumidor &
./productor
echo -e "\nTransferencia usando sockets"
gcc socketp.c -o socketp -lpthread -lm
sudo ./socketp &
gcc socketc.c -o socketc -lm
sudo ./socketc
echo -e "\nTransferencia usando memoria compartida"
gcc ProductorMemoria.c -o ProductorMemoria -lpthread -lm
sudo ./ProductorMemoria &
gcc ConsumidorMemoria.c -o ConsumidorMemoria -lm
sudo ./ConsumidorMemoria