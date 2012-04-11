#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <nds.h>

void lzssDecompress(const u8 *src, u8 *dst, int len);
void lzssDecompressASM(const u8 *src, u8 *dst, int len);
void LZ77_Decompress(const u8 *src, u8 *dst);

#endif /* DECOMPRESS_H */

