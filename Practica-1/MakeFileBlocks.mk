bloques: buscador interfaz

buscador: search.c tabla_hash data_blocks
	@sudo gcc -o search search.c
	@sudo ./search &

interfaz: main.c
	@sudo gcc -o main main.c
	@sudo ./main