all:
	gcc -ggdb -Werror -pg -Wextra -std=gnu11 -Wno-unused-result -mglibc -Wuninitialized -march=x86-64 -Wpedantic -Wall -o cleiton_guilhermite_t1_b1_listaadjacencia_epidemia cleiton_guilhermite_t1_b1_listaadjacencia_epidemia.c

clean:
	rm -vf cleiton_guilhermite_t1_b1_listaadjacencia_epidemia cleiton_guilhermite_t1_b1_listaadjacencia_epidemia.1por1.saida.* cleiton_guilhermite_t1_b1_listaadjacencia_epidemia.maior.saida.*

rebuild: clean all
