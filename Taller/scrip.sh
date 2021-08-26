#!/bin/bash
echo -e "Transferencia usando tuberias nombradas"
gcc mfifop.c -o mfifop -lpthread -lm
gcc mfifoc.c -o mfifoc -lm
sudo ./mfifoc &
sudo ./mfifop 
echo -e "\n\nTransferencia usando tuberias sin nombre"
gcc pipes.c -o pipes -lm
sudo ./pipes
echo -e "\n\nTransferencia usando Archivos"
gcc -o consumidor consumidor.c -lm
gcc -o productor productor.c -lm
sudo ./consumidor &
sudo ./productor
echo -e "\n\nTransferencia usando sockets"
gcc socketp.c -o socketp -lpthread -lm
sudo ./socketp &
gcc socketc.c -o socketc -lm
sudo ./socketc
echo -e "\n\nTransferencia usando memoria compartida"
gcc -o ConsumidorMemoria ConsumidorMemoria.c -lm
gcc -o ProductorMemoria ProductorMemoria.c -lm
sudo ./ConsumidorMemoria &
sudo ./ProductorMemoria