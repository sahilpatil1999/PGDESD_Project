node: node.c
	gcc node.c -lpthread
run:
	./a.out        
clean:
	rm a.out
