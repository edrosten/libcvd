	.text
.globl int_difference
	.type	int_difference, @function
int_difference:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi
	
	call	mcount
	
	movl	8(%ebp), %esi	/* a */
	movl	12(%ebp), %edi	/* b */
	movl	16(%ebp), %ebx  /* c */ 
	movl	20(%ebp), %ecx	/* size */
	shrl	$1, %ecx
	jz	.done
.pixel_loop:
	movq	(%esi), %mm0
	addl	$8, %esi
	movq	(%edi), %mm1
	addl	$8, %edi
	psubd	%mm0, %mm1
	movq	%mm1, (%ebx)
	addl	$8, %ebx
	decl	%ecx
	jnz	.pixel_loop
.done:
	emms
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	popl	%ebp
	ret
	.size	int_difference, .-int_difference
