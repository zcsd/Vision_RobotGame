#objects includes all the *.o files requires to make the project
objects = Vision.o Main.o Uart.o File.o

CFLAGS = `pkg-config --cflags opencv`
LIBS= `pkg-config --libs opencv`

#"VisionSystem - refers to the filename of the executable
VisionSystem : $(objects)
	gcc $(objects) -o VisionSystem $(LIBS)$(CFLAGS) -lpthread
	sync

#define how the *.O are created, i.e abc.O : (*.c file use to form the object) (all *.h file linking it)     
Main.o: Main.c Vision.h Uart.h File.h 
	gcc  Main.c -c $(LIBS)$(CFLAGS)

Vision.o: Vision.c Vision.h
	
	gcc $(LIBS)$(CFLAGS)  -c Vision.c
	
Uart.o: Uart.c Uart.h
	cc -c Uart.c
	
File.o: File.c File.h
	gcc $(LIBS)$(CFLAGS)  -c File.c	
     
clean :
	rm VisionSystem $(objects)
