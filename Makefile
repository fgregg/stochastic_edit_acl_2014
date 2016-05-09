CC= g++ 


OBJS=	csv_parser.o data-io.o wfst-train.o wfst-train-local-fast.o 
MAIN = sandbox
INC= -L/export/apps/lib/ -ldl -lfst -lpthread -Ilib/dlib-18.5   -I/export/apps/include -Isrc -g 




all:    $(OBJS) src/$(MAIN).cpp
	 $(CC) $(INC)  $(OBJS)  -o $(MAIN) src/$(MAIN).cpp
		strip $(MAIN)	

data-io.o:
	$(CC) $(INC) -c src/data-io.cpp -o data-io.o

wfst-train.o: data-io.o
	$(CC) $(INC) -c src/wfst-train.cpp -o wfst-train.o

wfst-train-local-fast.o: wfst-train.o
	$(CC) $(INC) -c src/wfst-train-local-fast.cpp -o wfst-train-local-fast.o

csv_parser.o: 
	$(CC) $(INC) -c src/csv_parser.cpp -o csv_parser.o


test:

clean:
	rm $(OBJS) 
	rm $(MAIN)

