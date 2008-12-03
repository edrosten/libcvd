	.text
.globl cvd_asm_yuv422_to_rgb
	.type	cvd_asm_yuv422_to_rgb, @function
cvd_asm_yuv422_to_rgb:	
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	
/*	call	mcount*/

	movl	8(%ebp), %esi	/* yuv */
	movl	12(%ebp), %edi	/* rgb */
	movl	16(%ebp), %ecx  /* size */
	shrl	$3, %ecx
	
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

	movl	$128, %eax
	pinsrw	$0, %eax, %mm5
	pshufw  $0, %mm5, %mm5

.pixel_loop:
	/*	prefetcht0	16(%esi)  */
	/*******************  first four *****************************/
	movq	(%esi), %mm0
	movq	%mm0, %mm4
	psllw	$8, %mm4
	psrlw	$1, %mm4
	psrlw	$8, %mm0	/* u1 v1 u2 v2 */	

	psubw	%mm5, %mm0	  /* (u1-128) (v1-128) (u2-128) (v2-128) */
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
	pinsrw	$3, %eax, %mm2	  /* uvg1 ub1 vr2 uvg2 */
	
	pshufw	$0x6d, %mm1, %mm1 /* ub2 vr2 uvg2 ub2 */

	pshufw	$0x40, %mm4, %mm3 /* y1 y1 y1 y2 */
	paddsw	%mm3, %mm0 
	pshufw	$0xa5, %mm4, %mm3 /* y2 y2 y3 y3 */
	paddsw	%mm3, %mm2 
	pshufw	$0xfe, %mm4, %mm3 /* y3 y4 y4 y4 */
	paddsw	%mm3, %mm1
	
	psraw   $7, %mm0
	psraw   $7, %mm2
	psraw   $7, %mm1
	packuswb %mm2, %mm0
	movntq	%mm0, (%edi)

	movq	%mm1, -12(%ebp)	/** save for later */
	
	/******************* second four *****************************/
	movq	8(%esi), %mm0
	movq	%mm0, %mm4
	psllw	$8, %mm4
	psrlw	$1, %mm4
	psrlw	$8, %mm0	/* u1 v1 u2 v2 */	

	psubw	%mm5, %mm0	  /* (u1-128) (v1-128) (u2-128) (v2-128) */
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
	pinsrw	$3, %eax, %mm2	  /* uvg1 ub1 vr2 uvg2 */
	
	pshufw	$0x6d, %mm1, %mm1 /* ub2 vr2 uvg2 ub2 */

	pshufw	$0x40, %mm4, %mm3 /* y1 y1 y1 y2 */
	paddsw	%mm3, %mm0 
	pshufw	$0xa5, %mm4, %mm3 /* y2 y2 y3 y3 */
	paddsw	%mm3, %mm2 
	pshufw	$0xfe, %mm4, %mm3 /* y3 y4 y4 y4 */
	paddsw	%mm3, %mm1
	
	psraw   $7, %mm0
	psraw   $7, %mm2
	psraw   $7, %mm1
		
	movq	-12(%ebp), %mm4
	packuswb %mm0, %mm4
	packuswb %mm1, %mm2

	movntq  %mm4, 8(%edi)
	movntq	%mm2, 16(%edi)
	
	addl	$24, %edi
	addl	$16, %esi
	decl	%ecx
	jnz	.pixel_loop
.done:
	emms
	leave
	ret
	.size	cvd_asm_yuv422_to_rgb, .-cvd_asm_yuv422_to_rgb
