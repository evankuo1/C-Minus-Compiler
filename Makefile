.PHONY: compile
compile:
	gcc -Wall -c program5.c -o program5.o
	gcc -Wall -c driver.c -o driver.o
	gcc -Wall -c ast-print.c -o ast-print.o
	gcc program5.o driver.o ast-print.o -o compile

.PHONY: clean
clean:
	rm -f *.o
	rm -f compile