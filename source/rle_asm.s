.arch armv5te

.arm
.align 2
.global rleDecompressASM
.type rleDecompressASM, %function
rleDecompressASM:
  cmp    r2,  #0             @ if(size <= 0)
  bxle   lr                  @   return
  ldrb   r12, [r0], #1       @ len = *in++
  tst    r12, #0x80          @ if(len & 0x80 == 0)
  and    r12, r12, #0x7F     @ <unconditionally> len &= 0x7F
  beq    .Luncompressed      @   goto uncompressed

  add    r12, r12, #3        @ len += 3
  sub    r2,  r2,  r12       @ size -= len
  ldrb   r3, [r0], #1        @ r3 = *in++
.Lcopy_compressed:
  subs   r12, r12, #1        @ --len
  strb   r3, [r1], #1        @ *out++ = r3
  bne    .Lcopy_compressed   @ if(len != 0) goto copy_compressed
  b      rleDecompressASM    @ goto rleDecompressASM

.Luncompressed:
  add    r12, r12, #1        @ len += 1
  sub    r2,  r2,  r12       @ size -= len
.Lcopy_uncompressed:
  ldrb   r3, [r0], #1        @ r3 = *in++
  subs   r12, r12, #1        @ --len
  strb   r3, [r1], #1        @ *out++ = r3
  bne    .Lcopy_uncompressed @ if(len != 0) goto copy_uncompressed
  b      rleDecompressASM    @ goto rleDecompressASM

