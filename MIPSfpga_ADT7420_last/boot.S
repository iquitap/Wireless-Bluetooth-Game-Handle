/*
 * start.S for microAptiv_UP
 *
 *  Created on: April 12, 2014
 *  Author: MIPS TECHNOLOGIES, INC
 *  Start of boot code
*/
/*
Copyright (c) 2014, Imagination Technologies LLC and Imagination Technologies
Limited.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions in binary form must be built to execute on machines
   implementing the MIPS32(R), MIPS64 and/or microMIPS instruction set
   architectures.

2. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

3. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

4. Neither the name of Imagination Technologies LLC, Imagination Technologies Limited
   nor the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL IMAGINATION TECHNOLOGIES LLC OR IMAGINATION
TECHNOLOGIES LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


#include <mips/regdef.h>
#include "boot.h"
#include "fpga.h"
#include <mips/m32c0.h>

.set    noreorder               # Don't allow the assembler to reorder instructions.

    # The first function, __reset_vector, just loads the address of the __cpu_init function
    # and jumps to it. This does 2 things; First it will jump to a KSEG1 address which is a
    # mirror of the BEV’s KSEG0 address but cacheable. Second it will do a mode switch to
    # enable micromips mode.

/**************************************************************************************
    R E S E T   E X C E P T I O N   H A N D L E R
**************************************************************************************/

LEAF(__reset_vector)
    la a2,__cpu_init
    jr a2
    mtc0    $0, C0_COUNT        # Clear cp0 Count (Used to measure boot time.)
    nop

END(__reset_vector)

LEAF(__cpu_init)

    # Verify the code is here due to a reset and not NMI. If this is an NMI then trigger
    # a debugger breakpoint using a sdbp instruction.

    mfc0    s1, C0_STATUS       # Read CP0 Status
    ext     s1, s1, 19, 1       # extract NMI
    beqz    s1, init_resources  # Branch if this is NOT an NMI exception.
    nop
    sdbbp                       # Failed assertion: NMI.

init_resources:                 # initializes resources for "cpu".

    # Initialize CP0 registers

    la a2,      init_cp0        # Init CP0 Status, Count, Compare, Watch*, and Cause.
    jalr a2
    nop

    # Initialize the TLB

 #   la      a2, init_tlb        # Generate unique EntryHi contents per entry pair.
 #   jalr    a2
 #   nop

    #Initialize the Instruction cache

  #  la      a2, init_icache     # Initialize the L1 instruction cache. (Executing using I$ on return.)
  #  jalr    a2
   # nop

    # The changing of Kernel mode cacheability must be done from KSEG1
    # Since the code is executing from KSEG0 It needs to do a jump to KSEG1 change K0 and jump back to KSEG0

  #  la          a2, change_k0_cca
  #  li          a1, 0xf
  #  ins         a2, a1, 29, 1   # changed to KSEG1 address by setting bit 29
  #  jalr        a2
  #  nop

    # Initialize the Data cache

  #  la      a2, init_dcache     # Initialize the L1 data cache
  #  jalr    a2
  #  nop

    # Prepare for eret to main.

    la      ra, all_done        # If main returns then go to all_done:.
    move    a0, zero            # Indicate that there are no arguments available.
    la      v0, _start          # load the address of the CRT entry point _start.
    mtc0    v0, $30             # Write ErrorEPC with the address of main
    ehb                         # clear hazards (makes sure write to ErrorPC has completed)

    # Return from exception will now execute code in main

    eret                        # Exit reset exception handler and start execution of _start.


/**************************************************************************************/
all_done:
    # If main returns it will return to this point.  Just spin here.
    b       all_done
    nop

END(__cpu_init)

# Inline the code: fill the rest of space between here and the next exception vector address.

#include "init_caches.S"

