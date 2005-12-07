	.text
.globl float_innerproduct
	.type	float_innerproduct, @function
float_innerproduct:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi

	subl	$16, %esp
	call	mcount
	
	movl	8(%ebp), %esi	/* a */
	movl	12(%ebp), %edi	/* b */
	movl	16(%ebp), %ecx  /* w */
	movl	20(%ebp), %edx  /* h */
	movl	24(%ebp), %eax  /* sa */
	movl	28(%ebp), %ebx  /* sb */
	subl	%ecx, %eax
	subl	%ecx, %ebx
	shll	$2, %eax
	shll	$2, %ebx
	shrl	$2, %ecx       
	jz	.done
	movl	%ecx, 16(%ebp) /* chunks of four per line */

	xorps	%xmm0, %xmm0
.outer_loop:
	movl	16(%ebp), %ecx	
.inner_loop:
	movups	(%esi), %xmm1
	addl	$16, %esi
	movups	(%edi), %xmm2
	addl	$16, %edi
	mulps	%xmm2, %xmm1
	addps	%xmm1, %xmm0
	decl	%ecx
	jnz	.inner_loop
	
	addl	%eax, %esi   /* next line */
	addl	%ebx, %edi   /* next line */
	decl	%edx
	jnz	.outer_loop
	
	movups	%xmm0, (%esp)
	flds	(%esp)
	fadds	4(%esp)
	fadds	8(%esp)
	fadds	12(%esp)
.done:
	addl	$16, %esp
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	popl	%ebp
	ret
	.size	float_innerproduct, .-float_innerproduct
