/*
in the x86_64 target, the limine bootloader is expected to be used.
*/

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	# Call the global constructors.
	call _init

	call term_init

	# Transfer control to the main kernel.
	call main_x86_64

	# Hang if main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start
