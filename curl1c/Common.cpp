
#include "Common.h"

// выводит текст в строку состояния 1С
int curl_progress_function(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,  curl_off_t ulnow) {

    CString msg;

    struct curl_progress *myp = (struct curl_progress *)p;
    CURL *curl = myp->curl;

    double curtime = 0;

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

    if((curtime - myp->lastruntime) >= 1) {
        myp->lastruntime = curtime;
    } else
        return 0;


    double ulspeed = 0;
    double dlspeed = 0;

    curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &ulspeed);
    curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &dlspeed);

    msg.Format(" Отправлено: %" CURL_FORMAT_CURL_OFF_T
               "/%" CURL_FORMAT_CURL_OFF_T " (%g kb/s)"
               " Загружено: %" CURL_FORMAT_CURL_OFF_T
               "/%" CURL_FORMAT_CURL_OFF_T " (%g kb/s)"
               , ultotal, ulnow, ulspeed / 1024, dlnow, dltotal, dlspeed / 1024);

    DoStsLine("%s ", msg);
    //DoMsgLine("tick");

    return 0;
}

size_t curl_swrite(void *ptr, size_t size, size_t count, void *stream) {
    ((string*)stream)->append((char*)ptr, 0, size * count);
    return size * count;
}


size_t curl_fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

size_t curl_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fread(ptr, size, nmemb, stream);
}

CURLoption curl_options_from_string(const char *key) {
    int i;
    for(i = 0; i < NKEYS; i++) {
        if(strcmp(lookuptable[i].key, key) == 0)
            return lookuptable[i].value;
    }
    RuntimeError("Неизвестный тип CURLOPT %s", key);
    return (CURLoption)NULL;
}

int CallAsProc1(CBLContext* pMod, int iProcNum, CValue const & param1) {
    const CValue *pParams[1] = {0};
    pParams[0] = &param1;
    return pMod->CallAsProc(iProcNum, const_cast<CValue**>(pParams));
}

void my_trace_dump(const char *text, const unsigned char *ptr, size_t size) {
    size_t i;
    size_t c;
    int width = 0x80;
    bool nohex = true;
    CString msg;
    CString msg2;

    msg.Format("%s, %d bytes (0x%x)\n", text, (int)size, (int)size);
    DoMsgLine("%s", mmNone, msg);

    for(i = 0; i < size; i += width) {
        msg.Format("%04x: ", (int)i);

        for(c = 0; (c < width) && (i + c < size); c++) {
            /* check for 0D0A; if found, skip past and start a new line of output */
            if(nohex && (i + c + 1 < size) && (ptr[i + c] == 0x0D) && (ptr[i + c + 1] == 0x0A)) {
                i += (c + 2 - width);
                break;
            }

            //msg2.Format("%c", ((ptr[i + c] >= 0x20) && (ptr[i + c] < 0x80)) ? ptr[i + c] : '.');
            msg2.Format("%c", (ptr[i+c] >= 0x20) ? ptr[i + c] : '.');
            msg += msg2;

            /* check again for 0D0A, to avoid an extra \n if it's at width */
            if(nohex &&
                    (i + c + 2 < size) && (ptr[i + c + 1] == 0x0D) && (ptr[i + c + 2] == 0x0A)) {
                i += (c + 3 - width);
                break;
            }
        }
        DoMsgLine("%s", mmNone, msg);
    }
}


CString ANSItoUTF8(const CString& Input) {

    int size = MultiByteToWideChar(CP_ACP,0,Input,-1,0,0);
    LPWSTR Temp = new WCHAR[size];
    MultiByteToWideChar(CP_ACP,0,Input,-1,Temp,size);
    size = WideCharToMultiByte(CP_UTF8,0,Temp,-1,0,0,0,0);
    LPSTR Output = new CHAR[size];
    WideCharToMultiByte(CP_UTF8,0,Temp,-1,Output,size,0,0);
    CString Result(Output);
    delete[] Temp;
    delete[] Output;
    return Result;
}

CString UTF8toANSI(const CString& Input) {
    int size = MultiByteToWideChar(CP_UTF8,0,Input,-1,0,0);
    LPWSTR Temp = new WCHAR[size];
    MultiByteToWideChar(CP_UTF8,0,Input,-1,Temp,size);
    size = WideCharToMultiByte(CP_ACP,0,Temp,-1,0,0,0,0);
    char* Output = new CHAR[size];
    WideCharToMultiByte(CP_ACP,0,Temp,-1,Output,size,0,0);
    CString Result(Output);
    delete[] Temp;
    delete[] Output;
    return Result;
}

