
.PHONEY: mkdirs makeforce

CFLAGSLOCAL += -Wall $(CFLAGSMORE) 


ALLX=

ALLX += xecho
xecho:  mkdirs xecho.c Makefile
	$(CC) $(CFLAGS) $(CFLAGSLOCAL) -o `../../makefile.sh --arch`/xecho xecho.c

include ../../Makefile.inc

#  used to make consolidated makefile when local make 
#  does not support include 
makefile: ../../makefile.sh ../../Makefile.inc
	../../makefile.sh --makefile
	
makeforce: ../../makefile.sh ../../Makefile.inc
	../../makefile.sh --makeforce

#end