/**************************************************************************************
    B O O T   E X C E P T I O N   H A N D L E R S (CP0 Status[BEV] = 1)
**************************************************************************************/
/* NOTE: the linker script must insure that this code starts at start + 0x200 so the exception */
/* vectors will be addressed properly. All .org assume this! */
/* TLB refill, 32 bit task. */
.org 0x180						# interrupt exception
	la a2,  __exception_entry   # prepare to enter the uart irq handler.
    jr a2
    nop

	
.org 0x200                      # TLB refill, 32 bit task.
	# display alternating one-zeros on board LEDs
    li      k0, BOARD_16_LEDS_ADDR  # Board LEDs display
    li      k1, 0x1111          # alternating one-zeros
    sw      k1, 0(k0)           # Write pattern to LEDs
    eret
    nop
	#la a2,  __exception_entry   # prepare to enter the uart irq handler.
    #jr a2
    #nop
    sdbbp                       # This has the effect of starting the debugger
    nop

.org 0x280                      # XTLB refill, 64 bit task. start + 0x280
	# display alternating one-zeros on board LEDs
    li      k0, BOARD_16_LEDS_ADDR  # Board LEDs display
    li      k1, 0x3333          # alternating one-zeros
    sw      k1, 0(k0)           # Write pattern to LEDs
    eret
    nop
    sdbbp                       # This has the effect of starting the debugger
    nop

	
.org 0x300                      # Cache error exception. start + 0x300
    sdbbp                       # This has the effect of starting the debugger
    nop

#include "init_tlb.S"


.org 0x380    /* General exception. */
	# disable interrupt
	mfc0    k0, C0_STATUS
	la		k1, 0xfffffffe
	and		k0, k0, k1			# StatusIE = 0
	mtc0	k0, C0_STATUS		# set the Status register to disable interrupt
	ehb

	la a2,  __exception_entry   # prepare to enter the uart irq handler.
    jr a2
    nop
	
    # display alternating one-zeros on board LEDs
    li      k0, BOARD_16_LEDS_ADDR  # Board LEDs display
    li      k1, 0x5555          # alternating one-zeros
    sw      k1, 0(k0)           # Write pattern to LEDs
    eret
    nop

.org 0x400    /* General interrupt. */
    # display alternating one-zeros on board LEDs
    li      k0, BOARD_16_LEDS_ADDR  # Board LEDs display
    li      k1, 0x7777          # alternating one-zeros
    sw      k1, 0(k0)           # Write pattern to LEDs
    eret
    nop
	

# If you want the above code to fit into 1k flash you will need to leave out the
# code below. This is the code that covers the debug exception which you normally will not get.

.org 0x480 /* EJTAG debug exception (EJTAG Control Register[ProbTrap] == 0.) */
    li      r24_malta_word, BOARD_16_LEDS_ADDR     # If post boot then $24 is clobbered.
    mtc0    a0, C0_DESAVE       # DeSave a0
    mfc0    a0, C0_DEPC         # Read DEPC
    sw      a0, 0(r24_malta_word)    # Display lower 16 bits of DEPC if there is not an EJTAG probe.
    mfc0    a0, C0_DESAVE       # Restore a0
1:  b       1b  /* Stay here */
    nop


#include "init_cp0.S"

#include "mini_crt0.S"


