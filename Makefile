all: bigextract
	gcc -o bigextract bigextract.o

bigextract.o:
	gcc -c -o bigextract.o bigextract.c

clean:
	@rm -rf bigextract{,.o}
