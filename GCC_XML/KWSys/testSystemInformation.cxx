/*============================================================================
  KWSys - Kitware System Library
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "kwsysPrivate.h"
#include KWSYS_HEADER(SystemInformation.hxx)
#include KWSYS_HEADER(ios/iostream)



// Work-around CMake dependency scanning limitation.  This must
// duplicate the above list of headers.
#if 0
# include "SystemInformation.hxx.in"
# include "kwsys_ios_iostream.h.in"
#endif

#define printMethod(inof, m) kwsys_ios::cout << #m << ": " \
<< info.m() << "\n"

#define printMethod2(inof, m, unit) kwsys_ios::cout << #m << ": " \
<< info.m() << " " << unit << "\n"

int testSystemInformation(int, char*[])
{
  kwsys::SystemInformation info;
  info.RunCPUCheck();
  info.RunOSCheck();
  info.RunMemoryCheck();
  printMethod(info, GetOSName);
  printMethod(info, GetHostname);
  printMethod(info, GetOSRelease);
  printMethod(info, GetOSVersion);
  printMethod(info, GetOSPlatform);
  printMethod(info, GetVendorString);
  printMethod(info, GetVendorID);
  printMethod(info, GetTypeID);
  printMethod(info, GetFamilyID);
  printMethod(info, GetModelID);
  printMethod(info, GetExtendedProcessorName);
  printMethod(info, GetProcessorSerialNumber);
  printMethod2(info, GetProcessorCacheSize, "KB");
  printMethod(info, GetLogicalProcessorsPerPhysical);
  printMethod2(info, GetProcessorClockFrequency, "MHz");
  printMethod(info, Is64Bits);
  printMethod(info, GetNumberOfLogicalCPU);
  printMethod(info, GetNumberOfPhysicalCPU);
  printMethod(info, DoesCPUSupportCPUID);
  printMethod(info, GetProcessorAPICID);
  printMethod2(info, GetTotalVirtualMemory, "MB");
  printMethod2(info, GetAvailableVirtualMemory, "MB");
  printMethod2(info, GetTotalPhysicalMemory, "MB");
  printMethod2(info, GetAvailablePhysicalMemory, "MB");

  //int GetProcessorCacheXSize(long int);
//  bool DoesCPUSupportFeature(long int);
  return 0;
}
