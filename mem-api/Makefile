all: null forget-to-free data alloc-then-print vector

clean:
		rm -f null forget-to-free data alloc-then-print

null: null.c
		gcc -o null null.c -Wall

forget-to-free: forget-to-free.c
		gcc -o forget-to-free forget-to-free.c -Wall

data: data.c
		gcc -o data data.c -Wall

alloc-then-print: alloc-then-print.c
		gcc -o alloc-then-print alloc-then-print.c -Wall

vector: vector.c
		gcc -o vector vector.c -Wall