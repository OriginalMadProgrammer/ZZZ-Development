
# SCOPE: rc.user ZZZ

all: allx

.PHONY: clean mkdirs all_made all allx mkdirs install make_test

clean:  
	@set -x; rm -rf $(ALLX) `../../makefile.sh --arch`

realclean: clean
	rm -f makefile

mkdirs:
	../../makefile.sh --mkdirs

install: mkdirs $(ALLX)
	set -x; \
	arch="`../../makefile.sh --arch`" && \
	for f in $(ALLX); do \
	    echo "make $$f"; \
	    cp -p "$$arch/$$f" "../../bin/$$arch/." || exit $$?; \
	 done

# phoney target used by makefile.sh to test if make supports include
make_test:
	true

allx: $(ALLX) Makefile ../../Makefile.inc

all_made:	
	@#  targets to be made
	@echo $(ALLX);

