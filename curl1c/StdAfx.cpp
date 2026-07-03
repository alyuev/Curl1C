// stdafx.cpp : source file that includes just the standard includes
//	1sci.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "../_1Common/1cheaders/1cheaders.inl"
#include "../_1Common/dev_serv.cpp"
//#include "../_1Common/nocasemap.inl"
#include "../_1Common/CtrlEvents.cpp"

CURLSH *CurlShare;
bool bUseCacheDNS     = TRUE;
bool bUseCacheCONNECT = TRUE;
bool bUseCacheSSL     = TRUE;
bool bUseCacheCOOKIE  = FALSE;
bool bNeedInitCache   = FALSE;


