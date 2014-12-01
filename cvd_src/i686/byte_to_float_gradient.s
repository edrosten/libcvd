
	.section	.rodata
	.align 8
.double255inv:
	.long	269488144
	.long	1064308752
.float255:
	.long	1132396544
	.text
.globl byte_to_float_gradient
	.type	byte_to_float_gradient, @function
byte_to_float_gradient:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$12, %esp
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%edi
	pushl	%esi
	call	mcount
	movl	8(%ebp), %esi	/* gray */
	movl	12(%ebp), %edi	/* grad */
	movl	16(%ebp), %eax  /* width */
	movl	20(%ebp), %ebx	/* height */
	imull	%ebx, %eax
	movl	%eax, %edx
	subl	$2, %edx
	fldl	.double255inv
.x_loop:
	movzbl	(%esi), %ebx
	movzbl	2(%esi), %eax
	subl	%ebx, %eax
	movl	%eax, -4(%ebp)
	fildl	-4(%ebp)
	fmul	%st(1), %st(0)
	fstps	8(%edi)
	addl	$8, %edi
	incl	%esi
	decl	%edx
	jnz	.x_loop

	movl	8(%ebp), %esi	/* gray */
	movl	12(%ebp), %edi	/* grad */
	movl	16(%ebp), %eax  /* width */
	movl	20(%ebp), %ebx	/* height */
	leal	(%eax, %eax), %ecx /* 2*width */
	imull	%ebx, %eax
	movl	%eax, %edx	/* width * height */
	subl	%ecx, %edx	/* minus two rows */
	leal	(%edi, %ecx, 4), %edi	/* second row */
.y_loop:
	movzbl	(%esi), %ebx
	movzbl	(%esi,%ecx), %eax
	subl	%ebx, %eax
	movl	%eax, -4(%ebp)
	fildl	-4(%ebp)
	fmul	%st(1), %st(0)
	fstps	4(%edi)
	addl	$8, %edi
	incl	%esi
	decl	%edx
	jnz	.y_loop

.done:
	emms
	popl	%esi
	popl	%edi
	popl	%edx
	popl	%ecx
	popl	%ebx
	addl	$12, %esp
	popl	%ebp
	ret
	.size	byte_to_float_gradient, .-byte_to_float_gradient
