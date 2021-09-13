IDIR=include
CC=cc
CFLAGS=-I$(IDIR) -std=c11 -g -fno-common `pkg-config --cflags libjit`
LDFLAGS=`pkg-config --libs libjit`

ODIR=obj
BDIR=bin

_DEPS = bee.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

bee: $(OBJ)
	mkdir -p $(BDIR)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LDFLAGS)

test: bee
	./tests/runall.sh "$(BDIR)/bee"

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
