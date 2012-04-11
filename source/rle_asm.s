.arch armv5te

.arm
.align 2
.global rleDecompressASM
.type rleDecompressASM, %function
rleDecompressASM:
  cmp    r2,  #0
  bxle   lr
  ldrb   r12, [r0], #1
  tst    r12, #0x80
  and    r12, r12, #0x7F
  beq    .Luncompressed
  add    r12, r12, #3
  sub    r2,  r2,  r12
  ldrb   r3, [r0], #1
.Lcopy_compressed:
  strb   r3, [r1], #1
  subs   r12, r12, #1
  bne    .Lcopy_compressed
  b      rleDecompressASM
.Luncompressed:
  add    r12, r12, #1
  sub    r2,  r2,  r12
.Lcopy_uncompressed:
  ldrb   r3, [r0], #1
  strb   r3, [r1], #1
  subs   r12, r12, #1
  bne    .Lcopy_uncompressed
  b      rleDecompressASM

