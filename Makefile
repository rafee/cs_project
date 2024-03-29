# Name: Makefile
# Project: Remote Sensor
# Author: Christian Starkjohann
# Creation Date: 2005-03-20
# Tabsize: 4
# Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
# This Revision: $Id$

CC              = gcc
LIBUSB_CONFIG   = libusb-config
OPENCV_LIBS	= pkg-config --libs opencv
OPENCV_COMPILE	= pkg-config --cflags opencv
# Make sure that libusb-config is in the search path or specify a full path.
# On Windows, there is no libusb-config and you must configure the options
# below manually. See examples.
CFLAGS          = `$(LIBUSB_CONFIG) --cflags` `$(OPENCV_COMPILE)` -O -Wall
#CFLAGS          = -I/usr/local/libusb/include
# On Windows replace `$(LIBUSB_CONFIG) --cflags` with appropriate "-I..."
# option to ensure that usb.h is found
LIBS            = `$(LIBUSB_CONFIG) --libs` `$(OPENCV_LIBS)` -lm
#LIBS            = `$(LIBUSB_CONFIG) --libs` -framework CoreFoundation
# You may need "-framework CoreFoundation" on Mac OS X and Darwin.
#LIBS            = -L/usr/local/libusb/lib/gcc -lusb
# On Windows use somthing similar to the line above.
EXE_SUFFIX=
# On Windows, you probably want to use .exe as suffix

PROGRAM=usbtest$(EXE_SUFFIX)

all: $(PROGRAM)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROGRAM): usbtest.o
	$(CC) -o $(PROGRAM) usbtest.o $(LIBS)

strip: $(PROGRAM)
	strip $(PROGRAM)

clean:
	rm -f *.o
	rm -f usbtest usbtest.exe
