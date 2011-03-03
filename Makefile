CC = gcc -fPIC 
LDFLAGS = -lm

# set DEBUG options
ifdef DEBUG
CFLAGS = -Wall -Wextra -ggdb -pg -DDEBUG
else
CFLAGS = -Wall -O2
endif

#name all the object files
OBJS = main.o hashtable.o

all : hashtable

hashtable : $(OBJS)
	$(CC) $(LDFLAGS) -o main $^

debug :
	make all DEBUG=1

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $^

doxy :
	doxygen Doxyfile
	sh update_doc.sh	

clean :
	rm -rf $(OBJS) main

cs :
	cscope -bRv

cscope :
	cscope -bRv

