CC = g++
CFLAGS = -g 
LFLAGS = -ldedx `wx-config --libs` `wx-config --cxxflags` 
codefiles = main.cpp mainframe.cpp mainMenu.cpp dropdown/target.cpp dropdown/prog.cpp

out: main
	./main
main: $(codefiles)
	$(CC) $(CFLAGS) $(codefiles) -o main $(LFLAGS)

clean:
	rm -f *.o main
