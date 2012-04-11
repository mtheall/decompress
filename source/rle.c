#include <string.h>
#include "decompress.h"

void rleDecompress(const u8 *src, u8 *dst, int size) {
  int len;
  unsigned char byte;

  while(size > 0) {
    // read in the data header
    byte = *src++;

    if(byte & 0x80) { // compressed block
      // read the length of the run
      len = (byte & (0x7F)) + 3;
      size -= len;

      // read in the byte used for the run
      byte = *src++;

      // for len, copy byte into output
      memset(dst, byte, len);
      dst += len;
    }

    else { // uncompressed block
      // read the length of uncompressed bytes
      len = (byte & (0x7F)) + 1;
      size -= len;

      // for len, copy from input into output
      memcpy(dst, src, len);
      dst += len;
      src  += len;
    }
  }
}

