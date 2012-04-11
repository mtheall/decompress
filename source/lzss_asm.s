.arch armv5te

.arm
.align 2
.global lzssDecompressASM
.type lzssDecompressASM, %function
lzssDecompressASM:
  push   {r4-r6}
  ldr    r3, =(0x01010101)

.Lloop:
  cmp    r2, #0              @ if(size <= 0)
  pople  {r4-r6}             @   pop stack
  bxle   lr                  @   return

  rors   r3, r3, #1          @ r3 = (r3<<31) | (r3>>1)
  ldrcsb r12, [r0], #1       @ if(old_r3 > new_r3) flags = *in++
  tst    r12, r3             @ if(flags & r3 == 0)
  beq    .Lcopy_uncompressed @   goto copy_uncompressed

  ldrb   r4, [r0], #1        @ r4 = *in++
  lsr    r6, r4, #4          @ len = r4>>4
  add    r6, r6, #3          @ len += 3
  and    r5, r4, #0x0F       @ disp = r4 & 0x0F      note: disp is in r5
  ldrb   r4, [r0], #1        @ r4 = *in++
  orr    r4, r4, r5, lsl #8  @ disp = r4 | (disp<<8) note: disp changes to r4
  add    r4, r4, #1          @ disp++
  sub    r2, r2, r6          @ size -= len

.Lcopy_compressed:
  subs   r6, r6, #1          @ if(len-- == 1)
  blt    .Lloop              @   goto loop
  ldrb   r5, [r1, -r4]       @ r5 = *(out - disp)
  strb   r5, [r1], #1        @ *out++ = r5
  b      .Lcopy_compressed   @ goto copy_compressed

.Lcopy_uncompressed:
  ldrb   r4, [r0], #1        @ r4 = *in++
  strb   r4, [r1], #1        @ *out++ = r4
  sub    r2, r2, #1          @ size--
  b      .Lloop              @ goto loop

