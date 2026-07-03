// 1scurl.cpp : implementation file
//

#include "stdafx.h"
#include <comutil.h>
#include <windows.h>


//#include "utex.h"

#include "Context.h"
//#include "../curl/include/curl/curl.h"
#include "Common.h"

//curl\lib\curl_multibyte.c
//#include "../curl/include/typecheck-gcc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BYTE Utf16BOM[] = {0xFF, 0xFE};
BYTE Utf8BOM[] = {0xEF, 0xBB, 0xBF};

enum CurlEvents {
    cecURL_Execution,
    ceLastEvent
};

stCtrlEvents CEBCtrlEvents[]  = {
    {"cURL_Execution", "cURL_Execution", 1}
};

struct curl_slist * headers = NULL;


BL_INIT_CONTEXT(CcURL);

static int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp) {
    const char *text;
    (void) handle;   /* prevent compiler warning */

    switch(type) {
    case CURLINFO_TEXT:
        DoMsgLine("== Info, %s", mmNone, data);
        return 0;
    case CURLINFO_HEADER_IN:
        text = "<= Recv header";
        break;
    case CURLINFO_HEADER_OUT:
        text = "=> Send header";
        break;
    case CURLINFO_DATA_IN:
        text = "<= Recv data";
        break;
    case CURLINFO_DATA_OUT:
        text = "=> Send data";
        break;
    case CURLINFO_SSL_DATA_IN:
        text = "<= Recv SSL data";
        break;
    case CURLINFO_SSL_DATA_OUT:
        text = "=> Send SSL data";
        break;
    default:
        return 0;
    }

    my_trace_dump(text, (unsigned char *)data, size);
    return 0;
}

int progress_function(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,  curl_off_t ulnow) {
    curl_progress_function(p, dltotal, dlnow, ultotal, ulnow);
    return 0;
}

void CcURL::AssignCurlCache() {

    if (bNeedInitCache){
        curl_share_cleanup(CurlShare);
        CurlShare = curl_share_init();
    }

    if (bUseCacheDNS) {
        curl_share_setopt(CurlShare,CURLSHOPT_SHARE,CURL_LOCK_DATA_DNS);
        TraceLog("CcURL::CacheDNS True");
    } else {
        curl_share_setopt(CurlShare,CURLSHOPT_UNSHARE,CURL_LOCK_DATA_DNS);
        TraceLog("CcURL::CacheDNS False");
    }
    if (bUseCacheCONNECT) {
        curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
        TraceLog("CcURL::CacheCONNECT True");
    } else {
        curl_share_setopt(CurlShare, CURLSHOPT_UNSHARE, CURL_LOCK_DATA_CONNECT);
        TraceLog("CcURL::CacheCONNECT False");
    }
    if (bUseCacheSSL) {
        curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
        TraceLog("CcURL::CacheSSL True");
    } else {
        curl_share_setopt(CurlShare, CURLSHOPT_UNSHARE, CURL_LOCK_DATA_SSL_SESSION);
        TraceLog("CcURL::CacheSSL False");
    }
    if (bUseCacheCOOKIE) {
        curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
        TraceLog("CcURL::CacheCOOKIE True");
    } else {
        curl_share_setopt(CurlShare, CURLSHOPT_UNSHARE, CURL_LOCK_DATA_COOKIE);
        TraceLog("CcURL::CacheCOOKIE False");
    }
    curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_PSL);
    curl_share_setopt(CurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_HSTS);

    TraceLog("curl_easy_setopt(m_curl, CURLOPT_SHARE, CurlShare)");
    curl_easy_setopt(m_curl, CURLOPT_SHARE, CurlShare);
    TraceLog("curl_easy_setopt(m_curl, CURLOPT_SHARE, CurlShare) DONE");
    bNeedInitCache = FALSE;
}