# the uart irq handler	
/*
 * mini_crt0.S
 *
 * Set up a basic runtime environment
*/
/*
Copyright (c) 2015, Imagination Technologies LLC and Imagination Technologies
Limited.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions in binary form must be built to execute on machines
   implementing the MIPS32(R), MIPS64 and/or microMIPS instruction set
   architectures.

2. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

3. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

4. Neither the name of Imagination Technologies LLC, Imagination Technologies Limited
   nor the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL IMAGINATION TECHNOLOGIES LLC OR IMAGINATION
TECHNOLOGIES LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include <mips/regdef.h>
#include "boot.h"
#include <mips/m32c0.h>


    .set    noreorder           # Don't allow the assembler to reorder instructions.
    .set    noat                # Don't allow the assembler to use r1(at) for synthetic instr.

/**************************************************************************************
**************************************************************************************/
LEAF(__exception_entry)

    move    k1,sp
    addiu   sp, sp, -176
    #sw  at, 16(sp)
    sw  v0, 20(sp)
    sw  v1, 24(sp)
    sw  a0, 28(sp)
    sw  a1, 32(sp)
    sw  a2, 36(sp)
    sw  a3, 40(sp)
    sw  t0, 44(sp)
    sw  t1, 48(sp)
    sw  t2, 52(sp)
    sw  t3, 56(sp)
    sw  t4, 60(sp)
    sw  t5, 64(sp)
    sw  t6, 68(sp)
    sw  t7, 72(sp)
    sw  s0, 76(sp)
    sw  s1, 80(sp)
    sw  s2, 84(sp)
    sw  s3, 88(sp)
    sw  s4, 92(sp)
    sw  s5, 96(sp)
    sw  s6, 100(sp)
    sw  s7, 104(sp)
    sw  t8, 108(sp)
    sw  t9, 112(sp)
    sw  k0, 116(sp)
    sw  gp, 124(sp)
    sw  k1, 128(sp)
    sw  s8, 132(sp)
    sw  ra, 136(sp)
    sw  zero,156(sp)
    mfhi    t1
    mflo    t2
    sw  t1,148(sp)
    sw  t2,152(sp)
    mfc0    ra,C0_EPC
    mfc0    s3,C0_CAUSE
    sw  ra,140(sp)
    mfc0    t1,C0_BADVADDR
    sw  t1,144(sp)
    sw  s3,164(sp)
    move    t3,zero
    move    t4,zero
    mfc0    t1,C0_CONFIG3
    ext t2,t1,0x1b,0x1
    beqz    t2, 1f
    mfc0    t3,$8,2
1:
    ext t1,t1,0x1a,0x1
    beqz    t1, 2f
    mfc0    t4,$8,1
2:
    sw  t3,172(sp)
    sw  t4,168(sp)
    move    a0,sp
    mfc0    s5,C0_STATUS
    lui t1,0x0
    addiu   t1,t1,-3
    and s5,s5,t1
    mtc0    s5,C0_STATUS
    sw  s5,160(sp)
    
    la  t9, _mips_handle_irq
    #andi   a1,s3,0x7c
    #lui a1, 0x10
    move a1, s3
    addiu   a0,a0,16
    #srl    a1,a1,0x2
    jalr  t9
    nop
    
    
        
    lw  t1,148(sp)
    lw  t2,152(sp)
    mthi    t1
    mtlo    t2
    lw  s5,160(sp)
    mtc0    s5,C0_STATUS
    lw  s6,140(sp)
    mtc0    s6,C0_EPC
    #lw at,16(sp)
    lw  v0,20(sp)
    lw  v1,24(sp)
    lw  a0,28(sp)
    lw  a1,32(sp)
    lw  a2,36(sp)
    lw  a3,40(sp)
    lw  t0,44(sp)
    lw  t1,48(sp)
    lw  t2,52(sp)
    lw  t3,56(sp)
    lw  t4,60(sp)
    lw  t5,64(sp)
    lw  t6,68(sp)
    lw  t7,72(sp)
    lw  s0,76(sp)
    lw  s1,80(sp)
    lw  s2,84(sp)
    lw  s3,88(sp)
    lw  s4,92(sp)
    lw  s5,96(sp)
    lw  s6,100(sp)
    lw  s7,104(sp)
    lw  t8,108(sp)
    lw  t9,112(sp)
    lw  k0,116(sp)
    lw  k1,120(sp)
    lw  gp,124(sp)
    lw  s8,132(sp)
    lw  ra,136(sp)
    lw  sp,128(sp)
    eret
    nop

    
END(__exception_entry)


