# Adjust the run address to match the .org in the source code
all: basic.mon

basic.mon: basic.bin
	bintoc -3 -b 24 -v -n -l 0XA000 basic.bin >basic.mon
	
basic.bin: basic.o
	ld65 -C none.cfg -vm -m basic.map -o basic.bin basic.o

basic.o: basic.asm neo6502_mon.asm neo6502.asm
	ca65 -g -l basic.lst --feature labels_without_colons -o basic.o neo6502_mon.asm

clean:
	$(RM) *.o *.lst *.map *.bin *.run *.mon *.c1p

distclean: clean
