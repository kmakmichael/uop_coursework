.data
set_1:	
		.word 10
		.word 20
		.word 10
		.word 20
		.word 20
		.word 501
set_2:
		.word 2
		.word 4
		.word 8
		.word 16
		.word 32
		.word 64
		.word 128
		.word 256
		.word 512
set_3:
		.word 1
		.word 2
		.word 3
		.word 4
		.word 5
		.word 16
		.word 17
		.word 18
		.word 19
		.word 20
		.word 501
set_4:
		.word 501

.globl main
.text 
# Register Map:
# $s2: <15
# $s5 >= 15

main:
# setup
la		$a0, set_1		# change the dataset here

jal		f_counting
add		$s2, $zero, $v0 # < 15
add		$s5, $zero, $v1	# >= 15

ori $v0, $0, 10
syscall # Exit


# Counts numbers >15 and >= 15
# input: $a0: base address
# return:	$v0: <15, $v1: >= 15 
f_counting:

# setup
add		$t0, $zero, $a0
add		$v0, $zero, $zero
add		$v1, $zero, $zero

# register map:
# 	$t0 - current addr
# 	$t1 - current number
#	$t2 - store comparison results
f_counting_loop1:
lw		$t1, 0($t0)		# retrieve next number
slti	$t2, $t1, 501	# num < 501 ?
bne		$t2, $zero, f_counting_lt500
jr		$ra				# return if num > 500

f_counting_lt500:
slti	$t2, $t1, 15	# check num < 15 ?
bne		$t2, $zero, f_counting_lt15

addi	$v1, $v1, 1		# num >= 15, increment counter
addi	$t0, $t0, 4		# increment current addr
j		f_counting_loop1

f_counting_lt15:
addi	$v0, $v0, 1		# num < 15, increment counter
addi	$t0, $t0, 4		# increment current addr
j		f_counting_loop1
