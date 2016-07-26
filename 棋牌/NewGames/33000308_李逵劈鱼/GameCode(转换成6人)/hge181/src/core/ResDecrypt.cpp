
#include "ResDecrypt.h"

unsigned long DecryptData(unsigned char* data, unsigned long datasize)
{
  // Ω‚√‹ ˝æ›
  unsigned long dwXorKey = g_dwKey;
  unsigned long dwDecryptKey = g_dwKey;
  unsigned short* pwSeed = (unsigned short*)data;
  unsigned long* pdwXor = (unsigned long*)data;
  unsigned short wDecrypCount = (unsigned short)(datasize / sizeof(unsigned long));
  for (unsigned short i = 0; i < wDecrypCount; i++)
  {
    dwXorKey = SeedRandMap(*pwSeed++);
    dwXorKey |= ((unsigned long)SeedRandMap(*pwSeed++)) << 16;
    dwXorKey ^= g_dwKey;
    *pdwXor++ ^= dwDecryptKey;
    dwDecryptKey = dwXorKey;
  }

  for (unsigned long i = 0; i < datasize; ++i)
  {
    data[i] = g_DecryptByteMap[data[i]];
  }

  return datasize;
}