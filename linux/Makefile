# Project: OBD Monitor
# Author:  Derek Chadwick
# Date:    01/12/2017


# Compiler flags

CC=gcc
CFLAGS=-Wall

# Linker flags

LDFLAGS=-static
LIBS=-lm
LIBDIRS=-L../../libs

# Sources

GUI_SOURCES=exbc_gui.c protocols.c sockets.c gui_dialogs.c gui_gauges.c log.c util.c gui_gauges_aux.c
UNIT_TEST_SOURCES=unit_test.c util.c log.c rs232.c
SERIAL_TEST_SOURCES=test_serial_rxtx.c rs232.c
PULSE_TEST_SOURCES=test_pulse.c rs232.c

# Objects

GUI_EXECUTABLE=obd_gui
SIMULATOR_EXECUTABLE=ecu_sim
UNIT_TEST_EXECUTABLE=unit_test
SERIAL_TEST_EXECUTABLE=serial_test
PULSE_TEST_EXECUTABLE=pulse_test

# Includes

INCPREFIX=../../libs/
INCLUDES=-I$(INCPREFIX)/include

# Target Rules

all: $(GUI_SOURCES) $(SERVER_SOURCES)

gui: exbc_gui.c protocols.h
	$(CC) $(GUI_SOURCES) $(LIBS) `pkg-config --libs --cflags gtk+-3.0` -o $(GUI_EXECUTABLE)

utests: unit_test.c
	$(CC) $(CFLAGS) $(UNIT_TEST_SOURCES) -o $(UNIT_TEST_EXECUTABLE)
	
ftests: test_server.c
	$(CC) $(CFLAGS) $(FUNCTION_TEST_SOURCES) -o $(FUNCTION_TEST_EXECUTABLE)
	
stests: test_serial_rxtx.c
	$(CC) $(CFLAGS) $(SERIAL_TEST_SOURCES) -o $(SERIAL_TEST_EXECUTABLE)
	
ptests: pulse_test.c
	$(CC) $(CFLAGS) $(PULSE_TEST_SOURCES) -o $(PULSE_TEST_EXECUTABLE)
	
strip:
	strip $(GUI_EXECUTABLE)

clean:
	rm $(GUI_EXECUTABLE) $(PULSE_TEST_EXECUTABLE) $(UNIT_TEST_EXECUTABLE) $(SERIAL_TEST_EXECUTABLE)
	
	
