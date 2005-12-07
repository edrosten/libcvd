	.text
.globl float_difference
	.type	float_difference, @function
float_difference:
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
	shrl	$2, %ecx
	jz	.done
.pixel_loop:
	movaps	(%edi), %xmm0
	addl	$16, %edi
	subps	(%esi), %xmm0
	addl	$16, %esi
	movaps	%xmm0, (%ebx)
	addl	$16, %ebx
	decl	%ecx
	jnz	.pixel_loop
.done:
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	popl	%ebp
	ret
	.size	float_difference, .-float_difference
