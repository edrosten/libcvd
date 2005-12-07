/* c[j] += f * (a[j] + b[j]) */
	.text
.globl float_add_mul_add_unaligned
	.type	float_add_mul_add_unaligned, @function
float_add_mul_add_unaligned:
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
	movl	16(%ebp), %edx  /* f */ 
	movl	20(%ebp), %ebx  /* c */ 
	movl	24(%ebp), %ecx	/* size */
	shrl	$2, %ecx
	jz	.done
	movss	16(%ebp), %xmm4
	shufps	$0, %xmm4, %xmm4
.pixel_loop:
	movups	(%edi), %xmm0
	addl	$16, %edi
	addps	(%esi), %xmm0
	addl	$16, %esi
	mulps	%xmm4, %xmm0
	movups	(%ebx), %xmm2
	addl	$16, %ebx
	addps	%xmm2, %xmm0
	movups	%xmm0, -16(%ebx)
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
	.size	float_add_mul_add_unaligned, .-float_add_mul_add_unaligned

	