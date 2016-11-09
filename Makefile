CC = g++ 


AltunSynthesis: 
	$(CC) -g AltunSinthesis.cc -o  AltunSinthesis

# functions.o : functions.cc
# 	$(CC) -c   functions.cc -lemon

# AltunSynthesis.o: AltunSynthesis.cc
# 	$(CC) -c  AltunSynthesis.cc -lemon	

clean:
	@rm -f *~
	@rm -f *.o
