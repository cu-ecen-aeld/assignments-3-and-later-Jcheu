# If CROSS_COMPILE is defined, use cross compiler, else native gcc
ifdef CROSS_COMPILE
	compileOption=$(CROSS_COMPILE)gcc
else
	compileOption=gcc
endif

all: writer

writer:
	$(compileOption) writer.c -o writer
	
clean:
	rm -f writer
	rm -f writer.o
