bloques: servidor cliente

servidor: tabla_hash data_blocks p2-server.c
	@gcc p2-server.c -o p2-server -lpthread

cliente: p2-client.c
	@gcc p2-client.c -o p2-client