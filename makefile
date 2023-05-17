GCCFLAGS=-Wall -Zomf -Zsys
MSCFLAGS=-W3 -AL

.c.ob2:
        gcc -c $(GCCFLAGS) -o $@ $*.c

.c.obj:
        cl -c $(MSCFLAGS) -Fo$@ $*.c


ipf2txt.exe: unipfc.ob2 printtxt.ob2 ipf2txt.def ipf2txtd.exe
        gcc $(GCCFLAGS) -o $@ unipfc.ob2 printtxt.ob2 ipf2txt.def

ipf2txtd.exe: unipfc.obj printtxt.obj
        cl $(CCFLAGS) -Fe$@ unipfc.obj printtxt.obj

unipfc.obj: unipfc.c ipf.h

unipfc.ob2: unipfc.c ipf.h

printtxt.obj: printtxt.c ipf.h

printtxt.ob2: printtxt.c ipf.h
