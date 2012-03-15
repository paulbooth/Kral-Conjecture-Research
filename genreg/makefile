CC = cc -D__UNIX__ 
OPTS = -c -O
STDH = gendef.h
OBJS = genreg.o main.o optmid.o

genreg : $(OBJS)
	  cc -o $@ $(OBJS) $(LIBS)

genreg.o : genreg.c $(STDH)
	    $(CC) $(OPTS) genreg.c

main.o : main.c $(STDH)
	  $(CC) $(OPTS) main.c

optmid.o : optmid.c $(STDH)
	    $(CC) $(OPTS) optmid.c
