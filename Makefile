CFLAGS = -O2 -Wall -I /opt/vc/include/ -I/opt/vc/include/interface/mmal/ -L/opt/vc/lib/ -lmmal_util -lmmal_core -lbcm_host -lmmal_vc_client -Wl,--whole-archive -lmmal_components -Wl,--no-whole-archive -lmmal_core -lpthread 
default: clean cam obj_rm

fs.o: fs.c SystemaCV.h
	gcc $(CFLAGS) -c fs.c -lm
servo.o: servo.c SystemaCV.h
	gcc $(CFLAGS) -c servo.c -lbcm2835
main.o: main.c SystemaCV.h
	gcc $(CFLAGS) -c main.c
ip.o : ip.c SystemaCV.h
	gcc $(CFLAGS) -c ip.c 
output.o: output.c SystemaCV.h 
	gcc $(CFLAGS) -c output.c -lSDL





cam: main.o output.o ip.o fs.o servo.o
	gcc  main.o output.o ip.o fs.o servo.o -o  cam $(CFLAGS) -lSDL -lm -lbcm2835
 




clean:
	$(RM) cam *.o NN/*.o

obj_rm:
	$(RM) *.o

