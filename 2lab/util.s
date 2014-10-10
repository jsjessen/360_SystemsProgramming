#============== util.s file ===============
# James Jessen      
# 10918967         
 
# CptS 360
# Lab #2


.global get_esp, get_ebp, get_ebx, get_ecx, get_edx, get_esi, get_edi

# Points to the current top of the stack.
get_esp:		
	movl	%esp, %eax
	ret

# Points to the stack Frame of the current active function.
get_ebp:		
	movl	%ebp, %eax
	ret
	
get_ebx:
	movl	%ebx, %eax
	ret

get_ecx:
	movl	%ecx, %eax
	ret

get_edx:
	movl	%edx, %eax
	ret

get_esi:
	movl	%esi, %eax
	ret

get_edi:
	movl	%edi, %eax
	ret