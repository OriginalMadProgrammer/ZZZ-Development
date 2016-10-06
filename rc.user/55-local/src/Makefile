
ALLX=

ALLX += ddump
ddump:  mkdirs ddump.c
	$(CC) -o `../../makefile.sh --arch`/ddump ddump.c

.PHONEY: makeforce

include ../../Makefile.inc

#  used to make consolidated makefile when local make 
#  does not support include 
makefile: ../../makefile.sh ../../Makefile.inc
	../../makefile.sh --makefile
	
makeforce: ../../makefile.sh ../../Makefile.inc
	../../makefile.sh --makeforce

#end