int CcURL::GetHeaders(CValue& retVal, CValue** ppParams) {

    retVal.CreateObject("ТаблицаЗначений");
    CValueTable* pVT = (static_cast<CValueTableContextData*>(retVal.GetContext()->GetInternalData()))->GetValueTable();

    const int iColCount = 1;

    CString str;
    const CType ctNum(NUMBER_TYPE_1C);
    const CType ctStr(STRING_TYPE_1C);

    pVT->Clear(TRUE);
    //pVT->AddColumn(str = "Number",		ctNum, str, 0, "", 0);
    pVT->AddColumn(str = "Заголовок", 	ctStr, str, 0, "", 0);

    std::vector<CValue> pv(iColCount);
    std::vector<CValue*> ppv(iColCount);
    for (int i = 0; i < iColCount; ++i)
        ppv[i] = &pv[i];

    struct curl_slist *item;
    if(!headers)
        return 0;
    item = headers;
    while(item->next) {
        item = item->next;
        //DoMsgLine("item.data %s",mmNone,item->data);
        pv[0] = item->data;
        pVT->AddRow(&ppv[0], -1);
    }

    return TRUE;
}

int CcURL::easy_init(CValue& retVal, CValue** ppParams) {

    /*
    curl_slist_free_all(headers);
    headers = NULL;

    curl_slist_free_all(recipients);
    recipients = NULL;

    m_curl = curl_easy_init();

    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(m_curl, CURLOPT_TRANSFER_ENCODING, 1L);
    curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
    */

    return TRUE;
}

int CcURL::easy_cleanup(CValue& retVal, CValue** ppParams) {

    curl_slist_free_all(headers);
    headers = NULL;

    curl_slist_free_all(recipients);
    recipients = NULL;

    m_curl = curl_easy_init();

    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(m_curl, CURLOPT_TRANSFER_ENCODING, 1L);
    curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

    return TRUE;

}

int CcURL::easy_setopt(CValue& retVal, CValue** ppParams) {

    const char* stroption = ppParams[0]->GetString();
    const char* optvalue = ppParams[1]->GetString();
    long intvalue =  ppParams[1]->GetNumeric();

    //DoMsgLine("stroption %s $d",mmNone,stroption,intvalue);
    TraceLog("CcURL::easy_setopt");

    CURLoption option = curl_options_from_string(stroption);

    if(option == CURLOPT_HTTPHEADER) {
        headers = curl_slist_append(headers, optvalue);
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
    } else if(option == CURLOPT_POSTFIELDS) {
        pSrc->Data = ppParams[1]->GetString();
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, optvalue);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, (long) strlen(pSrc->Data ));
    } else if(option == CURLOPT_MAIL_RCPT) {
        recipients = curl_slist_append(recipients, optvalue);
        curl_easy_setopt(m_curl, CURLOPT_MAIL_RCPT, recipients);
        isPosting=TRUE;
    } else {
        if (ppParams[1]->GetTypeCode() == 1) {
            curl_easy_setopt(m_curl, option, intvalue);
        } else if (ppParams[1]->GetTypeCode() == 2) {
            curl_easy_setopt(m_curl, option, optvalue);
        }
    }
    TraceLog("CcURL::easy_setopt done");
    return TRUE;
}

struct WriteThis {
    const char *readptr;
    size_t sizeleft;
};

static size_t read_callback(char *dest, size_t size, size_t nmemb, void *userp) {
    struct WriteThis *wt = (struct WriteThis *)userp;
    size_t buffer_size = size*nmemb;

    if(wt->sizeleft) {
        /* copy as much as possible from the source to the destination */
        size_t copy_this_much = wt->sizeleft;
        if(copy_this_much > buffer_size)
            copy_this_much = buffer_size;
        memcpy(dest, wt->readptr, copy_this_much);

        wt->readptr += copy_this_much;
        wt->sizeleft -= copy_this_much;
        return copy_this_much; /* we copied this many bytes */
    }

    return 0; /* no more data left to deliver */
}

