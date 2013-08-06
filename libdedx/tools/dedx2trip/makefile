CFLAGS  += -Wall -std=c99 -fopenmp -g#`gsl-config --cflags`
LDFLAGS += -lgomp #`gsl-config --libs`
obj = dedx2trip_new.o range.o adapt.o
obj2 = dedx4trip.o range.o adapt.o

main: $(obj)
	gcc $(CFLAGS) $(obj) -o dedx2trip_new -lm -ldedx

all:
	main
	
exec:
	./dedx2trip
	cat ./output/report.txt
	
clean:
	rm -f $(obj)  report.txt core*
	clear
	
plot:	
	cp ./output/trip_out.dedx ./testing/trip_out.dedx
	./testing/testprocedure
	clear

	
clean-plot:
	rm -f ./testing/plot.plt ./testing/plot.pdf
		
