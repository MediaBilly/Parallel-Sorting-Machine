CC = gcc
FLAGS = -Wall
TARGETS = mysort coach sorter_quicksort sorter_heapsort
OBJS = coordinator.o coach.o sorter_quicksort.o sorter_heapsort.o minheap.o record.o

all:$(TARGETS)

mysort:coordinator.o record.o
	$(CC) $(FLAGS) -o mysort coordinator.o record.o

coach:coach.o record.o
	$(CC) $(FLAGS) -o coach coach.o record.o

sorter_quicksort:sorter_quicksort.o minheap.o record.o
	$(CC) $(FLAGS) -o sorter_quicksort sorter_quicksort.o minheap.o record.o

sorter_heapsort:sorter_heapsort.o minheap.o record.o
	$(CC) $(FLAGS) -o sorter_heapsort sorter_heapsort.o minheap.o record.o

coordinator.o:./src/coordinator.c
	$(CC) $(FLAGS) -o coordinator.o -c ./src/coordinator.c

coach.o:./src/coach.c
	$(CC) $(FLAGS) -o coach.o -c ./src/coach.c

sorter_quicksort.o:./src/sorter_quicksort.c
	$(CC) $(FLAGS) -o sorter_quicksort.o -c ./src/sorter_quicksort.c

sorter_heapsort.o:./src/sorter_heapsort.c
	$(CC) $(FLAGS) -o sorter_heapsort.o -c ./src/sorter_heapsort.c

minheap.o:./src/minheap.c
	$(CC) $(FLAGS) -o minheap.o -c ./src/minheap.c

record.o:./src/record.c
	$(CC) $(FLAGS) -o record.o -c ./src/record.c

.PHONY : clean

clean:
	rm -f $(TARGETS) $(OBJS)
