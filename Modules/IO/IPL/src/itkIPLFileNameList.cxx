/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkIPLFileNameList.h"

#include <algorithm>
#include <functional>

namespace itk
{
struct IPLFileSortInfo_ascend_compare : public std::greater<IPLFileSortInfo *>
{
private:
  bool
  sort_IPLFileSortInfo_ascend_compare(IPLFileSortInfo * item1, IPLFileSortInfo * item2)
  {

    if (item1->GetImageNumber() < item2->GetImageNumber())
    {
      return true;
    }
    if (item1->GetImageNumber() > item2->GetImageNumber())
    {
      return false;
    }
    if (item1->GetEchoNumber() < item2->GetEchoNumber())
    {
      return true;
    }
    if (item1->GetEchoNumber() > item2->GetEchoNumber())
    {
      return false;
    }
    if (item1->GetSliceLocation() < item2->GetSliceLocation())
    {
      return true;
    }
    if (item1->GetSliceLocation() > item2->GetSliceLocation())
    {
      return false;
    }
    return (item1->GetImageFileName() < item2->GetImageFileName());
  }

public:
  bool
  operator()(IPLFileSortInfo * item1, IPLFileSortInfo * item2)
  {
    return sort_IPLFileSortInfo_ascend_compare(item1, item2);
  }
};

struct IPLFileSortInfo_descend_compare : public std::greater<IPLFileSortInfo *>
{
private:
  bool
  sort_IPLFileSortInfo_descend_compare(IPLFileSortInfo * item1, IPLFileSortInfo * item2)
  {
    if (item1->GetImageNumber() < item2->GetImageNumber())
    {
      return false;
    }
    if (item1->GetImageNumber() > item2->GetImageNumber())
    {
      return true;
    }
    if (item1->GetEchoNumber() < item2->GetEchoNumber())
    {
      return false;
    }
    if (item1->GetEchoNumber() > item2->GetEchoNumber())
    {
      return true;
    }
    if (item1->GetSliceLocation() < item2->GetSliceLocation())
    {
      return false;
    }
    if (item1->GetSliceLocation() > item2->GetSliceLocation())
    {
      return true;
    }
    return (item1->GetImageFileName() > item2->GetImageFileName());
  }

public:
  bool
  operator()(IPLFileSortInfo * item1, IPLFileSortInfo * item2)
  {
    return sort_IPLFileSortInfo_descend_compare(item1, item2);
  }
};

struct IPLFileSortInfo_ascendbyname_compare : public std::greater<IPLFileSortInfo *>
{
public:
  bool
  operator()(IPLFileSortInfo * item1, IPLFileSortInfo * item2)
  {
    return (item1->GetImageFileName() < item2->GetImageFileName());
  }
};

struct IPLFileSortInfo_descendbyname_compare : public std::greater<IPLFileSortInfo *>
{
public:
  bool
  operator()(IPLFileSortInfo * item1, IPLFileSortInfo * item2)
  {
    return (item1->GetImageFileName() >= item2->GetImageFileName());
  }
};

IPLFileSortInfo::~IPLFileSortInfo() = default;

IPLSetMacroDefinition(IPLFileSortInfo, ImageFileName, std::string);
IPLGetMacroDefinition(IPLFileSortInfo, ImageFileName, std::string);
IPLSetMacroDefinition(IPLFileSortInfo, SliceLocation, float);
IPLGetMacroDefinition(IPLFileSortInfo, SliceLocation, float);
IPLSetMacroDefinition(IPLFileSortInfo, SliceOffset, int);
IPLGetMacroDefinition(IPLFileSortInfo, SliceOffset, int);
IPLSetMacroDefinition(IPLFileSortInfo, EchoNumber, int);
IPLGetMacroDefinition(IPLFileSortInfo, EchoNumber, int);
IPLSetMacroDefinition(IPLFileSortInfo, ImageNumber, int);
IPLGetMacroDefinition(IPLFileSortInfo, ImageNumber, int);
IPLSetMacroDefinition(IPLFileSortInfo, Data, void *);
IPLGetMacroDefinition(IPLFileSortInfo, Data, const void *);

IPLFileNameList::~IPLFileNameList()
{
  auto it = begin();
  auto itend = end();

  while (it != itend)
  {
    delete (*it);
    ++it;
  }
}

void
IPLFileNameList::sortImageListAscend()
{
  std::sort(m_List.begin(), m_List.end(), IPLFileSortInfo_ascend_compare());
}

void
IPLFileNameList::sortImageListDescend()
{
  //  qsort (fnList->Info, fnList->numImageInfoStructs, sizeof
  // (IPLFileSortInfo),
  //     qsort_IPLFileSortInfo_descend_compar);
  std::sort(m_List.begin(), m_List.end(), IPLFileSortInfo_descend_compare());
}

void
IPLFileNameList::sortImageList()
{
  if (m_SortOrder == SortByNameAscend)
  {
    std::sort(m_List.begin(), m_List.end(), IPLFileSortInfo_ascendbyname_compare());
  }
  else if (m_SortOrder == SortByNameDescend)
  {
    std::sort(m_List.begin(), m_List.end(), IPLFileSortInfo_descendbyname_compare());
  }
  else if (m_SortOrder == SortGlobalDescend)
  {
    std::sort(m_List.begin(), m_List.end(), IPLFileSortInfo_descend_compare());
  }
  else if (m_SortOrder == SortGlobalAscend)
  {
    std::sort(m_List.begin(), m_List.end(), IPLFileSortInfo_ascend_compare());
  }
}

IPLSetMacroDefinition(IPLFileNameList, XDim, int);
IPLGetMacroDefinition(IPLFileNameList, XDim, int);
IPLSetMacroDefinition(IPLFileNameList, YDim, int);
IPLGetMacroDefinition(IPLFileNameList, YDim, int);
IPLSetMacroDefinition(IPLFileNameList, XRes, float);
IPLGetMacroDefinition(IPLFileNameList, XRes, float);
IPLSetMacroDefinition(IPLFileNameList, YRes, float);
IPLGetMacroDefinition(IPLFileNameList, YRes, float);
IPLSetMacroDefinition(IPLFileNameList, Key1, int);
IPLGetMacroDefinition(IPLFileNameList, Key1, int);
IPLSetMacroDefinition(IPLFileNameList, Key2, int);
IPLGetMacroDefinition(IPLFileNameList, Key2, int);
IPLSetMacroDefinition(IPLFileNameList, SortOrder, int);
} // namespace itk
