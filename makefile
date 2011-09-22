
COPT =  -Wall -Wno-sign-compare -O3 -fPIC -fexceptions # flags #-DNDEBUG
CC    = g++


biflow_solve: biflow_solve.o
	$(CC) biflow_solve.o -o biflow_solve
biflow_solve.o : biflow_solve.cpp
	$(CC) -c $(COPT) biflow_solve.cpp -o biflow_solve.o

# Target deleting unwanted files
clean:
	rm -f *.o *~ biflow_solve 
