CC = clang++
CFLAGS = -Wno-deprecated-declarations -Wno-shift-op-parentheses -Xpreprocessor -fopenmp -O3
INCFLAGS = -I./glm-0.9.7.1 -I/usr/local/opt/llvm/include -I./include/
LDFLAGS = -L./lib/mac/ -L/usr/local/opt/llvm/lib -lm -lfreeimage -lomp

RM = /bin/rm -f 
all: transforms
transforms: main.o Transform.o readfile.o variable.h readfile.h Transform.h
	$(CC) $(CFLAGS) -o transforms main.o Transform.o readfile.o $(INCFLAGS) $(LDFLAGS) 
main.o: main.cpp Transform.h variable.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
readfile.o: readfile.cpp readfile.h variable.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c readfile.cpp
Transform.o: Transform.cpp Transform.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c Transform.cpp  
clean: 
	$(RM) *.o transforms *.png


 
