.arch armv5te

.arm
.align 2
.global lz11DecompressASM
.type lz11DecompressASM, %function
lz11DecompressASM:
  push   {r4-r6}
.Lmain_loop:
  ldrb   r12, [r0], #1       @ flags = *in++
  mov    r3, #0              @ i = 0

.Lsub_loop:
  cmp    r2, #0              @ if(size <= 0)
  pople  {r4-r6}             @   pop stack
  bxle   lr                  @   return

  cmp    r3, #8              @ if(i == 8)
  beq    .Lmain_loop         @   goto main_loop

  tst    r12, #0x80          @ if(flags & 0x80 != 0x80)
  lsl    r12, r12, #1        @ (unconditionally) flags <<= 1
  beq    .Lcopy_uncompressed @   goto copy_uncompressed

  ldrb   r4, [r0], #1        @ r4 = *in++
  and    r5, r4, #0xF0       @ r5 = r4 & 0xF0
  cmp    r5, #0x00           @ if(r5 == 0x00)
  beq    .Lextended          @   goto extended
  cmp    r5, #0x10           @ if(r5 == 0x10)
  beq    .LXextended         @   goto Xextended

  lsr    r6, r4, #4          @ len = r4>>4
  add    r6, r6, #1          @ len++
  b      .L1

.Lextended:
  lsl    r6, r4, #4          @ len = r4<<4
  ldrb   r4, [r0], #1        @ r4 = *in++
  orr    r6, r6, r4, lsr #4  @ len = len | (r4>>4)
  add    r6, r6, #0x11       @ len += 0x11
  b      .L1

.LXextended:
  and    r4, r4, #0x0F       @ r4 &= 0x0F
  lsl    r6, r4, #12         @ len = r4<<12
  ldrb   r4, [r0], #1        @ r4 = *in++
  orr    r6, r6, r4, lsl #4  @ len |= r4<<4
  ldrb   r4, [r0], #1        @ r4 = *in++
  orr    r6, r6, r4, lsr #4  @ len |= r4>>4
  add    r6, r6, #0x011      @ len += 0x011
  add    r6, r6, #0x100      @ len += 0x100

.L1:
  and    r5, r4, #0x0F       @ disp = r4 & 0x0F      note: disp is in r5
  ldrb   r4, [r0], #1        @ r4 = *in++
  orr    r4, r4, r5, lsl #8  @ disp = r4 | (disp<<8) note: disp changes to r4
  add    r4, r4, #1          @ disp++
  sub    r2, r2, r6          @ size -= len
  add    r3, r3, #1          @ i++

.Lcopy_compressed:
  subs   r6, r6, #1          @ if(len-- == 1)
  blt    .Lsub_loop          @   goto sub_loop
  ldrb   r5, [r1, -r4]       @ r5 = *(out - disp)
  strb   r5, [r1], #1        @ *out++ = r5
  b      .Lcopy_compressed   @ goto copy_compressed

.Lcopy_uncompressed:
  ldrb   r4, [r0], #1        @ r4 = *in++
  strb   r4, [r1], #1        @ *out++ = r4
  add    r3, r3, #1          @ i++
  sub    r2, r2, #1          @ size--
  b      .Lsub_loop          @ goto sub_loop

