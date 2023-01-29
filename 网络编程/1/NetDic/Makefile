# Makefile
#

#CROSS_COMPILE = arm-linux-gnu-

CC = $(CROSS_COMPILE)gcc

ifdef CROSS_COMPILE
TARGET = /opt/filesystem
endif

DEBUG = -g -O0 

CFLAGS += $(DEBUG)

PROGS = ${patsubst %.c, %, ${wildcard *.c}} 

all : $(PROGS)

%:%.c
	$(CC)  $(CFLAGS)  $< -o $@ -lpthread

install: $(PROGS)
ifdef CROSS_COMPILE
	mkdir $(TARGET)/root/long_term/ipc -p
	cp $(PROGS) $(TARGET)/root/long_term/ipc -f
endif


.PHONY: uninstall clean dist

uninstall :
ifdef CROSS_COMPILE
	cd $(TARGET)/root/long_term/ipc && rm -f $(PROGS)
endif

clean : uninstall
	@- $(RM) -f $(PROGS) core *.gz

	
