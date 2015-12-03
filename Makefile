CC = gcc
CCFLAGS = -pthread
objects = raspi.o ardrone.o navdata.o
target = raspi

$(target): $(objects)
	$(CC) $(CCFLAGS) -o $(target) $(objects)

raspi.o: raspi.c common/ardrone.h
	$(CC) $(CCFLAGS) -c raspi.c

ardrone.o: common/ardrone.c common/ardrone.h
	$(CC) $(CCFLAGS) -c common/ardrone.c

navdata.o: navdata/navdata.c navdata/navdata.h navdata/navdata_common.h navdata/navdata_keys.h
	$(CC) $(CCFLAGS) -c navdata/navdata.c
 
clean: 
	-rm *.o $(target)

