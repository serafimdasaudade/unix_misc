CFLAGS=-I../include 
LIBS_PCSC=-lpcsclite
#LIBS=../lib/mc_libs.a  -lrt  /usr/lib/libftdi.so.1
LIBS=../lib/libs.a  -pthread -lrt -lm `pkg-config --cflags --libs libmodbus`

ALL=trs232 gpiocltest gpiotest test_log cli serv timer_test acrel_test rs232_modbus
#ALL:	gpiocltest timer_test test_mifare_reader test_mifare_reader_change_pin test_mifare_reader_write_data trs232 read_card_atrib
all:	trs232 gpiocltest gpiotest test_log cli serv timer_test acrel_test rs232_modbus

trs232:	trs232.o 
	cc -I/usr/include/mysql/ trs232.o $(LIBS) -o trs232 
	
gpiocltest:	gpiocltest.o
	cc gpiocltest.o $(LIBS) -o gpiocltest
	
gpiotest:	gpiotest.o
	cc gpiotest.o $(LIBS) -o gpiotest
	
meters_test:	meters_test.o 
	cc meters_test.o ../lib/libs.a -lrt -lm -o meters_test

acrel_test: 
	cc acrel_test.c $(CFLAGS) $(LIBS) -o acrel_test

rs232_modbus:
	cc rs232_modbus.c $(CFLAGS) $(LIBS) -o rs232_modbus
	
test_log:	test_log.o 
	cc test_log.o $(LIBS) -o test_log

cli:	socket_client.o 
	cc socket_client.o $(LIBS) -o cli

test_mifare_reader:	test_mifare_reader.o 
	cc test_mifare_reader.o $(LIBS) $(LIBS_PCSC) -o test_mifare_reader

test_mifare_reader_change_pin:	test_mifare_reader_change_pin.o 
	cc test_mifare_reader_change_pin.o $(LIBS) $(LIBS_PCSC) -o test_mifare_reader_change_pin

test_mifare_reader_write_data:	test_mifare_reader_write_data.o
	cc test_mifare_reader_write_data.o $(LIBS) $(LIBS_PCSC) -o test_mifare_reader_write_data

read_card_atrib:	read_card_atrib.o 
	cc read_card_atrib.o $(LIBS) -o read_card_atrib

serv:	socket_server.o 
	cc socket_server.o $(LIBS) -o serv

timer_test:	timer_test.o 
	cc $< $(LIBS) -o timer_test

cantest:	cantest.o
	cc cantest.o $(LIBS) -o cantest

clean:
	rm -f $(ALL) *.o
