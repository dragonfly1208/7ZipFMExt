// ContextMenuFlags.h

#ifndef __CONTEXT_MENU_FLAGS_H
#define __CONTEXT_MENU_FLAGS_H

namespace NContextMenuFlags
{
  const UINT32 kNull = 0;
  const UINT32 kExtract = 1 << 0;
  const UINT32 kExtractHere = 1 << 1;
  const UINT32 kExtractTo = 1 << 2;

  const UINT32 kTest = 1 << 4;
  const UINT32 kOpen = 1 << 5;
  const UINT32 kOpenAs = 1 << 6;

  const UINT32 kCompress = 1 << 8;
  const UINT32 kCompressTo7z = 1 << 9;
  const UINT32 kCompressEmail = 1 << 10;
  const UINT32 kCompressTo7zEmail = 1 << 11;
  const UINT32 kCompressToZip = 1 << 12;
  const UINT32 kCompressToZipEmail = 1 << 13;

  const UINT32 kCRC = (UINT32)1 << 31;
}

#endif
