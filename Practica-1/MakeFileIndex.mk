indexado: bucador interfaz

buscador: search_indexing.c tabla_hash_ind data_indexing
	@sudo gcc -o search_indexing search_indexing.c
	@sudo ./search_indexing &

interfaz: main.c
	@sudo gcc -o main main.c
	@sudo ./main