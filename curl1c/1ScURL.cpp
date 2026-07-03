// curl1c.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE My1ScURLDLL = { NULL, NULL };
extern CURLSH *CurlShare;

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
    UNREFERENCED_PARAMETER(lpReserved);

    if (dwReason == DLL_PROCESS_ATTACH) {
        TRACE0("curl1c.DLL Initializing!\n");

        if (!AfxInitExtensionModule(My1ScURLDLL, hInstance))
            return 0;

        context_obj::CContextBase::InitAllContextClasses();
        Init1CGlobal(hInstance);
        new CDynLinkLibrary(My1ScURLDLL);

        curl_global_init(CURL_GLOBAL_ALL);

        CurlShare = curl_share_init();
        curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS );
        curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
        curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);

    } else if (dwReason == DLL_PROCESS_DETACH) {
        TRACE0("curl1c.DLL Terminating!\n");
        curl_share_cleanup(CurlShare);
        curl_global_cleanup();
        context_obj::CContextBase::DoneAllContextClasses();
        AfxTermExtensionModule(My1ScURLDLL);
    }
    return 1;
}
