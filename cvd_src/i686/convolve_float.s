
	.text
.globl convolve_float
	.type	convolve_float, @function
convolve_float:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%edi
	pushl	%esi

	call	mcount
	movl	8(%ebp), %esi	/* float* I */
	movl	12(%ebp), %eax	/* width */
	movl	16(%ebp), %ebx  /* height */
	movl	20(%ebp), %edi	/* float* kernel */
	movl	24(%ebp), %edx	/* k */
	shll	$4, %edx
	subl	%edx, %esp
	subl	$64, %esp
	movl	%esp, %eax
	andl	$0xF, %eax
	subl	%eax, %esp
	movl	%eax, 32(%esp)

	shrl	$2, %edx
.expand_kernel:
	subl	$4, %edx
	movl	(%edi, %edx), %eax
	movl	%eax, 64(%esp, %edx, 4)
	movl	%eax, 68(%esp, %edx, 4)
	movl	%eax, 72(%esp, %edx, 4)
	movl	%eax, 76(%esp, %edx, 4)
	cmpl	$0, %edx
	jnz	.expand_kernel

	movl	24(%ebp), %edx	/* k */
	shrl	%edx		/* r = k/2 */
	shll	$2, %edx
	movl	%edx, 8(%esp)	/* r*4 */

	movl	%ebx, 4(%esp)   /* height */
	xor	%ebx, %ebx
	movl	%ebx, 16(%esp)	/* zero */
	movl	%ebx, 20(%esp)
	movl	%ebx, 24(%esp)
	movl	%ebx, 28(%esp)
.x_row_loop:
	movl	8(%ebp), %esi         /* I */
	addl	%ebx, %esi	      /* I+i*w */
	movl	12(%ebp), %eax	/* width */
	shll	$2, %eax
	addl	%eax, %ebx	/* next row */
	movl	12(%ebp), %eax	/* width */
	movl	24(%ebp), %edx
	andl	$0xFFFFFFFE, %edx
	subl	%edx, %eax	/* w-2*r */
.x_col_loop:
	movl	24(%ebp), %ecx
	shll	$4, %ecx
	movaps	16(%esp), %xmm2
.x_sum_loop:
	subl	$16, %ecx
	movaps	64(%esp, %ecx), %xmm0 /* kernel element */
	movaps	(%esi, %ecx), %xmm1	 /* data element ecx */
	mulps	%xmm0, %xmm1
	addps	%xmm1, %xmm2
	jnz	.x_sum_loop

	movaps	%xmm2, (%esi)
	addl	$16, %esi
	decl	%eax
	jnz	.x_col_loop

	movl	4(%esp), %eax
	decl	%eax
	movl	%eax, 4(%esp)
	jnz	.x_row_loop



	movl	12(%ebp), %ebx	/* width */
	shll	$4, %ebx	/* bytes per row */
	movl	%ebx, %eax
	movl	8(%esp), %edx
	subl	%edx, %eax
	subl	%edx, %eax	/* (w-2*r)*16 */
	movl	%eax, 12(%esp)

	movl	12(%ebp), %eax	/* width */
	incl	%eax
	imull	%edx, %eax
	movl	%eax, 8(%esp)	/* (width*r + r)*16 */
.y_col_loop:
	movl	8(%ebp), %esi   /* I */
	movl	12(%esp), %eax	/* col*16 */
	cmpl	$0, %eax
	jle	.done
	subl	$16, %eax
	movl	%eax, 12(%esp)
	addl	%eax, %esi

	movl	16(%ebp), %eax		/* height */
	movl	24(%ebp), %edx		/* k */
	andl	$0xFFFFFFFE, %edx	/* 2*r */
	subl	%edx, %eax		/* h-2*r */

.y_row_loop:
	prefetcht0 64(%esi)
	movl	24(%ebp), %ecx	/* k */
	leal	64(%esp), %edi	/* kernel */
	movl	%esi, %edx
	movaps	16(%esp), %xmm2	/* zero */
.y_sum_loop:
	movaps	(%edx), %xmm1	 /* data element ecx */
	addl	%ebx, %edx	/* ebx should be bytes_per_row */
	mulps	(%edi), %xmm1
	addl	$16, %edi
	addps	%xmm1, %xmm2
	decl	%ecx
	jnz	.y_sum_loop

	movl	8(%esp), %ecx	/* (width+1)*r*16 */
	movaps	%xmm2, (%esi, %ecx)
	addl	%ebx, %esi
	decl	%eax
	jnz	.y_row_loop

	jmp	.y_col_loop

.done:
	movl	32(%esp), %eax
	movl	24(%ebp), %edx
	shll	$4, %edx
	addl	%edx, %eax
	addl	$64, %eax
	addl	%eax, %esp

	popl	%esi
	popl	%edi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%ebp
	ret
	.size	convolve_float, .-convolve_float
