	.section	.rodata
	.align 8
	.text
.globl cvd_asm_yuv422_to_grey
	.type	cvd_asm_yuv422_to_grey, @function
cvd_asm_yuv422_to_grey:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi
	
	subl	$12, %esp
/*	call	mcount */
	movl	8(%ebp), %esi	/* yuv */
	movl	12(%ebp), %edi	/* grey */
	movl	16(%ebp), %ecx  /* size */
	shrl	$3, %ecx
	
	movl	$0xFF, %eax
	pinsrw	$0, %eax, %mm5
	pshufw  $0, %mm5, %mm5

.pixel_loop:
	prefetchnta	16(%esi)
	movq	(%esi), %mm0
	movq	8(%esi), %mm1
	addl	$16, %esi
	pand	%mm5, %mm0
	pand	%mm5, %mm1
	packuswb %mm1, %mm0
	movntq	%mm0, (%edi)
	addl	$8, %edi
	decl	%ecx
	jnz	.pixel_loop
.done:
	emms
	addl	$12, %esp
	
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	popl	%ebp
	ret
	.size	cvd_asm_yuv422_to_grey, .-cvd_asm_yuv422_to_grey
