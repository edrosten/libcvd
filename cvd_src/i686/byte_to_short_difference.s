	.text
.globl byte_to_short_difference
	.type	byte_to_short_difference, @function
byte_to_short_difference:
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
	shrl	$3, %ecx
	jz	.done
	xor	%eax,%eax
	pinsrw	$0, %eax, %mm7
	pshufw	$0, %mm7, %mm7
.pixel_loop:
	movq	(%esi), %mm0
	addl	$8, %esi
	movq	%mm0, %mm2
	
	movq	(%edi), %mm1
	addl	$8, %edi
	movq	%mm1, %mm3
	
	punpcklbw %mm7, %mm0  /* a1 a2 a3 a4 */
	punpckhbw %mm7, %mm2  /* a5 a6 a7 a8 */
	punpcklbw %mm7, %mm1  /* b1 b2 b3 b4 */
	punpckhbw %mm7, %mm3  /* b5 b6 b7 b8 */
	psubw	%mm0, %mm1
	psubw	%mm2, %mm3
	movntq	%mm1, (%ebx)
	movntq	%mm3, 8(%ebx)
	addl	$16, %ebx
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
	.size	byte_to_short_difference, .-byte_to_short_difference
