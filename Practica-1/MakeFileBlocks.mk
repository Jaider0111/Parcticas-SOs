bloques: buscador interfaz

buscador: tabla_hash data_blocks search.c
	@sudo gcc -o search search.c
	@sudo ./search &

interfaz: main.c
	@sudo gcc -o main main.c
	@sudo ./main