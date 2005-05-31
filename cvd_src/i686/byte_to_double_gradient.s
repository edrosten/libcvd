/*
        This file is part of the CVD Library.

        Copyright (C) 2005 The Authors

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

        You should have received a copy of the GNU Lesser General Public
        License along with this library; if not, write to the Free Software
        Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

	.section	.rodata
	.align 8
.double255inv:
	.long	269488144
	.long	1064308752
	.text
.globl byte_to_double_gradient
	.type	byte_to_double_gradient, @function
byte_to_double_gradient:
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
	fstpl	16(%edi)
	addl	$16, %edi
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
	leal	(%edi, %ecx, 8), %edi	/* second row */
.y_loop:
	movzbl	(%esi), %ebx
	movzbl	(%esi,%ecx), %eax
	subl	%ebx, %eax
	movl	%eax, -4(%ebp)
	fildl	-4(%ebp)
	fmul	%st(1), %st(0)
	fstpl	8(%edi)
	addl	$16, %edi
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
	.size	byte_to_double_gradient, .-byte_to_double_gradient
