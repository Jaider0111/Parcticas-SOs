bloques: consumidor productor

consumidor: consumidor.c
	sudo gcc -o consumidor consumidor.c -lm
	sudo ./consumidor &

productor: productor.c
	sudo gcc -o productor productor.c -lm
	sudo ./productor