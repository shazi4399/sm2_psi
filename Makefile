
BIN=bin
SRC=src


# compiler settings
CC=g++
#COMPILER_OPTIONS=-O2
COMPILER_OPTIONS=-g3 -O2 #-shared -fPIC #-mavx -maes -mpclmul -DRDTSC -DTEST=AES128

DEBUG_OPTIONS=-g3 -ggdb #-Wall -Wextra 

BATCH=

ARCHITECTURE = $(shell uname -m)
ifeq (${ARCHITECTURE},x86_64)
GNU_LIB_PATH:=x86_64
else
GNU_LIB_PATH:=i386
endif

INCLUDE=-I..  -I/usr/include/glib-2.0/ -I/usr/lib/${GNU_LIB_PATH}-linux-gnu/glib-2.0/include `pkg-config --cflags glib-2.0`


LIBRARIES=-lgmp -lgmpxx -lpthread  -L /usr/lib  -lssl -lippcp -lcrypto -lglib-2.0 `pkg-config --libs glib-2.0`
CFLAGS=

# all source files and corresponding object files 
SOURCES_CORE := $(shell find ${CORE} -type f -name '*.cpp'  -a -not -path '*/mains/*')
OBJECTS_CORE := $(SOURCES_CORE:.cpp=.o)
# directory for PSI related sources
SOURCES_UTIL=${SRC}/util/*.cpp
OBJECTS_UTIL=${SRC}/util/*.o
SOURCE_BIGN=${SRC}/util/bigNum/*.cpp
OBJECTS_BIGN=${SRC}/util/bigNum/*.o
SOURCE_CRYPT=${SRC}/util/gmCrypto/*.cpp
OBJECTS_CRYPT=${SRC}/util/gmCrypto/*.o

# public-key-based PSI
SOURCES_DHPSI=${SRC}/ecdh-psi/*.cpp
OBJECTS_DHPSI=${SRC}/ecdh-psi/*.o

all: core demo lib
	@echo "make all done."

core: ${OBJECTS_CORE}

%.o:%.cpp %.h
	${CC} $< ${COMPILER_OPTIONS} ${DEBUG_OPTIONS} -c ${INCLUDE} ${LIBRARIES} ${CFLAGS} ${BATCH} -o $@

demo:  
	${CC} -o psi.out ${SRC}/mains/psi_demo.cpp ${OBJECTS_DHPSI} ${OBJECTS_UTIL} ${CFLAGS} ${DEBUG_OPTIONS} ${LIBRARIES} ${OBJECTS_BIGN} ${OBJECTS_CRYPT} ${INCLUDE} ${COMPILER_OPTIONS}
lib:
	#${CC} -o libpsi.so ${OBJECTS_DHPSI} ${OBJECTS_UTIL} ${CFLAGS} ${DEBUG_OPTIONS} ${LIBRARIES} ${OBJECTS_BIGN} ${OBJECTS_CRYPT} ${INCLUDE} ${COMPILER_OPTIONS}

# only clean example objects, test object and binaries
clean:
	rm -f *.so *.a ${OBJECTS_EXAMPLE} ${OBJECTS_TEST} *.exe ${OBJECTS_DHPSI} ${OBJECTS_UTIL} ${OBJECTS_CRYPT} ${OBJECTS_BIGN}
