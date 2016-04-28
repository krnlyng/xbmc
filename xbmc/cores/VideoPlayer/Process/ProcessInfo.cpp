/*
 *      Copyright (C) 2005-2016 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "ProcessInfo.h"

#ifdef TARGET_RASPBERRY_PI
#include "linux/RBP.h"
#endif

CProcessInfo* CProcessInfo::CreateInstance()
{
  return new CProcessInfo();
}

// base class definitions
CProcessInfo::CProcessInfo()
{

}

CProcessInfo::~CProcessInfo()
{

}

EINTERLACEMETHOD CProcessInfo::GetFallbackDeintMethod()
{
#ifdef TARGET_RASPBERRY_PI
  return VS_INTERLACEMETHOD_DEINTERLACE_HALF;
#else
  return VS_INTERLACEMETHOD_DEINTERLACE;
#endif
}

bool CProcessInfo::AllowDTSHDDecode()
{
#ifdef TARGET_RASPBERRY_PI
  if (g_RBP.RasberryPiVersion() == 1)
    return false;
#else
  return true;
#endif
}
