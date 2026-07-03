#ifndef HEADER_A1DE48C53ED9E2F
#define HEADER_A1DE48C53ED9E2F

// Context.h
#pragma once

#include "Common.h"
#include <string>
//#include "../curl/include/curl/curl.h"
//#include "../curl/lib/curl_base64.h"
//#include "base64/base64.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "base64.h"

class CHTTPRequest;
class CcURL_EASY;
class CcURL;
class CSource;
class CurlValue;
class CCurlValue;

class CurlValue {
  public:
    CurlValue() {}
    virtual ~CurlValue() {}

  protected:
    int m_type;
    int m_val;
};

class CurlMime {
  public:
    CurlMime() {}
    virtual ~CurlMime() {}

};

enum tType {ttVariable = 0, ttFile = 1, ttMsg = 2};
enum tValType {ttSrc=0, ttDest=1, ttTrace=2};
enum tEncoding {ttASCII=0, ttUTF8=1, ttBIN=2};
bool isTrace;

extern CURLSH *CurlShare;

extern bool bUseCacheDNS;
extern bool bUseCacheCONNECT;
extern bool bUseCacheSSL;
extern bool bUseCacheCOOKIE;
extern bool bNeedInitCache;


void TraceLog(char* msg) {
    if (isTrace)
        DoMsgLine(msg,mmNone);
}

class CCurlValue : public CContextImpl<CCurlValue, no_init_done, FALSE>, public CurlValue {
    friend class CCurl;

  public:

    int Type;
    int Encoding;
    int ValType;
    CString Data;

    CCurlValue() : CurlValue() {
        Type = ttVariable;
        Encoding = ttASCII;
    }

    virtual ~CCurlValue() {
        //TraceLog("CCurlValue Destructor");
        Data.ReleaseBuffer();
        Data.FreeExtra();
    }

    BL_BEGIN_CONTEXT("CurlValue", "CurlValue");

    BL_PROP_RW(Type, "Тип") {
        value = Type;
        return TRUE;
    }
    BL_PROP_WRITE(Type) {
        int _type = value.GetNumeric();
        if(_type >= 0 && _type <= 2) {
            Type = value.GetNumeric();
            return TRUE;
        } else{
            RuntimeError("Неверное значение переменной");
            return FALSE;
        }
    }

    BL_PROP_RW(Data, "Данные") {
        value = Data;
        return TRUE;
    }
    BL_PROP_WRITE(Data) {
        Data = value.GetString();
        return TRUE;
    }

    BL_PROP_RW(Encoding, "Кодировка") {
        value = Encoding;
        return TRUE;
    }
    BL_PROP_WRITE(Encoding) {
        Encoding = value.GetNumeric();
        return TRUE;
    }

    BL_END_CONTEXT();

  private:
    //CString Data;
  protected:
    //CString Data;

};

class CCurlMime : public CContextImpl<CCurlMime, no_init_done, FALSE>, public CurlMime {

    friend class CCurl;

  public:
    curl_mime *p_mime;
    CURL *p_curl;

    //CCurlMime(CURL* m_curl) : CurlMime() {
    CCurlMime(CURL *m_curl,curl_mime *m_mime) {
        TraceLog("CurlMime Constructor");
        p_curl = m_curl;
        p_mime = m_mime;
        TraceLog("CurlMime Constructor done");
    }

    //virtual ~CCurlMime() {
    ~CCurlMime() {
        TraceLog("CurlMime Destructor");
        curl_mime_free(p_mime);
        p_mime = NULL;
        TraceLog("CurlMime Destructor done");
    }

    BL_BEGIN_CONTEXT("CurlMime", "CurlMime");

    BL_PROC(AddPart, "Добавить", 4) {
        //curl_mime *mime; //public
        curl_mimepart *part;
        part = curl_mime_addpart(p_mime);

        CString StrName     = ppParams[0]->GetString();
        CString StrFileName = ppParams[1]->GetString();
        CString StrType     = ppParams[2]->GetString();
        CString StrData     = ppParams[3]->GetString();

        //DoMsgLine("Set MIME data: %s %s %s",mmNone,CStrType,CStrName,CStrData);

        if (!StrName.IsEmpty()) {
            curl_mime_name(part,(LPCTSTR)StrName);
        }
        if (!StrFileName.IsEmpty()) {
            curl_mime_filename(part,(LPCTSTR)StrFileName);
            curl_mime_filedata(part,(LPCTSTR)StrFileName);
        }
        if (!StrType.IsEmpty()) {
            curl_mime_type(part,(LPCTSTR)StrType);
        }
        if (!StrData.IsEmpty()) {
            curl_mime_data(part,(LPCTSTR)StrData,CURL_ZERO_TERMINATED);
        }
        return TRUE;
    }

