	.text
.globl cvd_asm_rgb_to_gray
	.type	cvd_asm_rgb_to_gray, @function
cvd_asm_rgb_to_gray:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi
/*	call	mcount*/
	movl	8(%ebp), %esi
	movl	12(%ebp), %edi
	movl	16(%ebp), %ecx
	shrl	$2, %ecx
	cmpl	$0, %ecx
	je	.done
	movl	20(%ebp), %eax
	pinsrw	$0, %eax, %mm3
	pshufw	$0, %mm3, %mm3
	movl	24(%ebp), %eax
	pinsrw	$0, %eax, %mm4
	pshufw	$0, %mm4, %mm4
	movl	28(%ebp), %eax
	pinsrw	$0, %eax, %mm5
	pshufw	$0, %mm5, %mm5
.pixel_loop:

	xor	%eax, %eax
	movl	(%esi), %ebx
	movl	4(%esi), %edx
	movb	%bl, %al
	pinsrw	$0, %eax, %mm0	/* r1 */
	shrl	$8, %ebx
	movb	%bl, %al
	pinsrw	$0, %eax, %mm1  /* g1 */
	shrl	$8, %ebx
	movb	%bl, %al
	pinsrw	$0, %eax, %mm2  /* b1 */
	shrl	$8, %ebx
	movb	%bl, %al
	pinsrw	$1, %eax, %mm0  /* r2 */
	movl	8(%esi), %ebx
	movb	%dl, %al
	pinsrw	$1, %eax, %mm1  /* g2 */
	shrl	$8, %edx
	movb	%dl, %al
	pinsrw	$1, %eax, %mm2  /* b2 */
	shrl	$8, %edx
	movb	%dl, %al
	pinsrw	$2, %eax, %mm0  /* r3 */
	shrl	$8, %edx
	movb	%dl, %al
	pinsrw	$2, %eax, %mm1  /* g3 */
	movb	%bl, %al
	pinsrw	$2, %eax, %mm2  /* b3 */
	shrl	$8, %ebx
	movb	%bl, %al
	pinsrw	$3, %eax, %mm0  /* r4 */
	shrl	$8, %ebx
	movb	%bl, %al
	pinsrw	$3, %eax, %mm1  /* g4 */
	shrl	$8, %ebx
	movb	%bl, %al
	pinsrw	$3, %eax, %mm2  /* b4 */

	pmullw	%mm3, %mm0
	pmullw	%mm4, %mm1
	paddusw	%mm1, %mm0
	pmullw	%mm5, %mm2
	paddusw	%mm2, %mm0
	psrlw	$8, %mm0

	packuswb %mm0, %mm0
	movd	%mm0, (%edi)

	addl	$12, %esi
	addl	$4, %edi
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
	.size	cvd_asm_rgb_to_gray, .-cvd_asm_rgb_to_gray
