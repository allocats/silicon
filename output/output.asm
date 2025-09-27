global _start

_start:
	sub rsp, 4
	mov [rsp-4], word 69
	mov rdi, [rsp-4]
	add rsp, 4

	mov rax, 60
	syscall