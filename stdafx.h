// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#ifndef WINVER  
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#endif

#include <Windows.h>

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlcrack.h>
#include <atlmisc.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <memory>
#include <string>
#include <functional>
