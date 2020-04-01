SOURCE_DIR=src
BUILD_DIR=build
BIN = lynxvirus2020.lnx

CC65=$(CC65_HOME)
CC65_BIN=$(CC65)/bin
CC65_INC=$(CC65)/include
CC65_ASMINC=$(CC65)/asminc
CC65_TOOLS=$(CC65_BIN)

# Tool executables
CO=co65
CC=cc65
AS=ca65
AR=ar65
CL=cl65
SPRPCK=sprpck
CP=cp
RM=rm -f
ECHO=echo
TOUCH=touch

# Tool flags
SYS=lynx
TGI=$(SYS)-160-102-16
SEGMENTS=--code-name CODE --rodata-name RODATA --bss-name BSS --data-name DATA
ASFLAGS=-t $(SYS) -I $(CC65_ASMINC)
CFLAGS=$(SEGMENTS) --all-cdecl -I . -t $(SYS) -Cl -Or -O
LDFLAGS=-t $(SYS) $(SYS).lib
SPRFLAGS=-t6 -p1

.SUFFIXES:
.PHONY: clean mkbuild all
VPATH = src/asm src/c src/images

LIB_OBJECTS = $(BUILD_DIR)/$(TGI).o

# == Copy and convert TGI to assembly to TGI object
$(BUILD_DIR)/$(TGI).o: $(BUILD_DIR)/$(TGI).s
$(BUILD_DIR)/$(TGI).s:
	@$(ECHO) == Converting TGI library
	@$(CP) $(CC65_TGT)/$(SYS)/drv/tgi/$(TGI).tgi $(BUILD_DIR)
	@$(CO) --code-label _$(SYS)tgi $(BUILD_DIR)/$(TGI).tgi
	@$(AS) $(ASFLAGS) $(BUILD_DIR)/$(TGI).s

# == Assembly sources to assembly objects
ASM_SOURCES = $(wildcard $(SOURCE_DIR)/asm/*.s)
ASM_OBJECTS = $(patsubst $(SOURCE_DIR)/asm/%.s, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
$(BUILD_DIR)/%.o: %.s
	@$(ECHO) == Assembling $< =\> $@
	@$(AS) $(ASFLAGS) $< -o $@

# == C sources to assembly intermediate files to C objects
C_SOURCES = $(wildcard $(SOURCE_DIR)/c/*.c)
C_OBJECTS = $(patsubst $(SOURCE_DIR)/c/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
$(BUILD_DIR)/%.o: %.c
	@$(ECHO) == Compiling $< =\> $@
	@$(CC) $(CFLAGS) $< -o $(BUILD_DIR)/$*.s
	@$(AS) $(ASFLAGS) $(BUILD_DIR)/$*.s -o $@

# == Bitmap files to intermediate assembly to bitmap objects
BMP_SOURCES = $(wildcard $(SOURCE_DIR)/images/*.bmp)
BMP_OBJECTS = $(patsubst $(SOURCE_DIR)/images/%.bmp, $(BUILD_DIR)/%.o, $(BMP_SOURCES))
$(BUILD_DIR)/%.o: %.bmp
	@$(ECHO) == Converting bitmap $< to $@
	@$(SPRPCK) $(SPRFLAGS) $< $@
	@$(ECHO) .global _$* > $(BUILD_DIR)/$*.s
	@$(ECHO) .segment \"RODATA\" >> $(BUILD_DIR)/$*.s
	@$(ECHO) _$*: .incbin \"$*.spr\" >> $(BUILD_DIR)/$*.s
	@$(AS) $(ASFLAGS) $(BUILD_DIR)/$*.s -o $@

# == Link all objects to final binary
$(BIN): $(LIB_OBJECTS) $(ASM_OBJECTS) $(BMP_OBJECTS) $(C_OBJECTS)
	@$(ECHO) == Linking binary executable to $@
	@$(CL) $(LDFLAGS) $^ -o $@

mkbuild:
	@mkdir -p build
	
all: mkbuild $(BIN)

clean:
	@$(ECHO) == Cleaning up
	@$(RM) -rf $(BUILD_DIR) $(BIN)
