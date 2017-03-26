
timeit.o: \
 timeit.c \
 timeit.h \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/err.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h
sieve.o: \
 sieve.c \
 timeit.h \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/assert.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/stdarg.h

