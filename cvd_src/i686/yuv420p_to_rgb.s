	.section	.rodata
	.align 8
	.text
.globl cvd_asm_yuv420p_to_rgb
	.type	cvd_asm_yuv420p_to_rgb, @function
cvd_asm_yuv420p_to_rgb:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$12, %esp
	call	mcount
	movl	8(%ebp), %esi	/* yuv */
	movl	12(%ebp), %edi	/* rgb */
	movl	16(%ebp), %eax  /* width */
	movl	20(%ebp), %ebx	/* height */
	movl	%eax, -8(%ebp)
	imull	%ebx
	movl	%eax, %ebx	/* width * height */
	addl	%esi, %ebx	/* beginning of uv data */
	
	movl	16(%ebp), %eax
	shrl	$1, %eax
	jz	.done
	movl	%eax, %ecx
	movl	%eax, 16(%ebp)  /* width/2 */

	movl	20(%ebp), %eax
	shrl	$1, %eax	/* height/2 */
	testl	%eax, %eax
	jz	.done

	imull	%ecx
	movl	%eax, %edx	/* width/2 * height/2 */
	
	movl	16(%ebp), %eax
	shrl	$1, %eax
	movl	%eax, 16(%ebp)	/* width/4 */

	xor	%eax, %eax
	movd	%eax, %mm7
	pshufw	$0, %mm7, %mm7	/* zeros */

	movl	$-38, %eax
	pinsrw	$0, %eax, %mm6
	movl	$256, %eax
	pinsrw	$1, %eax, %mm6
	movl	$-74, %eax
	pinsrw	$2, %eax, %mm6
	movl	$147, %eax
	pinsrw	$3, %eax, %mm6
	
.outer_loop:
	movl	16(%ebp), %ecx
.inner_loop_a:
	movzbl	(%ebx), %eax
	pinsrw	$0, %eax, %mm0
	movb	(%ebx, %edx), %al
	pinsrw	$1, %eax, %mm0
	movb	1(%ebx), %al
	pinsrw	$2, %eax, %mm0
	movb	1(%ebx, %edx), %al
	pinsrw	$3, %eax, %mm0    /* u1 v1 u2 v2 */
	movl	$128, %eax
	pinsrw	$0, %eax, %mm2
	pshufw	$0, %mm2, %mm2
	psubw	%mm2, %mm0	  /* (u1-128) (v1-128) (u2-128) (v2-128) */
	pshufw	$0xfa, %mm0, %mm1 /* U2 U2 V2 V2 */
	pshufw  $0x50, %mm0, %mm0 /* U1 U1 V1 V1 */

	pmullw	%mm6, %mm0        /* A*U1 B*U1 C*V1 D*V1 */	
	pmullw	%mm6, %mm1	  /* A*U2 B*U2 C*V2 D*V2 */

	pextrw	$0, %mm0, %eax	 
	pinsrw	$2, %eax, %mm7	  
	paddw	%mm7, %mm0	  /* x ub1 uvg1 vr1 */

	pextrw	$0, %mm1, %eax	 
	pinsrw	$2, %eax, %mm7	  
	paddw	%mm7, %mm1	  /* x ub2 uvg2 vr2 */
		
	pshufw	$0xdb, %mm0, %mm0 /* vr1 uvg1 ub1 vr1 */

	pshufw	$0x49, %mm0, %mm2 /* uvg1 ub1 vr1 uvg1 */
	pextrw	$3, %mm1, %eax
	pinsrw	$2, %eax, %mm2
	pextrw  $2, %mm1, %eax
	pinsrw	$3, %eax, %mm2	  /* vr2 uvg1 ub1 uvg2 */
	
	pshufw	$0x6d, %mm1, %mm1 /* ub2 vr2 uvg2 ub2 */

	movd	(%esi), %mm5
	punpcklbw %mm7, %mm5	/* y1 y2 y3 y4 */
	psllw	$7, %mm5
	pshufw	$0x40, %mm5, %mm3  /* y1 y1 y1 y2                         */
	pshufw	$0xa5, %mm5, %mm4  /*             y2 y2 y3 y3             */
	pshufw	$0xfe, %mm5, %mm5  /*                         y3 y4 y4 y4 */


	paddsw	%mm0, %mm3 
	psraw	$7, %mm3
	paddsw	%mm2, %mm4 
	psraw	$7, %mm4
	paddsw	%mm1, %mm5 
	psraw	$7, %mm5

	packuswb %mm4, %mm3
	packuswb %mm7, %mm5

	movq	%mm3, (%edi)
	movd	%mm5, 8(%edi)

	movl	-8(%ebp), %eax
	movd	(%esi, %eax), %mm5
	punpcklbw %mm7, %mm5	/* y1 y2 y3 y4 */
	psllw	$7, %mm5
	pshufw	$0x40, %mm5, %mm3  /* y1 y1 y1 y2                         */
	pshufw	$0xa5, %mm5, %mm4  /*             y2 y2 y3 y3             */
	pshufw	$0xfe, %mm5, %mm5  /*                         y3 y4 y4 y4 */

	paddsw	%mm0, %mm3 
	psraw	$7, %mm3
	paddsw	%mm2, %mm4 
	psraw	$7, %mm4
	paddsw	%mm1, %mm5 
	psraw	$7, %mm5

	packuswb %mm4, %mm3
	packuswb %mm7, %mm5

	leal	(%eax, %eax, 2), %eax
	movq	%mm3, (%edi, %eax)
	movd	%mm5, 8(%edi, %eax)
	
.inner_end_a:
	addl	$4, %esi
	addl	$2, %ebx
	addl	$12, %edi
	decl	%ecx
	jnz	.inner_loop_a
.outer_end:		
	addl	%eax, %edi
	addl	-8(%ebp), %esi

	movl	20(%ebp), %eax
	subl	$2, %eax
	movl	%eax, 20(%ebp)
	jnz	.outer_loop
.done:
	emms
	addl	$12, %esp
	popl	%ebp
	ret
	.size	cvd_asm_yuv420p_to_rgb, .-cvd_asm_yuv420p_to_rgb
