shell: shellex.c csapp.c csapp.h JobControl.c giveme.c signalH.c JobControl.h
	gcc -Wall -o ../install/bin/TigerShell -pthread shellex.c csapp.c JobControl.c
	gcc -o ../install/bin/giveme giveme.c
	gcc -o ../install/bin/signalH signalH.c

giveme: giveme.c
	gcc -o ../install/bin/giveme giveme.c

signalH: signalH.c
	gcc -o ../install/bin/signalH signalH.c

clean:
	rm -f *.o
	rm ../install/bin/TigerShell
