gcc mfifop.c -o mfifop -lpthread -lm
gcc mfifoc.c -o mfifoc -lm
sudo ./mfifoc &
sudo ./mfifop 