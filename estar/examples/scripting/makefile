CC= gcc
CFLAGS= -g
LFLAGS = -Wall -ldedx
SOURCE = main.c invoke_func.c


main: main.c invoke_func.c
	$(CC) $(CFLAGS) $(SOURCE) -o main $(LFLAGS)
test: main
	./main libConf	
clean:
	\rm main libdedx_out lib.temp
