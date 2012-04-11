#include <nds.h>
#include <stdio.h>
#include "decompress.h"
#include "raw_img_bin.h"
#include "lzss_img_bin.h"

#define SIZE 49152
static u8 buf[SIZE];

void testLZSS();

int main(int argc, char *argv[]) {
  consoleDemoInit();

  testLZSS();

  while(1)
    swiWaitForVBlank();

  return 0;
}

#define FAIL(routine)       printf("%-8s: failure\n", #routine)
#define PASS(routine, time) printf("%-8s: %7d\n", #routine, time)

void testLZSS() {
  int timer;

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
}

