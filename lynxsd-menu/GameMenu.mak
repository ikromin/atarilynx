#http://msdn.microsoft.com/en-us/library/dd9y37ha(v=VS.80).aspx

!INCLUDE <lynxcc65.mak>

target = menu.bin
objects = lynx-160-102-16.o lynx-stdjoy.o GameMenu.o LynxSD.o icon.o

all: $(target)

$(target) : $(objects)
	$(CL) -t $(SYS) -o $@ $(objects) lynx.lib -m menu.map
	
clean:
  $(RM) *.tgi
  $(RM) *.s
  $(RM) *.joy
  $(RM) *.o
  $(RM) *.lnx
  $(RM) *.bin
  $(RM) *.spr
  $(RM) *.pal
  $(RM) *.map