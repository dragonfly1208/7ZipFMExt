// ZipRegistry.h

#ifndef __ZIP_REGISTRY_H
#define __ZIP_REGISTRY_H

#include "MyTypes.h"
//#include "../../../Common/MyString.h"

//#include "ExtractMode.h"

struct CContextMenuInfo
{
  CBoolPair Cascaded;
  CBoolPair MenuIcons;
  CBoolPair ElimDup;

  bool Flags_Def;
  UINT32 Flags;

  void Save() const;
  void Load();
};

CString getAppPath();

#endif
