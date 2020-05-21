#
# Makefile for cc65
#
# Linux: - Asuming cc65 distribution was installed after make with:   sudo make install PREFIX=/usr
#        - you need to have then shared cc65 folder here:             /usr/share/cc65
# Win32: - This Makefile requires GNU make or mingw32-make
#        - Setup cc65 compiler path in (local) CC65_HOME variable below

# OS Type

ifeq "$(findstring Windows,$(OS))" "Windows"
	OS_TYPE = MINGW32_NT
	CC65_HOME = D:/cc65
else
	OS_TYPE = LINUX
	export CC65_HOME=/usr/share/cc65
endif


# Enter the target system here
SYS  = c64

CC65_LIB = $(CC65_HOME)/lib
CC65_TGI = $(CC65_HOME)/target/c64/drv/tgi
CC65_MOU = $(CC65_HOME)/target/c64/drv/mou

MOUS = $(CC65_MOU)/$(SYS)*.mou
TGI  = $(CC65_TGI)/$(SYS)*.tgi
CLIB = $(CC65_LIB)/$(SYS).lib

ifeq ($(OS_TYPE),MINGW32_NT)
# --------------------------------------------- Windows / local CC65_HOME variable need to point to cc65 folder
	CC  = $(CC65_HOME)/bin/cc65
	CL  = $(CC65_HOME)/bin/cl65
	AS  = $(CC65_HOME)/bin/ca65
	LD  = $(CC65_HOME)/bin/ld65
else
# --------------------------------------------- Linux   / binaries are assumed to be in common /usr/bin/
	CC  = cc65
	CL  = cl65
	AS  = ca65
	LD  = ld65
endif

# ----------------------------------------------------------------------------
# Generic rules

.c.o:
	@echo $<
	@$(CC) -o ./list/$(basename $<).s -Oirs -T --include-dir ./lib --codesize 100 --check-stack -g -t $(SYS) -I./ $<    
	@$(AS) -o ./obj/$(basename $<).o ./list/$(basename $<).s

.asm.o:
	@echo $<
	@$(AS) -o ./obj/$(basename $<).o $(basename $<).asm $<


# --------------------------------------------------------------------------
# Rules how to make each one of the binaries

OBJECTLIST = main.o

LINKLIST =	./obj/main.o \
			./lib/ultimate_lib_fix.o \
			./lib/dottedquad.o \
			./lib/dottedquad_c.o \
			$(CLIB)



.PHONY:	all   	

all:	$@ $(OBJECTLIST)
		@$(LD)  -t $(SYS) -m ./list/home64.map -Ln ./list/home64.lbl -o ./exe/home64.prg $(LINKLIST) 




# --------------------------------------------------------------------------


# --------------------------------------------------------------------------
# Cleanup rules

.PHONY:	clean
clean:
ifeq ($(OS_TYPE),MINGW32_NT)
	del .\exe\*.prg /s /q
	del .\obj\*.o /s /q
	del .\list\*.map /s /q
	del .\list\*.lbl /s /q
	del .\list\*.s /s /q
else
	rm -f ./obj/*.o
	rm -f ./exe/*.prg
	rm -f ./list/*.map
	rm -f ./list/*.lbl
	rm -f ./list/*.s
endif