int CcURL::easy_perform(CValue& retVal, CValue** ppParams) {
    TraceLog("CcURL::easy_perform starting");

    struct curl_progress prog;
    prog.lastruntime = 0;
    prog.curl = m_curl;

    delete pTrace;
    pTrace = new CCurlValue(); // Трассировка
    pTrace->ValType = ttTrace;
    pTrace->Type = ttMsg;

    FILE *fp = NULL;
    FILE *fd = NULL;
    std::string data="";
    struct stat file_info;

    struct WriteThis wt;

    DWORD t_start;

    //wt.readptr = data;
    //wt.sizeleft = strlen(data);


    CURLcode res;
    // установка параметров для отправки данных

    if (isMultipart) {
        TraceLog("CcURL::easy_perform Send Multipart data");

        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, PostFildsData);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, PostFildsData.GetLength());
        curl_easy_setopt(m_curl, CURLOPT_MIMEPOST, m_mime);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curl_swrite);

    } else if (pSrc->Type == ttVariable) {
        TraceLog("CcURL::easy_perform Send Data from variable");

        if(!pSrc->Data.IsEmpty()) {
            // отсылаемые данные из переменной
            TraceLog("CcURL::easy_perform ANSItoUTF8");
            PostFildsData = ANSItoUTF8(pSrc->Data);
            TraceLog("CcURL::easy_perform ANSItoUTF8 done");

            if (isPosting == FALSE) {
                curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, PostFildsData);
                //DoMsgLine("send data [%s]",mmNone,PostFildsData);
                curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, PostFildsData.GetLength());
            } else {
                wt.readptr = PostFildsData;
                wt.sizeleft = PostFildsData.GetLength();
                curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, read_callback);
                curl_easy_setopt(m_curl, CURLOPT_READDATA, &wt);
                curl_easy_setopt(m_curl, CURLOPT_UPLOAD, 1L);
            }
        } else {
            TraceLog("CcURL::easy_perform Отсылаемые данные пустые");
            //DoMsgLine("отсылаемые данные пустые");

        }
    } else if(pSrc->Type  == ttFile) {
        TraceLog("CcURL::easy_perform Send Data from file");

        fd = fopen(pSrc->Data, "rb"); /* open file to upload */
        if(!fd)
            RuntimeError("Не удалось открыть файл");
        if(fstat(fileno(fd), &file_info) != 0)
            RuntimeError("Файл имеет нулевой размер");
        curl_easy_setopt(m_curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(m_curl, CURLOPT_READDATA, fd);
        curl_easy_setopt(m_curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)file_info.st_size);
        //curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, read_callback);
    }

    // установка параметров для приема данных
    if(pDest->Type  == ttVariable) {
        TraceLog("CcURL::easy_perform Устанавливаем принимаемые данные в переменную");
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curl_swrite);
    } else if(pDest->Type  == ttFile) {
        // принимаемые данные в файл
        fp = fopen(pDest->Data, "wb");
        if (!fp)
            RuntimeError("Не удалось создать файл");

        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curl_fwrite);
    }

    // установка вывода строки состояния работы
    if(m_StatusLine == 1) {
        TraceLog("CcURL::easy_perform Установка вывода состояния работы");
        curl_easy_setopt(m_curl, CURLOPT_XFERINFODATA, &prog);
        curl_easy_setopt(m_curl, CURLOPT_XFERINFOFUNCTION, progress_function);
        curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
    } else {
        curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
    }

    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

    //time(&t_start);
    //DoMsgLine("curl_easy_perform",mmNone);
    //t_start = GetTickCount();
    TraceLog("CcURL::easy_perform Начало выполнения запроса");
    res = curl_easy_perform(m_curl);
    TraceLog("CcURL::easy_perform Запрос выполнен");
    //DoMsgLine("curl_easy_perform %d ms",mmNone,(GetTickCount()-t_start));

    int retcode;
    TraceLog("CcURL::easy_perform Начало получение кода возврата");
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &retcode);
    TraceLog("CcURL::easy_perform Завершено получение кода возврата");

    if(fp) {
        TraceLog("CcURL::easy_perform fclose(fp)");
        fclose(fp);
    }
    if(fd) {
        TraceLog("CcURL::easy_perform fclose(fd)");
        fclose(fd);
    }

    if(res != CURLE_OK) {
        RuntimeError("Ошибка получения данных %d %d ", res, retcode);
        return FALSE;
    }

    if(pDest->Type  == ttVariable) {
        TraceLog("CcURL::easy_perform UTF8toANSI(data.c_str()");
        pDest->Data = UTF8toANSI(data.c_str());
        TraceLog("CcURL::easy_perform UTF8toANSI(data.c_str() done");
    } else if(pDest->Type  == ttFile) {
        TraceLog("CcURL::easy_perform fclose(fp)");
        fclose(fp);
    }

    //DoMsgLine("CURLINFO_HTTP_CODE %d", mmNone, retcode);
    //DoMsgLine("m_data_to_send %s", mmNone, m_data_to_send);

    //*ppParams[0] = data.c_str();
    //DoMsgLine("%s",mmNone,data.c_str());

    retVal.SetType(CType(2));
    retVal = retcode;

    return TRUE;
}

