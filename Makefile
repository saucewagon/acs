.phony all:
all: ACS

ACS: ACS.c
	gcc ACS.c -lreadline -lhistory -o ACS

.PHONY clean:
clean:
	-rm -rf *.o *.exe
