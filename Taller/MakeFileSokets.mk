bloques: servidor cliente

servidor: socketp.c
	@gcc socketp.c -o socketp -lpthread
	@sudo ./socketp &

cliente: socketc.c
	@gcc socketc.c -o socketc
	@sudo ./socketc