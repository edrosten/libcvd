
	.text
.globl cvd_asm_halfsample_mmx
	.type	cvd_asm_halfsample_mmx, @function
cvd_asm_halfsample_mmx:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%edi
	pushl	%esi
/*	call	mcount*/
	movl	8(%ebp), %esi
	movl	12(%ebp), %edi
	movl	16(%ebp), %ebx
	movl	20(%ebp), %edx
	shrl	%edx

	xor	%eax, %eax
	pinsrw	$0, %eax, %mm4
	pshufw	$0, %mm4, %mm4
	movb	$0xFF, %al
	pinsrw	$0, %eax, %mm3
	pshufw  $0, %mm3, %mm3
.outer_loop:
	movl	16(%ebp), %ecx
	shrl	$3, %ecx
.inner_loop:
	movq	(%esi), %mm0
	movq	(%esi, %ebx), %mm1
	addl	$8, %esi
	pavgb	%mm1, %mm0
	movq	%mm0, %mm2
	pand	%mm3, %mm0
	psrlw	$8, %mm2
	pavgw	%mm2, %mm0

	packuswb %mm4, %mm0
	movd	%mm0, (%edi)
	addl	$4, %edi
	decl	%ecx
	jnz	.inner_loop

	movl	16(%ebp), %eax
	addl	%eax, %esi
	andl	$7, %eax
	addl	%eax, %esi
	shrl	%eax
	addl	%eax, %edi
	decl	%edx
	jnz	.outer_loop
.done:
	emms
	popl	%esi
	popl	%edi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%ebp
	ret
	.size	cvd_asm_halfsample_mmx, .-cvd_asm_halfsample_mmx
