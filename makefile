# Builds the entire OS

export TARGET := x86_64
export ISONAME := os.iso
# toolchain
export TC := $(TARGET)-elf
export CC := $(TC)-gcc
export AR := $(TC)-ar
export LD := $(TC)-ld

export CFLAGS :=\
	-Wall \
	-Wextra \
	-std=gnu11 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-lto \
	-fno-PIE \
	-fno-PIC \
	-m64 \
	-march=x86-64 \
	-mabi=sysv \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel \
	-g \
	-O2 \
	-pipe

export CPPFLAGS :=\
	-I. \
	-MMD \
	-MP

export LDFLAGS :=\
	-nostdlib \
	-static \
	-m elf_x86_64 \
	-z max-page-size=0x1000 \

export LIBCFLAGS := -D__is_libk -I../kernel/include -I../limine

KERNEL := kernel/kernel.elf
LIBK := libc/libk.a
ISO := intos.iso

$(ISO): $(KERNEL)
	mkdir -p iso_root
	cp -v $(KERNEL) limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso_root -o $(ISO)
	./limine/limine-deploy $(ISO)

$(KERNEL): $(LIBK)
	make -C kernel

$(LIBK):
	make -C libc

# run using qemu
.PHONY: qemu
qemu: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -m 256M

.PHONY: clean
clean:
	rm -rf $(ISO)
	make -C kernel clean
	make -C libc clean
