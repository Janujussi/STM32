.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .data
.balign

array: .word 5,4,3,2,1,-1 // Input grades here.

.section .text
.balign
.global main

main:
		
			ldr r0, =array 		//base memory address
			
			//r1: current address data
			//r3: next address data
			//r5: placeholder
			//r6: number of values in array

			mov r1, #0
			mov r2, #0			//used as offset
			mov r3, #0
			mov r4, #4			//adding 4 because first part of program used for grabbing the next value in array
			mov r6, #0
			mov r7, #0
			mov r11, #0

			//r8 is used as min value
			//r9 is used as max value
			//r10 is used as average value
			//r11 is used as median value

valuecount:

			ldr r1, [r0, r2]	//load from current memory address

			cmp r1, #-1			//-1 moves to sort
			it eq
			moveq r2, #0
			it eq
			moveq r9, r6		//r9 now holds how many values there are
			it eq
			muleq r6, r6, r6	//sets how many times the program runs through the array to sort
			beq sort

			add r10, r1			//adding to sum of array
			add r6, #1			//count
			add r2, #4			//offset
			b valuecount

sort:

			cmp r6, #1			//if there is only 1 value in the array, find min and max
			beq minandmax

			ldr r1, [r0, r2]	//load from current memory address
			ldr r3, [r0, r4]	//load from next memory address

			cmp r3, #-1			// Finds if iterator has reached end of array...
			it eq				//... starting address from base
			moveq r2, #0
			it eq
			moveq r4, #4
			beq sort			//start of loop

			cmp r1, r3
			blt store			//if r1 < r3, swap values
			mov r5, r1
			mov r1, r3
			mov r3, r5

store:
			str r1, [r0, r2]	//store value
			str r3, [r0, r4]	//store value

			add r2, #4			//index
			add r4, #4			//index
			add r7, #1
			cmp r7, r6			//checking if algorithm has looped through array r6 many times
			beq minandmax
			b sort

minandmax:
	
			ldr r1, [r0]		//base address holds smallest number
			mov r8, r1
			
			mov r7, r9			//r7 holds number of values now
			sub r9, #1
			lsl r9, #2
			ldr r1, [r0, r9]	//multiplied r9 holds the offset for the last number
			mov r9, r1

average:

			udiv r10, r7		//divides sum of values by how many values there are

/**********************************************************************************************************/

			mov r4, r7			//figuring out whether there are an odd or even amount of numbers
			lsr r7, #1
			lsl r7, #1
			sub r7, r4, r7
			cmp r7, #2
			beq medianodd		//if odd amount, then just figure out the which address is middle
			cmp r7, #1
			beq medianodd
			cmp r7, #3
			beq medianodd

median:							//for even number

			mov r5, r4			//getting 2 middle numbers and averaging them
			sub r4, #2
			lsl r4, #1
			lsl r5, #1
			ldr r6,	[r0, r4]
			ldr r7, [r0, r5]

			sub r11, r7, r6
			lsr r11, #1
			add r11, r6

			b forever

medianodd:

			sub r4, #1
			lsl r4, #1			//finding middle value

			ldr r11, [r0, r4]

forever:
	b     forever