CC = gcc
CXX = g++
CCFLAGS = -O2 -pthread 
LIB = -lopencv_core -lopencv_highgui
objects = raspi.o ardrone.o navdata.o video.o board.o
target = raspi

$(target): $(objects)
	$(CC) $(CCFLAGS) $(LIB) -Wall -o $(target) $(objects)

raspi.o: raspi.c common/ardrone.h
	$(CC) $(CCFLAGS) $(LIB) -c raspi.c  

video.o: video/video.c video/video.h 
	$(CC) $(CCFLAGS) $(LIB) -c video/video.c

ardrone.o: common/ardrone.c common/ardrone.h
	$(CC) $(CCFLAGS) -c common/ardrone.c

navdata.o: navdata/navdata.c navdata/navdata.h
	$(CC) $(CCFLAGS) -c navdata/navdata.c 

board.o: board/board.c board/board.h
	$(CC) $(CCFLAGS) -c board/board.c 

clean: 
	-rm *.o $(target)

