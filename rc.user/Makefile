
# SCOPE: rc.user ZZZZ

.PHONEY:  clean  rcinit  all install1 install2 install tar makefile

rcinit:
	chmod a+rx,go-w makefile.sh


clean:
	./makefile.sh clean

all:
	./makefile.sh all

install1:
	./install.sh 

install2:
	./makefile.sh install

install:  install1 install2

tar:
	./makefile.sh --tar


gmake_test:
	true


# debugging not used in the normal case as each directory makes its own.
deep_makefiles:
	set -x; \
	for mf in */src/Makefile; do \
	    ( cd "${mf#Makefile}" && make -f ../../Makefile.inc makefile ); \
	done

#end
