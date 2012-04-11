#include <nds.h>
#include <stdio.h>
#include "decompress.h"
#include "raw_img_bin.h"
#include "raw_rand_bin.h"
#include "lzss_img_bin.h"
#include "lzss_rand_bin.h"
#include "rle_img_bin.h"

#define SIZE 49152
static u8 buf[SIZE];

void testLZSS(const u8 *in, const u8 *out);
void testRLE(const u8 *in, const u8 *out);

int main(int argc, char *argv[]) {
  consoleDemoInit();

  iprintf("lzss_img_bin\n");
  testLZSS(lzss_img_bin, raw_img_bin);

  iprintf("lzss_rand_bin\n");
  testLZSS(lzss_rand_bin, raw_rand_bin);

  iprintf("rle_img_bin\n");
  testRLE(rle_img_bin, raw_img_bin);

  while(1)
    swiWaitForVBlank();

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

