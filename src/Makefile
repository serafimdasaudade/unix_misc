
#ARCHITECTURE=ARM

#ARCHITECTURE=PC32
ARCHITECTURE=PC64

####################################################################

CFLAGS+= -Wall -I../include #../include/FTDI 

SRCS_MISC= gpio.c log.c rs232.c scard.c signals.c socket.c timers.c fifo.c file.c thread.c mifare.c meters.c crc16.c
SRCS_CAN= can.c ftdi.c 
SRCS_MODBUS= acrel.c
OBJS_MISC:=$(patsubst %.c,%.o,$(SRCS_MISC))
OBJS_MODBUS:=$(patsubst %.c,%.o,$(SRCS_MODBUS))
OBJS_CAN:=$(patsubst %.c,%.o,$(SRCS_CAN))

LIBS= -lrt

#ALL=$(OBJS_MISC) $(OBJS_CAN) $(OBJS_MODBUS) modbus_libs.a can_libs.a libs.a
ALL=$(OBJS_MISC) $(OBJS_CAN) can_libs.a libs.a

all:	$(ALL)

libs.a:	$(OBJS_MISC)
	ar rcs libs.a $(OBJS_MISC) ; cp libs.a ../lib/
	
modbus_libs.a:	$(OBJS_MODBUS)
	ar rcs modbus_libs.a $(OBJS_MODBUS) ; cp modbus_libs.a ../lib/
	
can_libs.a:	$(OBJS_CAN)
	ar rcs can_libs.a $(OBJS_CAN) ; cp can_libs.a ../lib/
	
socket.o:		socket.c ../include/socket.h

signals.o:		signals.c ../include/signals.h

gpio.o:		gpio.c ../include/gpio.h

timers.o:		timers.c ../include/timers.h

socket_client.o:	socket_client.c ../include/socket.h

log.o:		log.c ../include/log.h

scard.o:		scard.c ../include/scard.h

mifare.o:		mifare.c

meters.o:		meters.c

rs232.o:	rs232.c ../include/rs232.h

can.o:	can.c ../include/can.h

fifo.o:	fifo.c ../include/fifo.h

file.o:	file.c ../include/file.h

acrel.o:	acrel.c ../include/acrel.h

crc16.o:	crc16.c ../include/crc16.h

clean:
	rm -f $(ALL)
