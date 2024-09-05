			;		DATASETS
set_1		dcd		10, 20, 10, 20, 20, 501
set_2		dcd		2, 4, 8, 16, 32, 64, 128, 256, 512
set_3		dcd		1, 2, 3, 4, 5, 16, 17, 18, 19, 20, 501
set_4		dcd		501
			
			
			;		Register Map:
			;		r0	current addr
			;		r1	current number
			;		r2	<15
			;		r5	>= 15
			adr		r0, set_1		; change the dataset here
			mov		r1, #0
			mov		r2, #0
			mov		r5, #0
			bl		count_loop
			end
			
			
count_loop
			ldr		r1, [r0], #4
			cmp		r1, #500
			movge	r15, r14		; return
			cmp		r1, #15
			addlt	r2, r2, #1	; < 15
			addge	r5, r5, #1	; >= 15
			b		count_loop
