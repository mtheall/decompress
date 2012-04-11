#include <nds.h>
#include <stdio.h>
#include "decompress.h"
#include "raw_img_bin.h"
#include "lzss_img_bin.h"
#include "rle_img_bin.h"

#define SIZE 49152
static u8 buf[SIZE];

void testLZSS();
void testRLE();

int main(int argc, char *argv[]) {
  consoleDemoInit();

  testLZSS();
  testRLE();

  while(1)
    swiWaitForVBlank();

  return 0;
}

#define FAIL(routine)       printf("%-10s: failure\n", #routine)
#define PASS(routine, time) printf("%-10s: %7d\n", #routine, time)

void testLZSS() {
  int timer;

  /* LZSS in C */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  lzssDecompress(lzss_img_bin+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, raw_img_bin, SIZE))
    FAIL(lzss_C);
  else
    PASS(lzss_C, timer);

  /* LZSS in asm */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  lzssDecompressASM(lzss_img_bin+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, raw_img_bin, SIZE))
    FAIL(lzss_asm);
  else
    PASS(lzss_asm, timer);

  /* LZSS in asm - fincs version */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  LZ77_Decompress(lzss_img_bin, buf);
  timer = cpuEndTiming();

  if(memcmp(buf, raw_img_bin, SIZE))
    FAIL(lzss_fincs);
  else
    PASS(lzss_fincs, timer);

  /* LZSS in BIOS */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  decompress(lzss_img_bin, buf, LZ77);
  timer = cpuEndTiming();

  if(memcmp(buf, raw_img_bin, SIZE))
    FAIL(lzss_swi);
  else
    PASS(lzss_swi, timer);
}

void testRLE() {
  int timer;

  /* RLE in C */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  rleDecompress(rle_img_bin+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, raw_img_bin, SIZE))
    FAIL(rle_C);
  else
    PASS(rle_C, timer);

  /* RLE in asm */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  rleDecompressASM(rle_img_bin+4, buf, SIZE);
  timer = cpuEndTiming();

  if(memcmp(buf, raw_img_bin, SIZE))
    FAIL(rle_asm);
  else
    PASS(rle_asm, timer);

  /* RLE in BIOS */
  memset(buf, 0, SIZE);
  DC_FlushAll();
  DC_InvalidateAll();

  cpuStartTiming(0);
  decompress(rle_img_bin, buf, RLE);
  timer = cpuEndTiming();

  if(memcmp(buf, raw_img_bin, SIZE))
    FAIL(rle_swi);
  else
    PASS(rle_swi, timer);
}

