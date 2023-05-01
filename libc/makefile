# Builds the custom libc for the kernel
# Should be imported by ../makefile

CPPFLAGS +=\
	-Iinclude \
	$(LIBCFLAGS)

CFILES := $(shell find src arch/$(TARGET) -type f -name "*.c")
ASFILES := $(shell find src arch/$(TARGET) -type f -name "*.s")
NASMFILES := $(shell find src arch/$(TARGET) -type f -name "*.asm")
OBJ := $(CFILES:.c=.o) $(ASFILES:.s=.o) $(NASMFILES:.asm=.o)
DEP := $(CFILES:.c=.d) $(ASFILES:.s=.d)
OUTPUT := libk.a

libk.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ) $(DEP) $(OUTPUT)