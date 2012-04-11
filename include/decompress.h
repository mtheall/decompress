#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <nds.h>

void lzssDecompress(const u8 *src, u8 *dst, int size);
void lzssDecompressASM(const u8 *src, u8 *dst, int size);
void LZ77_Decompress(const u8 *src, u8 *dst);

void rleDecompress(const u8 *src, u8 *dst, int size);
void rleDecompressASM(const u8 *src, u8 *dst, int size);

void huffDecompress(const u8 *src, u8 *dst, int size, int bits);

#endif /* DECOMPRESS_H */

