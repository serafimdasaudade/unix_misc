
#ARCHITECTURE=ARM

#ARCHITECTURE=PC32
ARCHITECTURE=PC64

####################################################################

CFLAGS+= -Wall -Wextra -pedantic -I../include

SRCS_MISC= gpio.c log.c rs232.c signals.c socket.c timers.c fifo.c file.c thread.c i2c.c crc16.c 
SRCS_CAN= can.c ftdi.c 
OBJS_MISC:=$(patsubst %.c,%.o,$(SRCS_MISC))
OBJS_CAN:=$(patsubst %.c,%.o,$(SRCS_CAN))

LIBS= -lrt

ALL=$(OBJS_MISC) $(OBJS_CAN) can_libs.a libs.a

all:	$(ALL)

libs.a:	$(OBJS_MISC)
	ar rcs libs.a $(OBJS_MISC) ; cp libs.a ../lib/
	
can_libs.a:	$(OBJS_CAN)
	ar rcs can_libs.a $(OBJS_CAN) ; cp can_libs.a ../lib/
	
socket.o:		socket.c ../include/socket.h

signal.o:		signals.c ../include/signals.h

gpio.o:		gpio.c ../include/gpio.h

timers.o:		timers.c ../include/timers.h

socket_client.o:	socket_client.c ../include/socket.h

log.o:		log.c ../include/log.h

rs232.o:		rs232.c ../include/rs232.h

can.o:		can.c ../include/can.h

fifo.o:		fifo.c ../include/fifo.h

file.o:		file.c ../include/file.h

i2c.o:		i2c.c ../include/i2c.h

crc16.o:	crc16.c ../include/crc16.h

clean:
	rm -f $(ALL)
