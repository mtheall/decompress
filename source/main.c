#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "decompress.h"
#include "raw_img_bin.h"
#include "raw_rand_bin.h"
#include "raw_zero_bin.h"
#include "lzss_img_bin.h"
#include "lzss_rand_bin.h"
#include "lzss_zero_bin.h"
#include "lz11_img_bin.h"
#include "rle_img_bin.h"
#include "huf_img_bin.h"

#define SIZE 49152
static u8 buf[SIZE];

void testLZSS(const u8 *in, const u8 *out);
void testLZ11(const u8 *in, const u8 *out);
void testRLE(const u8 *in, const u8 *out);
void testHuff(const u8 *in, const u8 *out);

typedef struct {
  char *name;
  void (*test)(const u8 *, const u8 *);
  const u8 *in;
  const u8 *out;
} Test_t;

Test_t tests[] = {
  {
    .name = "LZSS - img",
    .test = testLZSS,
    .in   = lzss_img_bin,
    .out  = raw_img_bin,
  },
  {
    .name = "LZSS - zero",
    .test = testLZSS,
    .in   = lzss_zero_bin,
    .out  = raw_zero_bin,
  },
  {
    .name = "LZSS - rand",
    .test = testLZSS,
    .in   = lzss_rand_bin,
    .out  = raw_rand_bin,
  },
  {
    .name = "LZ11 - img",
    .test = testLZ11,
    .in   = lz11_img_bin,
    .out  = raw_img_bin,
  },
  {
    .name = "RLE - img",
    .test = testRLE,
    .in   = rle_img_bin,
    .out  = raw_img_bin,
  },
  {
    .name = "Huff - img",
    .test = testHuff,
    .in   = huf_img_bin,
    .out  = raw_img_bin,
  },
};
#define numTests (sizeof(tests)/sizeof(tests[0]))

int main(int argc, char *argv[]) {
  int choice = 0;
  int down   = 0;
  int i;

  consoleDemoInit();

  while(!(down & KEY_B)) {
    for(i = 0; i < numTests; i++)
      iprintf("%c%s\n", choice == i ? '*' : ' ', tests[i].name);

    do {
      swiWaitForVBlank();
      scanKeys();
      down = keysDown();
    } while(!down);

    iprintf("\x1b[2J");

    if(down & KEY_UP)
      choice = (choice + (numTests-1)) % numTests;
    else if(down & KEY_DOWN)
      choice = (choice + 1) % numTests;
    else if(down & KEY_A) {
      iprintf("%s\n", tests[choice].name);
      tests[choice].test(tests[choice].in, tests[choice].out);
      do {
        swiWaitForVBlank();
        scanKeys();
        down = keysDown();
      } while(!(down & KEY_A));
      iprintf("\x1b[2J");
    }
  }

  return 0;
}

#define FAIL(routine)       iprintf(" %-10s: failure\n", #routine)
#define PASS(routine, time) iprintf(" %-10s: %7d\n", #routine, time)

void testLZSS(const u8 *in, const u8 *out) {
  int timer;

  /* LZSS in C */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  lzssDecompress(in+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(lzss_C);
  else
    PASS(lzss_C, timer);

  /* LZSS in asm */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  lzssDecompressASM(in+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(lzss_asm);
  else
    PASS(lzss_asm, timer);

  /* LZSS in asm - fincs version */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  LZ77_Decompress(in, buf);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(lzss_fincs);
  else
    PASS(lzss_fincs, timer);

  /* LZSS in BIOS */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  decompress(in, buf, LZ77);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(lzss_swi);
  else
    PASS(lzss_swi, timer);
}

void testLZ11(const u8 *in, const u8 *out) {
  int timer;

  /* LZ11 in C */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  lz11Decompress(in+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(lz11_C);
  else
    PASS(lz11_C, timer);

  /* LZ11 in asm */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  lz11DecompressASM(in+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(lz11_asm);
  else
    PASS(lz11_asm, timer);
}

void testRLE(const u8 *in, const u8 *out) {
  int timer;

  /* RLE in C */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  rleDecompress(in+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(rle_C);
  else
    PASS(rle_C, timer);

  /* RLE in asm */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  rleDecompressASM(in+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(rle_asm);
  else
    PASS(rle_asm, timer);

  /* RLE in BIOS */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  decompress(in, buf, RLE);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(rle_swi);
  else
    PASS(rle_swi, timer);
}

void testHuff(const u8 *in, const u8 *out) {
  int timer;

  /* Huffman in C */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  huffDecompress(in+4, buf, SIZE, 8);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(huff_C);
  else
    PASS(huff_C, timer);

  /* Huffman in asm */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  huffDecompressASM(in+4, buf, SIZE, 8);
  timer = cpuEndTiming();

  if(memcmp(buf, out, SIZE))
    FAIL(huff_asm);
  else
    PASS(huff_asm, timer);
}

