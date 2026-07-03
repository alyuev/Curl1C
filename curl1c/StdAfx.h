#ifndef HEADER_FD242D9FF81759A8
#define HEADER_FD242D9FF81759A8

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9D8CB2CA_B180_4D84_A0F3_C96E2D87B8AE__INCLUDED_)
#define AFX_STDAFX_H__9D8CB2CA_B180_4D84_A0F3_C96E2D87B8AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#include <atlbase.h>

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9D8CB2CA_B180_4D84_A0F3_C96E2D87B8AE__INCLUDED_)

#include "../_1Common/1cheaders/1cheaders.h"
#include "../_1Common/contextimpl.hpp"
#include "../_1Common/dev_serv.h"
#include "../_1Common/types1c.h"
#include "../_1Common/nocasemap.hpp"
#include "../_1Common/blptr.h"
#include "../_1Common/CtrlEvents.h"
#include "../_1Common/V7Control.h"

// #include <platform.h>
#include "curl.h"
//CURLSH *CurlShare;
#endif // header guard

