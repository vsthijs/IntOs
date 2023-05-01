# Builds the kernel
# Should be imported by ../makefile

CPPFLAGS +=\
	-Iinclude \
	-I../libc/include \
	-I../limine \
	-D__is_kernel \
	-D_TARGET_$(TARGET)

CFILES := $(shell find src arch/$(TARGET) -type f -name "*.c")
ASFILES := $(shell find src arch/$(TARGET) -type f -name "*.s")
NASMFILES := $(shell find src arch/$(TARGET) -type f -name "*.asm")
OBJ := $(CFILES:.c=.o) $(ASFILES:.s=.o) $(NASMFILES:.asm=.o)
DEP := $(CFILES:.c=.d) $(ASFILES:.s=.d)
OUTPUT := kernel.elf
LIBK:=../libc/libk.a

$(OUTPUT): $(OBJ)
	$(LD) $(OBJ) $(LIBK) $(LDFLAGS) -o $@ -T arch/$(TARGET)/linker.ld

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ) $(DEP) $(OUTPUT)