int CcURL::Debug(CValue& retVal, CValue** ppParams) {

    int trace = ppParams[0]->GetNumeric();

    if(trace == 1) {
        m_trace = true;
        curl_easy_setopt(m_curl, CURLOPT_DEBUGFUNCTION, my_trace);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
    } else if(trace == 0) {
        m_trace = false;
        curl_easy_setopt(m_curl, CURLOPT_DEBUGFUNCTION, NULL);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 0L);
    }
    return TRUE;
}

int CcURL::StatusLine(CValue& retVal, CValue** ppParams) {
    int StatusLine = ppParams[0]->GetNumeric();
    if(StatusLine == 1) {
        m_StatusLine = true;
    } else if(StatusLine == 0) {
        m_StatusLine = false;
    }
    return TRUE;
}

CcURL::AddStatisticLine(CValueTable* pVT,CURLINFO info,char* textinfo) {
    curl_off_t retcode;
    int res;

    const int iColCount = 2;
    std::vector<CValue> pv(iColCount);
    std::vector<CValue*> ppv(iColCount);
    for (int i = 0; i < iColCount; ++i)
        ppv[i] = &pv[i];

    res = curl_easy_getinfo(m_curl, info, &retcode);
    pv[0] = textinfo;
    pv[1] = static_cast<long>(retcode);
    pVT->AddRow(&ppv[0], -1);
}

int CcURL::GetStatistics(CValue& retVal, CValue** ppParams) {

    retVal.CreateObject("ТаблицаЗначений");
    CValueTable* pVT = (static_cast<CValueTableContextData*>(retVal.GetContext()->GetInternalData()))->GetValueTable();

    const int iColCount = 2;

    CString str;
    const CType ctNum(NUMBER_TYPE_1C);
    const CType ctStr(STRING_TYPE_1C);

    pVT->Clear(TRUE);
    pVT->AddColumn(str = "Показатель", 	ctStr, str, 0, "", 0);
    pVT->AddColumn(str = "Значение", 	ctNum, str, 0, "", 0);

    AddStatisticLine(pVT,CURLINFO_TOTAL_TIME_T,      "TOTAL_TIME");
    AddStatisticLine(pVT,CURLINFO_NAMELOOKUP_TIME_T, "NAMELOOKUP_TIME");
    AddStatisticLine(pVT,CURLINFO_CONNECT_TIME_T,    "CONNECT_TIME");
    AddStatisticLine(pVT,CURLINFO_PRETRANSFER_TIME_T,"PRETRANSFER_TIME");
    AddStatisticLine(pVT,CURLINFO_STARTTRANSFER_TIME_T,"STARTTRANSFER_TIME");
    AddStatisticLine(pVT,CURLINFO_REDIRECT_TIME_T,   "REDIRECT_TIME");
    AddStatisticLine(pVT,CURLINFO_REDIRECT_COUNT,    "REDIRECT_COUNT");
    AddStatisticLine(pVT,CURLINFO_SIZE_UPLOAD_T,     "SIZE_UPLOAD");
    AddStatisticLine(pVT,CURLINFO_SPEED_UPLOAD_T,    "SPEED_UPLOAD");
    AddStatisticLine(pVT,CURLINFO_SIZE_DOWNLOAD_T,   "SIZE_DOWNLOAD");
    AddStatisticLine(pVT,CURLINFO_SPEED_DOWNLOAD_T,  "SPEED_DOWNLOAD");
    AddStatisticLine(pVT,CURLINFO_HEADER_SIZE,       "HEADER_SIZE");
    AddStatisticLine(pVT,CURLINFO_REQUEST_SIZE,      "REQUEST_SIZE");
    AddStatisticLine(pVT,CURLINFO_CONTENT_LENGTH_DOWNLOAD_T,"CONTENT_LENGTH_DOWNLOAD");
    AddStatisticLine(pVT,CURLINFO_CONTENT_LENGTH_UPLOAD_T,"CONTENT_LENGTH_UPLOAD");

    return TRUE;
}




