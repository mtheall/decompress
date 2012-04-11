#include "decompress.h"

void huffDecompress(const u8 *in, u8 *out, int size, int bits) {
  u32 treeSize = ((*in)+1)*2; // size of the huffman header
  u32 word = 0;               // 32-bits of input bitstream
  u32 mask = 0;               // which bit we are reading
  u32 dataMask = (1<<bits)-1; // mask to apply to data
  u8  *node;                  // node in the huffman tree
  u8  *child;                 // child of a node
  u8  *root;                  // the root node of the huffman tree
  u32 offset;                 // offset from node to child

  // point to the root of the huffman tree
  if(treeSize <= 512) {
    root = alloca(treeSize);
    memcpy(root, in, treeSize);
    root++;
  }
  else
    root = (u8*)in+1;
  node = root;

  // move input pointer to beginning of bitstream
  in += treeSize;

  while(size > 0) {
    if(mask == 0) { // we exhausted 32 bits
      // reset the mask
      mask = 0x80000000;

      // read the next 32 bits
      memcpy(&word, in, 4);
      in += 4;
    }

    // read the current node's offset value
    offset = (*node)&0x1F;

    if(word & mask) { // we read a 1
      // point to the "right" child
      child = (u8 *)((((u32)node)&(~1))+offset*2+3);

      if((*node)&0x40) { // "right" child is a data node
        // copy the child node into the output buffer and apply mask
        *out++ = (*child)&dataMask;
        size--;

        // start over at the root node
        node = root;
      }
      else // traverse to the "right" child
        node = child;
    }

    else { // we read a 0
      // point to the "left" child
      child = (u8 *)((((u32)node)&(~1))+offset*2+2);

      if((*node)&0x80) { // "left" child is a data node
        // copy the child node into the output buffer and apply mask
        *out++ = (*child)&dataMask;
        size--;

        // start over at the root node
        node = root;
      }
      else // traverse to the "left" child
        node = child;
    }

    // shift to read next bit (read bit 31 to bit 0)
    mask >>= 1;
  }
}

