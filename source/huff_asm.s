.arch armv5te

.arm
.align 2
.global huffDecompressASM
.type huffDecompressASM, %function
huffDecompressASM:
  push   {r4-r9}             @ push stack
  ldrb   r12, [r0]           @ treeSize = *in
  add    r12, r12, #1        @ treeSize += 1
  lsl    r12, r12, #1        @ treeSize *= 2
  mvn    r4, #0              @ dataMask = ~0
  mvn    r3, r4, lsl r3      @ dataMask = ~(dataMask<<bits)
  add    r4, r0, #1          @ root = in + 1
  mov    r5, r4              @ node = root
  add    r0, r0, r12         @ in += treeSize
  mov    r12, #1             @ mask = 1
  cmp    r2, #0

.Lloop:
  popeq  {r4-r9}             @ if(size == 0) pop stack
  bxeq   lr                  @ if(size == 0) return

  rors   r12, r12, #1        @ mask = (mask << 31) | (mask >> 1)
  bcc    .Lget_offset        @   goto get_offset
  ldrb   r6, [r0], #1        @ word = *in++
  ldrb   r7, [r0], #1        @ r7 = *in++
  orr    r6, r6, r7, lsl #8  @ word |= r7 << 8
  ldrb   r7, [r0], #1        @ r7 = *in++
  orr    r6, r6, r7, lsl #16 @ word |= r7 << 16
  ldrb   r7, [r0], #1        @ r7 = *in++
  orr    r6, r6, r7, lsl #24 @ word |= r7 << 24
.Lget_offset:
  ldrb   r7, [r5]            @ offset = *node
  and    r7, r7, #0x1F       @ offset &= 0x1F
  mov    r9, r5              @ child = node
  bic    r9, r9, #1          @ child &= ~1
  add    r9, r9, r7, lsl #1  @ child += offset*2
  ldrb   r7, [r5]            @ r7 = *node
  tst    r6, r12             @ if(word & mask == 0)
  beq    .Lleft              @   goto left

.Lright:
  add    r9, r9, #3          @ child += 3
  tst    r7, #0x40           @ if(r7 & 0x40 != 0)
  bne    .Lcopy_child        @   goto copy_child
  mov    r5, r9              @ node = child
  cmp    r2, #0
  b      .Lloop              @ goto loop

.Lleft:
  add    r9, r9, #2          @ child += 2
  tst    r7, #0x80           @ if(r7 & 0x80 != 0)
  bne    .Lcopy_child        @   goto copy_child
  mov    r5, r9              @ node = child
  cmp    r2, #0
  b      .Lloop              @ goto loop

.Lcopy_child:
  ldrb   r8, [r9]            @ r8 = *child
  and    r8, r3              @ r8 &= dataMask
  strb   r8, [r1], #1        @ *out++ = r8
  subs   r2, r2, #1          @ --size
  mov    r5, r4              @ node = root
  b      .Lloop              @ goto loop

