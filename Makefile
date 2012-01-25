all: myfs
myfs: myfs.c functions.o
	cc -Wall -o myfs functions.o myfs.c -lm
functions.o: functions.c
	cc -Wall -c functions.c -lm