    BL_END_CONTEXT();

};

class CcURL: public CContextImpl<CcURL> {
    friend class CCurlValue;
    friend class CCurlMime;

  public:

    CURL *m_curl;
    curl_mime *m_mime;
    bool m_trace;

    //curl_mime *mime;
    //struct curl_slist *headers;
    //struct curl_slist *recipients;

    CcURL() {

        TraceLog("CcURL Constructor");

        m_curl = curl_easy_init();
        AssignCurlCache();

        m_trace = FALSE;
        isTrace = FALSE;

        headers = NULL;
        headers = curl_slist_append(headers, "pragma:");

        recipients = NULL;

        pSrc = new CCurlValue(); // Отправляемые данные
        pSrc->ValType = ttSrc;

        pDest = new CCurlValue(); // Получаемые данные
        pDest->ValType = ttDest;

        pTrace = new CCurlValue(); // Трассировка
        pTrace->ValType = ttTrace;
        pTrace->Type = ttMsg;

        m_mime = curl_mime_init(m_curl);
        pCurlMime = new CCurlMime(m_curl,m_mime);

        isMultipart = FALSE;
        m_bodyCompression = 0;

        curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(m_curl, CURLOPT_TRANSFER_ENCODING, 1L);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);

        TraceLog("CcURL Constructor done");
    };

    ~CcURL() {
        TraceLog("CcURL Destructor");

        TraceLog("CcURL Destructor curl_slist_free_all(headers)");
        curl_slist_free_all(headers);
        TraceLog("CcURL Destructor curl_slist_free_all(recipients)");
        curl_slist_free_all(recipients);
        //TraceLog("CcURL Destructor curl_mime_free(m_mime);");
        //curl_mime_free(m_mime);
        /*
        TraceLog("CcURL Destructor delete m_mime;");
        if(m_mime){
            delete m_mime;
        }
        */

        TraceLog("CcURL Destructor curl_easy_cleanup(m_curl);");
        curl_easy_cleanup(m_curl);

        //TraceLog("CcURL Destructor curl_mime_free(m_mime);");
        //curl_mime_free(m_mime);
        //delete m_mime;


        TraceLog("CcURL Destructor delete pSrc;");
        delete pSrc;
        TraceLog("CcURL Destructor delete pDest;");
        delete pDest;
        TraceLog("CcURL Destructor delete pTrace;");
        delete pTrace;
        TraceLog("CcURL Destructor delete pCurlMime;");
        //pCurlMime = NULL;
        //delete[] pCurlMime;
        delete[] pCurlMime;

        /*
        TraceLog("CcURL Destructor delete headers;");
        delete headers;
        TraceLog("CcURL Destructor delete recipients;");
        delete recipients;
        */

        TraceLog("CcURL Destructor done");
    }

    CString PostFildsData;
    //pTrace;

    void AssignCurlCache();

    int m_StatusLine; // выводить ли строку состояния работы
    //CcURL::AddStatisticLine(CURLINFO info);
    AddStatisticLine(CValueTable* pVT,CURLINFO info,char* text);

    BL_BEGIN_CONTEXT("cURL1C", "Карлик");

    BL_FUNC(Version, "Версия", 0) {
        retVal = 17;
        return TRUE;
    }

    /*
    BL_FUNC(GetRoundMode, "GetRoundMode", 0) {
        retVal = CNumeric::GetRoundMode();
        return TRUE;
    }
    */

    BL_FUNC(ComponentsVersion, "ВерсииКомпонентов", 0) {
        CString msg;
        curl_version_info_data *curlinfo = curl_version_info(CURLVERSION_NOW);
        msg.Format("curl %s, ssl %s, libz %s, brotli %s, zstd %s",
                   curlinfo->version,
                   curlinfo->ssl_version,
                   curlinfo->libz_version,
                   curlinfo->brotli_version,
                   curlinfo->zstd_version
                   );
        retVal = msg;
        return TRUE;
    }

    BL_FUNC(easy_init, "Инит", 0);
    BL_FUNC(easy_cleanup, "ОчиститьПараметры", 0);
    BL_FUNC(easy_setopt, "УстановитьПараметр", 2);
    BL_FUNC(easy_perform, "Выполнить", 0);
    BL_FUNC(CompressBody, "СжатьТело", 1);
    BL_FUNC(CompressFile, "СжатьФайл", 3);

    BL_FUNC(Debug, "Отладка", 1);
    BL_FUNC(Trace, "Трассировка", 1) {
        int trace = ppParams[0]->GetNumeric();
        if (trace==0) {
            isTrace=FALSE;
        } else {
            isTrace=TRUE;
        }
        return TRUE;
    };

    BL_FUNC(StatusLine, "ВыводитьСостояние",1);
    BL_FUNC(GetHeaders, "ПолучитьЗаголовки", 0);
    BL_FUNC(GetStatistics, "ПолучитьСтатистику", 0);

    BL_PROC(AssignCurlCache, "ОчиститьКэш",0){
        bNeedInitCache   = TRUE;
        AssignCurlCache();
        return TRUE;
    };
    BL_FUNC(UseCache,"ИспользоватьКэш",4){
        bNeedInitCache   = TRUE;
        bUseCacheDNS     = ppParams[0]->GetNumeric()==0 ? FALSE : TRUE;
        bUseCacheCONNECT = ppParams[1]->GetNumeric()==0 ? FALSE : TRUE;
        bUseCacheSSL     = ppParams[2]->GetNumeric()==0 ? FALSE : TRUE;
        bUseCacheCOOKIE  = ppParams[3]->GetNumeric()==0 ? FALSE : TRUE;
        AssignCurlCache();
        return TRUE;
    }

    BL_FUNC(mime_init, "СоздатьЗапрос", 0) {
        isMultipart=TRUE;
        retVal.AssignContext(pCurlMime);
        return TRUE;
    }

    BL_FUNC(free_all, "ОчиститьСтроки", 0) {
        //curl_slist_free_all(headers);
        return TRUE;
    }

    BL_PROP_RO(Source, "Источник") {
        value.Reset();
        value.AssignContext(pSrc);
        return TRUE;
    }

    BL_PROP_RO(Dest, "Приемник") {
        value.Reset();
        value.AssignContext(pDest);
        return TRUE;
    }

    BL_FUNC(ToUTF8, "ToUTF8", 1) {
        retVal.type = 2;
        retVal.m_String = ANSItoUTF8(ppParams[0]->GetString());
        return TRUE;
    }

    BL_FUNC(FromUTF8, "FromUTF8", 1) {
        retVal.type = 2;
        retVal.m_String = UTF8toANSI(ppParams[0]->GetString());
        return TRUE;
    }

    BL_FUNC(ToURL, "ToURL", 1) {
        CString Input = ppParams[0]->GetString();
        char *Output = curl_easy_escape(m_curl, Input, Input.GetLength());
        retVal = Output;
        curl_free(Output);
        return TRUE;
    }

    BL_FUNC(FromURL, "FromURL", 1) {
        //https://curl.se/libcurl/c/curl_easy_unescape.html
        CString Input = ppParams[0]->GetString();
        int decodelen;
        char *Output = curl_easy_unescape(m_curl, Input, Input.GetLength(),&decodelen);
        retVal = Output;
        curl_free(Output);
        return TRUE;
    }

    BL_FUNC(ToBase64, "ToBase64", 1) {
        CString Input = ppParams[0]->GetString();

        if (Input.IsEmpty()) {

            retVal="";
            return TRUE;
        }

        std::string input((LPCTSTR)Input);

        static std::string encoded;
        static std::string decoded;

        if (!Base64::Encode(input, &encoded)) {
            DoMsgLine("Ошибка кодировки строки в BASE64",mmRedErr);
            return FALSE;
        } else {
            CString cs(encoded.c_str());
            retVal = cs;
            return TRUE;
        }
        return TRUE;
    }

    BL_FUNC(FromBase64, "FromBase64", 1) {
        CString Input = ppParams[0]->GetString();

        if (Input.IsEmpty()) {
            retVal="";
            return TRUE;
        }

        std::string input((LPCTSTR)Input);

        static std::string encoded;
        static std::string decoded;

        if (!Base64::Decode(input, &encoded)) {
            DoMsgLine("Ошибка разкодировки строки из BASE64",mmRedErr);
            return FALSE;
        } else {
            CString cs(encoded.c_str());
            retVal = cs;
            return TRUE;
        }
        return TRUE;
    }

    BL_END_CONTEXT();

  private:

    bool isPosting;
    bool isMultipart;
    bool idInited;

    //CCtrlEventManager m_EventManager;
    //CGetDoc7* m_pDoc;

    CCurlValue *pSrc;
    CCurlValue *pDest;
    CCurlValue *pTrace;
    CCurlMime  *pCurlMime;

    struct curl_slist *headers;
    struct curl_slist *recipients;

    int m_bodyCompression;
    std::string m_compressedBody;

};



#endif // header guard

