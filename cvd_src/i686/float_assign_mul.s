/* c[j] += f * (a[j] + b[j]) */
	.text
.globl float_assign_mul
	.type	float_assign_mul, @function
float_assign_mul:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi
	
	call	mcount
	
	movl	8(%ebp), %esi	/* in */
	movl	12(%ebp), %edx  /* f */ 
	movl	16(%ebp), %ebx  /* out */ 
	movl	20(%ebp), %ecx	/* size */
	shrl	$2, %ecx
	jz	.done
	movss	12(%ebp), %xmm4
	shufps	$0, %xmm4, %xmm4
.pixel_loop:
	movaps	(%esi), %xmm0
	addl	$16, %esi
	mulps	%xmm4, %xmm0
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
	.size	float_assign_mul, .-float_assign_mul

	