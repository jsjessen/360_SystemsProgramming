#------------- s.s file -------------------
	.global get_ebp 
get_ebp: 
	movl %ebp, %eax
	ret
